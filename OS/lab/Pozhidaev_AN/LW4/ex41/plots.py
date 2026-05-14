import glob
import csv
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

def load_data():
    events = []
    files = glob.glob("*.csv")
    
    if not files:
        print("Error: No CSV log files found!")
        return []

    for filename in files:
        is_writer = "Writer" in filename
        pid = filename.split('_')[1].split('.')[0]
        
        with open(filename, 'r') as f:
            reader = csv.reader(f)
            # Временные буферы для отслеживания фаз
            wait_start = None
            active_start = None
            active_page = None
            
            for row in reader:
                if not row: continue
                ts, state, page = int(row[0]), int(row[1]), int(row[2])
                
                if state == 0:     # WAIT START
                    wait_start = ts
                elif state == 1:   # ACTIVE START (Reading/Writing)
                    if wait_start is not None:
                        events.append({
                            'pid': pid, 'type': 'Writer' if is_writer else 'Reader',
                            'page': page, 'state': 'WAIT', 'start': wait_start, 'end': ts
                        })
                        wait_start = None
                    active_start = ts
                    active_page = page
                elif state == 2:   # RELEASE
                    if active_start is not None:
                        events.append({
                            'pid': pid, 'type': 'Writer' if is_writer else 'Reader',
                            'page': active_page, 'state': 'ACTIVE', 'start': active_start, 'end': ts
                        })
                        active_start = None
    return events

def plot_gantt(events):
    # Отбираем уникальные PID для 5 Читателей и 5 Писателей
    readers = list(set([e['pid'] for e in events if e['type'] == 'Reader']))[:5]
    writers = list(set([e['pid'] for e in events if e['type'] == 'Writer']))[:5]
    selected_pids = readers + writers
    
    if not selected_pids: return
    
    # Находим общую точку отсчета времени (t=0)
    min_time = min([e['start'] for e in events])
    
    plt.figure(figsize=(12, 6))
    
    # Цвета состояний
    colors = {'WAIT': '#ff9999', 'ACTIVE': '#66b3ff'}
    
    for idx, pid in enumerate(selected_pids):
        proc_type = 'Reader' if pid in readers else 'Writer'
        proc_events = [e for e in events if e['pid'] == pid]
        
        for e in proc_events:
            start_rel = (e['start'] - min_time) / 1000.0  # переводим в секунды
            duration = (e['end'] - e['start']) / 1000.0
            
            # Отрисовка интервала
            plt.barh(idx, duration, left=start_rel, 
                     color=colors[e['state']], edgecolor='black', alpha=0.8)
            
            # Подписываем номер страницы поверх активной фазы
            if e['state'] == 'ACTIVE' and duration > 0.2:
                plt.text(start_rel + duration/2, idx, f"p{e['page']}", 
                         va='center', ha='center', fontsize=8, color='black', weight='bold')

    plt.yticks(range(len(selected_pids)), [f"{'R' if pid in readers else 'W'}_{pid}" for pid in selected_pids])
    plt.xlabel("Time (seconds)")
    plt.ylabel("Processes (PID)")
    plt.title("Gantt Chart: Process State Transitions (5 Readers & 5 Writers)")
    
    # Кастомная легенда
    from matplotlib.patches import Patch
    legend_elements = [Patch(facecolor='#ff9999', label='WAIT (Queue)'),
                       Patch(facecolor='#66b3ff', label='ACTIVE (Execution)')]
    plt.legend(handles=legend_elements, loc='upper right')
    plt.grid(axis='x', linestyle='--', alpha=0.5)
    plt.tight_layout()
    plt.savefig("gantt_chart.png", dpi=300)
    plt.show()

def plot_heatmap(events):
    min_time = min([e['start'] for e in events])
    max_time = max([e['end'] for e in events])
    
    # Разбиваем весь эксперимент на временные кванты по 500 мс (0.5 сек)
    time_bin_ms = 500
    total_bins = int((max_time - min_time) / time_bin_ms) + 1
    
    # Матрица: 16 страниц на N временных интервалов
    heatmap_data = np.zeros((16, total_bins))
    
    # Заполняем только фазы ACTIVE
    for e in events:
        if e['state'] == 'ACTIVE':
            start_bin = int((e['start'] - min_time) / time_bin_ms)
            end_bin = int((e['end'] - min_time) / time_bin_ms)
            
            # Инкрементируем занятость страницы в данных квантах времени
            for b in range(start_bin, end_bin + 1):
                if 0 <= b < total_bins:
                    if e['type'] == 'Writer':
                        heatmap_data[e['page']][b] += 5  # Вес писателя больше, чтобы выделить на карте
                    else:
                        heatmap_data[e['page']][b] += 1  # Вес читателя
                        
    plt.figure(figsize=(14, 7))
    # Создаем временные метки для оси X в секундах
    x_labels = [f"{i * time_bin_ms / 1000.0:.1f}" for i in range(total_bins)]
    
    # Ограничим количество подписей на оси X, чтобы они не сливались
    stride = max(1, total_bins // 20)
    
    sns.heatmap(heatmap_data, cmap="YlOrRd", linewidths=0.1, linecolor='gray',
                yticklabels=list(range(16)), xticklabels=x_labels)
    
    plt.xticks(range(0, total_bins, stride), x_labels[::stride], rotation=45)
    plt.xlabel("Timeline (seconds)")
    plt.ylabel("Buffer Page Number (0-15)")
    plt.title("Heatmap: Buffer Page Occupancy Over Time")
    plt.tight_layout()
    plt.savefig("page_heatmap.png", dpi=300)
    plt.show()

if __name__ == "__main__":
    all_events = load_data()
    if all_events:
        print(f"Loaded {len(all_events)} execution phases. Generating charts...")
        plot_gantt(all_events)
        plot_heatmap(all_events)
        print("Success! 'gantt_chart.png' and 'page_heatmap.png' saved.")