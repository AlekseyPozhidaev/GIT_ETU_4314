#include "graph.h"
#include <iostream>
#include <locale>

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
	// Windows
	command = "start \"\" \"" + filePath + "\"";
#elif __APPLE__
	// macOS
	command = "open \"" + filePath + "\"";
#else
	// Linux и другие Unix-системы
	command = "xdg-open \"" + filePath + "\"";
#endif

	std::system(command.c_str());
}

int main() {
	setlocale(LC_ALL, "RU");
	Graph g;
	int choice, GraphVis = isGraphVizInstalled();
	if (GraphVis) {
		std::cout << "GraphViz установлен на компьютере." << std::endl;

#ifdef _WIN32
		std::system("dot -V");
#else
		std::system("dot -V 2>&1");
#endif
	}
	else {
		std::cout << "GraphViz НЕ установлен на компьютере.\nОн нужен для визуализации графов, но файл с графом все равно будет сохранятся в корневой папке и его можно будет открыть самостоятельно" << std::endl;
		std::cout << "Установите GraphViz с официального сайта: https://graphviz.org/download/" << std::endl;
	}
	while (true) {
		std::cout << "\n1 — ручной ввод графа\n";
		std::cout << "2 — случайное дерево\n";
		std::cout << "3 — случайный связный граф\n";
		std::cout << "4 — тестовый граф\n";
		std::cout << "0 — выход\n";
		std::cout << "Выбор: ";
		std::cin >> choice;

		if (choice == 0) break;

		if (choice == 1) g.readGraphFromInput();
		else if (choice == 2) {
			int n; std::cout << "Количество вершин: "; std::cin >> n;
			g.generateRandomTree(n);
		}
		else if (choice == 3) {
			int n, extra;
			std::cout << "Вершин: "; std::cin >> n;
			std::cout << "Дополнительных рёбер (хорд): "; std::cin >> extra;
			g.generateRandomConnectedGraph(n, extra);
		}
		else if (choice == 4) {
			g.clear();
			g.addNode(); g.addNode(); g.addNode(); g.addNode(); g.addNode();
			g.addEdge(1, 2); g.addEdge(2, 3); g.addEdge(0, 1); g.addEdge(1, 3); g.addEdge(0, 2); g.addEdge(3, 4);
		}

		g.DFS(0);
		g.exportToDot("graph.dot");
		if (GraphVis) {
			system("dot -Tpng graph.dot -o graph.png");
			openFile("graph.png");
		}
		std::cout << "\nФундаментальные циклы:\n";
		g.getCycle();
	}
	return 0;
}