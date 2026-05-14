import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import glob
import numpy as np

PAGE_COUNT = 16  # Из common.h

def load_all_logs():
    all_data = []
    for f in glob.glob("Reader_*.csv"):
        df = pd.read_csv(f, names=['time', 'state', 'page'])
        df['type'] = 'Reader'
        df['pid'] = f
        all_data.append(df)
    for f in glob.glob("Writer_*.csv"):
        df = pd.read_csv(f, names=['time', 'state', 'page'])
        df['type'] = 'Writer'
        df['pid'] = f
        all_data.append(df)
    return pd.concat(all_data) if all_data else pd.DataFrame()

def draw_cumulative_heatmap(df):
    # Фильтруем только переход в состояние ACTIVE (1), чтобы считать именно обращения
    active_events = df[df['state'] == 1]
    
    stats = pd.DataFrame(index=range(PAGE_COUNT))
    stats['Total'] = active_events['page'].value_counts()
    stats['Readers'] = active_events[active_events['type'] == 'Reader']['page'].value_counts()
    stats['Writers'] = active_events[active_events['type'] == 'Writer']['page'].value_counts()
    stats = stats.fillna(0).astype(int).T

    plt.figure(figsize=(12, 4))
    sns.heatmap(stats, annot=True, fmt="d", cmap="YlGnBu")
    plt.title("Общее количество обращений к страницам")
    plt.xlabel("Номер страницы")
    plt.ylabel("Тип")
    plt.show()

def draw_occupancy_over_time(df):
    # Определяем границы времени всего эксперимента
    t_min = df['time'].min()
    t_max = df['time'].max()
    
    # Шаг дискретизации (100 мс)
    step = 100 
    time_bins = np.arange(t_min, t_max + step, step)
    
    # Сетка: Страницы x Время
    grid = np.zeros((PAGE_COUNT, len(time_bins)))

    # Для каждого процесса находим интервалы активности (State 1 -> State 2)
    for pid in df['pid'].unique():
        proc_df = df[df['pid'] == pid].sort_values('time')
        for i in range(len(proc_df) - 1):
            if proc_df.iloc[i]['state'] == 1: # Начало ACTIVE
                t_start = proc_df.iloc[i]['time']
                t_end = proc_df.iloc[i+1]['time'] # Конец ACTIVE (переход в RELEASE)
                page = int(proc_df.iloc[i]['page'])
                
                # Заполняем сетку
                mask = (time_bins >= t_start) & (time_bins <= t_end)
                grid[page, mask] += 1

    plt.figure(figsize=(15, 6))
    sns.heatmap(grid, cmap="Reds", cbar_kws={'label': 'Число активных процессов'})
    plt.title("Занятость страниц во времени (Occupancy)")
    plt.xlabel(f"Временные интервалы (шаг {step}мс)")
    plt.ylabel("Номер страницы")
    plt.show()

if __name__ == "__main__":
    data = load_all_logs()
    if not data.empty:
        draw_cumulative_heatmap(data)
        draw_occupancy_over_time(data)
    else:
        print("Данные не загружены.")