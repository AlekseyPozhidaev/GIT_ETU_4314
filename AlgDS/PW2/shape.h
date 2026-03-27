//======== Файл shape.h — библиотека фигур =========
#include <iostream>
#include <list>
#include <string>
#include "screen.h"
#include "errors.h"

using std::list;
using std::string;

//== 1. Поддержка экрана в форме матрицы символов ==
char screen[YMAX][XMAX];
enum color { black = '*', white = '.' };

void screen_init() {
    for (auto y = 0; y < YMAX; ++y)
        for (auto &x : screen[y]) x = white;
}

void screen_destroy() {
    for (auto y = 0; y < YMAX; ++y)
        for (auto &x : screen[y]) x = black;
}

void put_point(int a, int b) {
    if (on_screen(a, b)) screen[b][a] = black;
}

void put_line(int x0, int y0, int x1, int y1) {
    int dx = 1;
    int a = x1 - x0; if (a < 0) dx = -1, a = -a;
    int dy = 1;
    int b = y1 - y0; if (b < 0) dy = -1, b = -b;
    int two_a = 2 * a;  // ИСПРАВЛЕНО: было 2a
    int two_b = 2 * b;
    int xcrit = -b + two_a;
    int eps = 0;
    for (;;) {
        put_point(x0, y0);
        if (x0 == x1 && y0 == y1) break;
        if (eps <= xcrit) x0 += dx, eps += two_b;
        if (eps >= a || a < b) y0 += dy, eps -= two_a;
    }
}

void screen_clear() { screen_init(); }

void screen_refresh() {
    for (int y = YMAX - 1; 0 <= y; --y) {
        for (auto x : screen[y]) std::cout << x;
        std::cout << '\n';
    }
}

//== Вспомогательные функции проверки границ ==
inline void check_bounds(int x, int y, const string& context = "") {
    if (!on_screen(x, y)) {
        throw OutOfScreenException(" (" + std::to_string(x) + "," + std::to_string(y) + ") " + context);
    }
}

inline void check_rect_bounds(int x1, int y1, int x2, int y2, const string& context = "") {
    check_bounds(x1, y1, context);
    check_bounds(x2, y2, context);
    check_bounds(x1, y2, context);
    check_bounds(x2, y1, context);
}

//== 2. Библиотека фигур ==
struct shape {
    static list<shape*> shapes;
    shape() { shapes.push_back(this); }
    
    virtual point north() const = 0;
    virtual point south() const = 0;
    virtual point east() const = 0;
    virtual point west() const = 0;
    virtual point neast() const = 0;
    virtual point seast() const = 0;
    virtual point nwest() const = 0;
    virtual point swest() const = 0;
    virtual void draw() = 0;
    virtual void move(int, int) = 0;
    virtual void resize(double) = 0;
    virtual ~shape() { shapes.remove(this); }
};

list<shape*> shape::shapes;

void shape_refresh() {
    screen_clear();
    for (auto p : shape::shapes) p->draw();
    screen_refresh();
}

class rotatable : virtual public shape {
protected:
    enum class rotated { left, no, right };
    rotated state;
public:
    rotatable(rotated r = rotated::no) : state(r) { }
    void rotate_left() { state = rotated::left; }
    void rotate_right() { state = rotated::right; }
};

class reflectable : virtual public shape {
protected:
    bool hor, vert;
public:
    reflectable(bool h = false, bool v = false) : hor(h), vert(v) { }
    void flip_horisontally() { hor = !hor; }
    void flip_vertically() { vert = !vert; }
};

class line : public shape {
protected:
    point w, e;
public:
    line(point a, point b) : w(a), e(b) {
        check_bounds(w.x, w.y, "line start");
        check_bounds(e.x, e.y, "line end");
    }
    line(point a, int L) : w(point(a.x + L - 1, a.y)), e(a) {
        check_bounds(w.x, w.y, "line start");
        check_bounds(e.x, e.y, "line end");
    }
    
    // Геттеры для доступа к координатам из производных классов
    point get_w() const { return w; }
    point get_e() const { return e; }
    
    point north() const { return point((w.x+e.x)/2, e.y<w.y? w.y : e.y); }
    point south() const { return point((w.x+e.x)/2, e.y<w.y? e.y : w.y); }
    point east() const { return point(e.x<w.x? w.x : e.x, (w.y+e.y)/2); }
    point west() const { return point(e.x<w.x? e.x : w.x, (w.y+e.y)/2); }
    point neast() const { return point(w.x<e.x? e.x : w.x, e.y<w.y? w.y : e.y); }
    point seast() const { return point(w.x<e.x? e.x : w.x, e.y<w.y? e.y : w.y); }
    point nwest() const { return point(w.x<e.x? w.x : e.x, e.y<w.y? w.y : e.y); }
    point swest() const { return point(w.x<e.x? w.x : e.x, e.y<w.y? e.y : w.y); }
    
    void move(int a, int b) {
        int nx1 = w.x + a, ny1 = w.y + b;
        int nx2 = e.x + a, ny2 = e.y + b;
        check_bounds(nx1, ny1, "move line start");
        check_bounds(nx2, ny2, "move line end");
        w.x = nx1; w.y = ny1; e.x = nx2; e.y = ny2;
    }
    
    void draw() { put_line(w, e); }
    
    void resize(double d) {
        int nx = w.x + (e.x - w.x) * d;
        int ny = w.y + (e.y - w.y) * d;
        check_bounds(nx, ny, "resize line end");
        e.x = nx; e.y = ny;
    }
};

class rectangle : public rotatable {
protected:
    point sw, ne;
public:
    rectangle(point a, point b) : sw(a), ne(b) {
        check_rect_bounds(sw.x, sw.y, ne.x, ne.y, "rectangle bounds");
    }
    
    point north() const { return point((sw.x + ne.x) / 2, ne.y); }
    point south() const { return point((sw.x + ne.x) / 2, sw.y); }
    point east() const { return point(ne.x, (sw.y + ne.y) / 2); }
    point west() const { return point(sw.x, (sw.y + ne.y) / 2); }
    point neast() const { return ne; }
    point seast() const { return point(ne.x, sw.y); }
    point nwest() const { return point(sw.x, ne.y); }
    point swest() const { return sw; }
    
    void rotate_right() {
        int w_dim = ne.x - sw.x, h_dim = ne.y - sw.y;
        int n_sw_x = ne.x - h_dim;
        int n_ne_y = sw.y + w_dim;
        check_rect_bounds(n_sw_x, sw.y, ne.x, n_ne_y, "rotate right");
        sw.x = n_sw_x;
        ne.y = n_ne_y;
        state = rotated::right;
    }
    
    void rotate_left() {
        int w_dim = ne.x - sw.x, h_dim = ne.y - sw.y;
        int n_ne_x = sw.x + h_dim;
        int n_ne_y = sw.y + w_dim;
        check_rect_bounds(sw.x, sw.y, n_ne_x, n_ne_y, "rotate left");
        ne.x = n_ne_x;
        ne.y = n_ne_y;
        state = rotated::left;
    }
    
    void move(int a, int b) {
        int nx1 = sw.x + a, ny1 = sw.y + b;
        int nx2 = ne.x + a, ny2 = ne.y + b;
        check_rect_bounds(nx1, ny1, nx2, ny2, "move rectangle");
        sw.x = nx1; sw.y = ny1; ne.x = nx2; ne.y = ny2;
    }
    
    void resize(double d) {
        int nx = sw.x + (ne.x - sw.x) * d;
        int ny = sw.y + (ne.y - sw.y) * d;
        check_rect_bounds(sw.x, sw.y, nx, ny, "resize rectangle");
        ne.x = nx; ne.y = ny;
    }
    
    void draw() {
        put_line(nwest(), ne); put_line(ne, seast());
        put_line(seast(), sw); put_line(sw, nwest());
    }
};

class h_circle : public rectangle, public reflectable {
public:
    h_circle(point a, int rd) : rectangle(point(a.x - rd, a.y), point(a.x + rd, a.y + rd)) { }
    void draw();
};

void h_circle::draw() {
    int x0 = (ne.x + sw.x) / 2, y0 = (ne.y + sw.y) / 2;
    int radius = (ne.x - sw.x) / 2;
    if (radius <= 0) return;
    int x = 0, y = radius, delta = 2 - 2 * radius, error = 0;
    while (y >= 0) {
        put_point(x0 + x, y0 + y); put_point(x0 - x, y0 + y);
        put_point(x0 + x, y0 - y); put_point(x0 - x, y0 - y);
        error = 2 * (delta + y) - 1;
        if (delta < 0 && error <= 0) { ++x; delta += 2 * x + 1; continue; }
        error = 2 * (delta - x) - 1;
        if (delta > 0 && error > 0) { --y; delta += 1 - 2 * y; continue; }
        ++x; delta += 2 * (x - y); --y;
    }
}

class olique_cross : public rectangle {
public:
    olique_cross(point a, point b) : rectangle(a, b) { }
    void draw();
};

void olique_cross::draw() {
    put_line(sw.x, sw.y, ne.x, ne.y);
    put_line(nwest().x, nwest().y, seast().x, seast().y);
}

class rhombus : public shape {
protected:
    point c;
    int h_dig;
public:
    rhombus(point a, int b) : c(a), h_dig(b) {
        check_rect_bounds(c.x - h_dig, c.y - h_dig, c.x + h_dig, c.y + h_dig, "rhombus bounds");
    }

    point north() const { return point(c.x, c.y + h_dig); }
    point south() const { return point(c.x, c.y - h_dig); }
    point east() const { return point(c.x + h_dig, c.y); }
    point west() const { return point(c.x - h_dig, c.y); }
    point neast() const { return point(c.x + h_dig, c.y + h_dig); }
    point seast() const { return point(c.x + h_dig, c.y - h_dig); }
    point nwest() const { return point(c.x - h_dig, c.y + h_dig); }
    point swest() const { return point(c.x - h_dig, c.y - h_dig); }

    void draw();
    void move(int a, int b) {
        int nx = c.x + a, ny = c.y + b;
        check_rect_bounds(nx - h_dig, ny - h_dig, nx + h_dig, ny + h_dig, "move rhombus");
        c.x = nx; c.y = ny;
    }
    void resize(double d) {
        int nd = h_dig * d;
        if (nd < 1) throw InvalidParameter("Размер слишком мал");
        check_rect_bounds(c.x - nd, c.y - nd, c.x + nd, c.y + nd, "resize rhombus");
        h_dig = nd;
    }
};

void rhombus::draw() {
    put_line(west().x, west().y, north().x, north().y);
    put_line(north().x, north().y, east().x, east().y);
    put_line(west().x, west().y, south().x, south().y);
    put_line(south().x, south().y, east().x, east().y);
}

class cross_rhombus : public rhombus {
public:
    cross_rhombus(point a, int b) : rhombus(a, b) { }
    void draw();
};

void cross_rhombus::draw() {
    rhombus::draw();
    put_line(swest().x, swest().y, neast().x, neast().y);
    put_line(nwest().x, nwest().y, seast().x, seast().y);
}

// Утилиты позиционирования
void down(shape& p, const shape& q) {
    point n = q.south(); point s = p.north();
    p.move(n.x - s.x, n.y - s.y - 1);
}
void left(shape& p, const shape& q) {
    point w = q.west(); point e = p.east();
    p.move(w.x - e.x - 1, w.y - e.y);
}
void right(shape& p, const shape& q) {
    point e = q.east(); point w = p.west();
    p.move(e.x - w.x + 1, e.y - w.y);
}
void center(shape& p, const shape& q) {
    int q_cx = (q.west().x + q.east().x) / 2;
    int q_cy = (q.south().y + q.north().y) / 2;
    int p_cx = (p.west().x + p.east().x) / 2;
    int p_cy = (p.south().y + p.north().y) / 2;
    p.move(q_cx - p_cx, q_cy - p_cy);
}
void up(shape& p, const shape& q) {
    point n = q.north(); point s = p.south();
    p.move(n.x - s.x, n.y - s.y + 1);
}