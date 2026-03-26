#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <iostream>
#include <aio.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <ctime>
#include <atomic>
#include <sys/stat.h>
#include <signal.h>

struct CopyContext {
    int src_fd;
    int dst_fd;
    std::atomic<int> pending{0};
    timespec t0;

    CopyContext(int s, int d) : src_fd(s), dst_fd(d) {
        clock_gettime(CLOCK_MONOTONIC, &t0);
    }

    double elapsed() const {
        timespec now{};
        clock_gettime(CLOCK_MONOTONIC, &now);
        return (now.tv_sec - t0.tv_sec) +
               (now.tv_nsec - t0.tv_nsec) * 1e-9;
    }
};

struct Task {
    aiocb cb{};
    char* data;
    bool is_write;
    CopyContext* ctx;

    Task(size_t size, CopyContext* c)
        : data(new char[size]), is_write(false), ctx(c) {
        memset(&cb, 0, sizeof(cb));
    }

    ~Task() {
        delete[] data;
    }
};

void completion(sigval_t sig) {
    Task* t = static_cast<Task*>(sig.sival_ptr);
    CopyContext* ctx = t->ctx;

    ssize_t ret = aio_return(&t->cb);
    if (ret < 0) {
        perror("aio_return");
        ctx->pending--;
        delete t;
        return;
    }

    if (!t->is_write) {
        // switch to write
        t->is_write = true;
        t->cb.aio_fildes = ctx->dst_fd;
        t->cb.aio_nbytes = ret;

        if (aio_write(&t->cb) < 0) {
            perror("aio_write");
            ctx->pending--;
            delete t;
        }
    } else {
        ctx->pending--;
        delete t;
    }
}

double async_copy(const char* src, const char* dst,
                  size_t chunk, int max_ops) {

    int in = open(src, O_RDONLY);
    if (in < 0) {
        perror("open src");
        return -1;
    }

    int out = open(dst, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (out < 0) {
        perror("open dst");
        close(in);
        return -1;
    }

    struct stat st{};
    fstat(in, &st);
    off_t total = st.st_size;

    off_t pos = 0;

    auto* ctx = new CopyContext(in, out);

    while (pos < total || ctx->pending > 0) {

        while (ctx->pending < max_ops && pos < total) {

            size_t len = std::min((off_t)chunk, total - pos);

            Task* t = new Task(len, ctx);

            t->cb.aio_fildes = in;
            t->cb.aio_buf = t->data;
            t->cb.aio_nbytes = len;
            t->cb.aio_offset = pos;

            t->cb.aio_sigevent.sigev_notify = SIGEV_THREAD;
            t->cb.aio_sigevent.sigev_notify_function = completion;
            t->cb.aio_sigevent.sigev_value.sival_ptr = t;

            ctx->pending++;

            if (aio_read(&t->cb) < 0) {
                perror("aio_read");
                ctx->pending--;
                delete t;
                break;
            }

            pos += len;
        }

        usleep(100); // prevent CPU spin
    }

    double time = ctx->elapsed();

    close(in);
    close(out);
    delete ctx;

    return time;
}

int main(int argc, char** argv) {
    if (argc < 5) {
        std::cout << "Usage: ./linux <src> <dst> <block_kb> <max_ops>\n";
        return 1;
    }

    size_t block = static_cast<size_t>(atoi(argv[3])) * 1024;
    int ops = atoi(argv[4]);

    double t = async_copy(argv[1], argv[2], block, ops);

    printf("Done. Time: %.4f sec\n", t);
    return 0;
}
