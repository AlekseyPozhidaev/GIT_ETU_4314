#include <iostream>
#include <limits>
#include "shape.h"

using namespace std;

void pause() {
    cout << "\nНажмите Enter для продолжения...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int get_int(const string& prompt) {
    int val;
    while (true) {
        cout << prompt;
        if (cin >> val) return val;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Некорректный ввод. Попробуйте снова.\n";
    }
}

void create_shape_menu() {
    cout << "\n--- Создание фигуры ---\n";
    cout << "1. Линия\n2. Прямоугольник\n3. Круг (эллипс)\n4. Ромб\nВыбор: ";
    int type;
    cin >> type;
    
    try {
        if (type == 1) {
            int x1 = get_int("X1: "), y1 = get_int("Y1: ");
            int x2 = get_int("X2: "), y2 = get_int("Y2: ");
            new line(point(x1, y1), point(x2, y2));
        } else if (type == 2) {
            int x1 = get_int("X1 (левый нижний): "), y1 = get_int("Y1: ");
            int x2 = get_int("X2 (правый верхний): "), y2 = get_int("Y2: ");
            new rectangle(point(x1, y1), point(x2, y2));
        } else if (type == 3) {
            int x = get_int("X центра: "), y = get_int("Y центра: ");
            int r = get_int("Радиус: ");
            new h_circle(point(x, y), r);
        } else if (type == 4) {
            int x = get_int("X центра: "), y = get_int("Y центра: ");
            int d = get_int("Полудиагональ: ");
            new rhombus(point(x, y), d);
        } else {
            cout << "Неизвестный тип фигуры.\n";
            return;
        }
        cout << "Фигура успешно создана!\n";
    } catch (const ShapeException& e) {
        cout << "Ошибка создания: " << e.what() << "\n";
        cout << "Фигура не была добавлена на экран.\n";
    }
}

void edit_shape_menu() {
    if (shape::shapes.empty()) {
        cout << "Нет фигур для редактирования.\n";
        return;
    }

    cout << "\n--- Список фигур ---\n";
    int i = 0;
    for (auto s : shape::shapes) {
        cout << i++ << ". Фигура (тип: " << typeid(*s).name() << ")\n";
    }
    
    int idx = get_int("Выберите индекс фигуры: ");
    if (idx < 0 || idx >= (int)shape::shapes.size()) {
        cout << "Неверный индекс.\n";
        return;
    }

    auto it = shape::shapes.begin();
    std::advance(it, idx);
    shape* s = *it;

    cout << "\nДействие:\n1. Переместить\n2. Изменить размер\n3. Повернуть (если доступно)\n4. Отразить (если доступно)\nВыбор: ";
    int action;
    cin >> action;

    try {
        if (action == 1) {
            int dx = get_int("Сдвиг X: "), dy = get_int("Сдвиг Y: ");
            s->move(dx, dy);
        } else if (action == 2) {
            double d = 1.0;
            cout << "Множитель размера (например, 1.5): ";
            cin >> d;
            s->resize(d);
        } else if (action == 3) {
            rotatable* r = dynamic_cast<rotatable*>(s);
            if (r) {
                cout << "1. Влево, 2. Вправо: ";
                int dir; cin >> dir;
                if (dir == 1) r->rotate_left();
                else r->rotate_right();
            } else {
                throw CantBeRotated("Фигура не поддерживает поворот");
            }
        } else if (action == 4) {
            reflectable* r = dynamic_cast<reflectable*>(s);
            if (r) {
                cout << "1. Горизонтально, 2. Вертикально: ";
                int dir; cin >> dir;
                if (dir == 1) r->flip_horisontally();
                else r->flip_vertically();
            } else {
                throw CantBeReflected("Фигура не поддерживает отражение");
            }
        }
        cout << "Изменение применено.\n";
    } catch (const ShapeException& e) {
        cout << "Ошибка изменения: " << e.what() << "\n";
        cout << "Фигура осталась без изменений.\n";
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    screen_init();
    
    int choice = 0;
    while (choice != 5) {
        cout << "\n=== ГРАФИЧЕСКИЙ РЕДАКТОР ===\n";
        cout << "1. Создать фигуру\n";
        cout << "2. Изменить фигуру\n";
        cout << "3. Показать экран (Refresh)\n";
        cout << "4. Очистить экран\n";
        cout << "5. Выход\n";
        cout << "Выбор: ";
        cin >> choice;

        try {
            if (choice == 1) {
                create_shape_menu();
            } else if (choice == 2) {
                edit_shape_menu();
            } else if (choice == 3) {
                shape_refresh();
                pause();
            } else if (choice == 4) {
                screen_clear();
                // Очищаем список фигур при полной очистке экрана (опционально)
                for (auto s : shape::shapes) delete s;
                shape::shapes.clear();
                cout << "Экран очищен.\n";
            }
        } catch (const std::exception& e) {
            cout << "Критическая ошибка: " << e.what() << "\n";
        }
    }

    screen_destroy();
    // Очистка памяти
    for (auto s : shape::shapes) delete s;
    return 0;
}