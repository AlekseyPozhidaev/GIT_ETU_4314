import pandas as pd
import matplotlib.pyplot as plt
import glob
import os

# Конфигурация
OUTPUT_DIR = "process_states_plots"
STATE_MAP = {0: 'WAIT', 1: 'ACTIVE', 2: 'RELEASE'}

if not os.path.exists(OUTPUT_DIR):
    os.makedirs(OUTPUT_DIR)

def plot_process_states():
    # Поиск всех файлов логов
    log_files = glob.glob("Reader_*.csv") + glob.glob("Writer_*.csv")
    
    if not log_files:
        print("Лог-файлы не найдены.")
        return

    for file_path in log_files:
        # Загрузка данных (Timestamp, StateID, PageID)
        df = pd.read_csv(file_path, names=['timestamp', 'state', 'page'])
        
        # Расчет относительного времени (от 0)
        start_time = df['timestamp'].min()
        df['rel_time'] = (df['timestamp'] - start_time) / 1000.0  # в секунды
        
        process_name = os.path.splitext(file_path)[0]
        
        plt.figure(figsize=(12, 4))
        # Используем step для визуализации смены состояний
        plt.step(df['rel_time'], df['state'], where='post', color='teal', linewidth=2)
        
        plt.yticks([0, 1, 2], ['WAIT', 'ACTIVE', 'RELEASE'])
        plt.xlabel('Время (сек)')
        plt.ylabel('Состояние')
        plt.title(f'Смена состояний: {process_name}')
        plt.grid(True, axis='x', linestyle='--', alpha=0.7)
        
        # Автоматическое сохранение
        save_path = os.path.join(OUTPUT_DIR, f"{process_name}_state.png")
        plt.savefig(save_path)
        print(f"График сохранен: {save_path}")
        
        # Интерактивный режим
        plt.show()

if __name__ == "__main__":
    plot_process_states()