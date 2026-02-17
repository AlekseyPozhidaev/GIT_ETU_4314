#include "graph.h"
#include <iostream>
#include <locale>
#include <iomanip>

bool isGraphVizInstalled() {
#ifdef _WIN32
    int result = std::system("dot -V > nul 2>&1");
#else
    int result = std::system("dot -V > /dev/null 2>&1");
#endif
    return (result == 0);
}

void openFile(const std::string& filePath) {
    std::string command;
#ifdef _WIN32
    command = "start \"\" \"" + filePath + "\"";
#elif __APPLE__
    command = "open \"" + filePath + "\"";
#else
    command = "xdg-open \"" + filePath + "\"";
#endif
    std::system(command.c_str());
}

void printHeader() {
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << "          АНАЛИЗ ГРАФОВ\n";
    std::cout << std::string(50, '=') << "\n";
}

void printMenu() {
    std::cout << "\n" << std::string(40, '-') << "\n";
    std::cout << "           МЕНЮ\n";
    std::cout << std::string(40, '-') << "\n";
    
    std::cout << std::left;
    std::cout << "  1. Ввести граф вручную\n";
    std::cout << "  2. Сгенерировать случайное дерево\n";
    std::cout << "  3. Сгенерировать связный граф\n";
    std::cout << "  4. Использовать тестовый граф\n";
    std::cout << "  0. Выход\n";
    std::cout << std::string(40, '-') << "\n";
}

void printSection(const std::string& title) {
    std::cout << "\n" << std::string(40, '-') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(40, '-') << "\n";
}

int main() {
    setlocale(LC_ALL, "RU");
    std::cout << std::boolalpha;
    
    Graph g;
    int choice;
    
    // Проверка GraphViz
    printHeader();
    bool graphVisInstalled = isGraphVizInstalled();
    
    if (graphVisInstalled) {
        printSection("ПРОВЕРКА GraphViz");
        std::cout << "  ✓ GraphViz установлен\n";
        
#ifdef _WIN32
        std::system("dot -V");
#else
        std::system("dot -V 2>&1");
#endif
    } else {
        printSection("ПРЕДУПРЕЖДЕНИЕ");
        std::cout << "  ✗ GraphViz не установлен на компьютере.\n";
        std::cout << "  Вы можете использовать все функции, но для визуализации\n";
        std::cout << "  графов необходимо установить GraphViz.\n\n";
        std::cout << "  Скачать GraphViz можно по ссылке:\n";
        std::cout << "  https://graphviz.org/download/\n";
    }
    
    // Главный цикл меню
    while (true) {
        printMenu();
        std::cout << "\n  Ваш выбор: ";
        std::cin >> choice;
        
        if (choice == 0) {
            printSection("ВЫХОД");
            std::cout << "  Программа завершена. До свидания!\n";
            break;
        }
        
        // Обработка выбора пользователя
        switch (choice) {
            case 1: {
                printSection("РУЧНОЙ ВВОД ГРАФА");
                std::cout << "  Введите данные графа:\n";
                g.readGraphFromInput();
                break;
            }
            case 2: {
                printSection("ГЕНЕРАЦИЯ СЛУЧАЙНОГО ДЕРЕВА");
                int n;
                std::cout << "  Количество вершин: ";
                std::cin >> n;
                g.generateRandomTree(n);
                std::cout << "  ✓ Дерево сгенерировано (" << n << " вершин)\n";
                break;
            }
            case 3: {
                printSection("ГЕНЕРАЦИЯ СВЯЗНОГО ГРАФА");
                int n, extra;
                std::cout << "  Вершин: ";
                std::cin >> n;
                std::cout << "  Дополнительных рёбер (мин): ";
                std::cin >> extra;
                g.generateRandomConnectedGraph(n, extra);
                std::cout << "  ✓ Граф сгенерирован (" << n << " вершин, +" << extra << " рёбер)\n";
                break;
            }
            case 4: {
                printSection("ТЕСТОВЫЙ ГРАФ");
                g.clear();
                g.addNode(); g.addNode(); g.addNode(); g.addNode(); g.addNode();
                g.addEdge(1, 2); g.addEdge(2, 3); g.addEdge(0, 1);
                g.addEdge(1, 3); g.addEdge(0, 2); g.addEdge(3, 4);
                std::cout << "  ✓ Тестовый граф создан (5 вершин, 6 рёбер)\n";
                break;
            }
            default: {
                printSection("ОШИБКА");
                std::cout << "  Неверный выбор. Пожалуйста, выберите пункт от 0 до 4.\n";
                continue;
            }
        }
        
        // Анализ графа
        printSection("АНАЛИЗ ГРАФА");
        
        std::cout << "  Выполнение обхода в глубину (DFS) от вершины 0:\n  ";
        g.DFS(0);
        
        g.exportToDot("graph.dot");
        std::cout << "  ✓ Граф экспортирован в файл 'graph.dot'\n";

        
        // if (graphVisInstalled) {
        //     system("dot -Tpng graph.dot -o graph.png");
        //    std::cout << "  ✓ PNG изображение создано: 'graph.png'\n";
            
        //     std::cout << "  Открываю изображение графа...\n";
        //     openFile("graph.png");
        // } else {
        //     std::cout << "  ⚠  GraphViz не установлен. Визуализация недоступна.\n";
        // }
        
        printSection("ПОИСК ЦИКЛОВ");
        std::cout << "  Результат поиска циклов:\n";
        g.getCycle();
    }
    
    return 0;
}