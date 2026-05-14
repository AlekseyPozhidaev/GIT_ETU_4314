import glob
import csv

def analyze():
    events = []
    # Поиск всех .csv файлов логов
    files = glob.glob("*.csv")
    
    for filename in files:
        is_writer = "Writer" in filename
        pid = filename.split('_')[1].split('.')[0]
        
        with open(filename, 'r') as f:
            reader = csv.reader(f)
            start_time = None
            active_page = None
            
            for row in reader:
                if not row: continue
                ts, state, page = int(row[0]), int(row[1]), int(row[2])
                
                if state == 1: # ACTIVE
                    start_time = ts
                    active_page = page
                elif state == 2 and start_time is not None: # RELEASE
                    events.append({
                        'pid': pid,
                        'type': 'Writer' if is_writer else 'Reader',
                        'page': active_page,
                        'start': start_time,
                        'end': ts
                    })
                    start_time = None

    # Проверка коллизий с учетом погрешности
    err_count = 0
    ALLOWED_OVERLAP_MS = 5  # Допустимая погрешность в 5 мс

    for i in range(len(events)):
        for j in range(i + 1, len(events)):
            e1, e2 = events[i], events[j]
            
            if e1['page'] == e2['page']:
                # Вычисляем величину пересечения интервалов
                # Пересечение существует, если start_max < end_min
                start_max = max(e1['start'], e2['start'])
                end_min = min(e1['end'], e2['end'])
                
                overlap = end_min - start_max
                
                # Если пересечение больше допустимой погрешности
                if overlap > ALLOWED_OVERLAP_MS:
                    if e1['type'] == 'Writer' or e2['type'] == 'Writer':
                        print(f"COLLISION on page {e1['page']}: {e1['type']}({e1['pid']}) and {e2['type']}({e2['pid']})")
                        print(f"  T1: {e1['start']}-{e1['end']}, T2: {e2['start']}-{e2['end']} | Overlap: {overlap} ms")
                        err_count += 1
    
    if err_count == 0:
        print("Success: No synchronization errors found (with 5ms tolerance).")
    else:
        print(f"Total errors: {err_count}")

if __name__ == "__main__":
    analyze()