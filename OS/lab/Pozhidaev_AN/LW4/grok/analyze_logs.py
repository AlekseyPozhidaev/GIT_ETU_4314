import pandas as pd
import matplotlib.pyplot as plt
import glob

# =========================
# 1. Загрузка логов
# =========================

def load_logs():
    files = glob.glob("*.csv")
    dfs = []

    for f in files:
        df = pd.read_csv(f)
        df["SOURCE"] = f
        dfs.append(df)

    return pd.concat(dfs, ignore_index=True)


df = load_logs()

# сортируем по времени
df = df.sort_values("TIME")

# =========================
# 2. Разделение ролей
# =========================

readers = df[df["ROLE"] == "READER"]
writers = df[df["ROLE"] == "WRITER"]

# =========================
# 3. Смена состояний процессов
# =========================

def plot_process(pid_df, title):
    plt.figure()

    states = {
        "WAITING": 0,
        "READING": 1,
        "WRITING": 1,
        "RELEASING": 2
    }

    y = pid_df["STATE"].map(states)

    plt.plot(pid_df["TIME"], y, marker="o")

    plt.yticks([0, 1, 2], ["WAITING", "ACTIVE", "RELEASING"])

    plt.title(title)
    plt.xlabel("Time (ms)")
    plt.ylabel("State")

    plt.grid()

# берём по 1 процессу каждого типа
for pid in readers["PID"].unique()[:3]:
    plot_process(readers[readers["PID"] == pid],
                 f"Reader {pid}")

for pid in writers["PID"].unique()[:3]:
    plot_process(writers[writers["PID"] == pid],
                 f"Writer {pid}")

plt.show()

# =========================
# 4. Heatmap занятости страниц
# =========================

import numpy as np

time_min = df["TIME"].min()
time_max = df["TIME"].max()

time_bins = np.linspace(time_min, time_max, 50)

heatmap = np.zeros((16, len(time_bins)))

# заполняем heatmap
for _, row in df.iterrows():
    page = int(row["PAGE"])
    t = row["TIME"]

    bin_idx = np.searchsorted(time_bins, t) - 1

    if 0 <= bin_idx < len(time_bins):
        heatmap[page][bin_idx] += 1

plt.figure(figsize=(12, 6))
plt.imshow(heatmap, aspect="auto")

plt.colorbar(label="Access count")

plt.yticks(range(16), range(16))

plt.title("Page usage heatmap")

plt.xlabel("Time")
plt.ylabel("Page")

plt.show()

# =========================
# 5. Анализ starvation
# =========================

writer_wait = df[df["STATE"] == "WAITING"]

writer_stats = writer_wait.groupby("PID").size()

print("\nWriter waiting frequency:")
print(writer_stats.sort_values(ascending=False))

reader_stats = readers[readers["STATE"] == "WAITING"].groupby("PID").size()

print("\nReader waiting frequency:")
print(reader_stats.sort_values(ascending=False))