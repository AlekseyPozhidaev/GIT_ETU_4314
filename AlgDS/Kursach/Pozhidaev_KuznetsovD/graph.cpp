#include "graph.h"
#include <algorithm> 
#include <fstream>
#include <random>     

Graph::Graph(size_t initialCapacity) : numNodes(0), timer(0) {
	adjacencyList.reserve(initialCapacity);
}

void Graph::clear() {
	adjacencyList.clear();
	cycleAdjacency.clear();
	numNodes = 0;
	parent.clear();
	visited.clear();
	disc.clear();
}

int Graph::addNode() {
	adjacencyList.emplace_back();
	return numNodes++;
}

void Graph::addEdge(int u, int v) {
	if (u < 0 || u >= static_cast<int>(numNodes) || v < 0 || v >= static_cast<int>(numNodes)) {
		throw std::out_of_range("Узел не существует");
	}
	adjacencyList[u].insert(v);
	adjacencyList[v].insert(u);
}

size_t Graph::getNumNodes() const {
	return numNodes;
}

void Graph::generateRandomTree(size_t n) {
	clear();
	if (n == 0) return;

	for (size_t i = 0; i < n; ++i) addNode();

	std::mt19937 rng(std::random_device{}());
	for (size_t i = 1; i < n; ++i) {
		std::uniform_int_distribution<int> dist(0, static_cast<int>(i) - 1);
		int parent = dist(rng);
		addEdge(static_cast<int>(i), parent);
	}
}

void Graph::generateRandomConnectedGraph(size_t n, int additionalEdges) {
	if (n == 0) return;
	generateRandomTree(n);

	if (additionalEdges <= 0) return;

	std::mt19937 rng(std::random_device{}());
	std::uniform_int_distribution<int> dist(0, static_cast<int>(n) - 1);

	int added = 0;
	while (added < additionalEdges) {
		int u = dist(rng);
		int v = dist(rng);
		if (u == v) continue;

		if (adjacencyList[u].find(v) == adjacencyList[u].end()) {
			addEdge(u, v);
			++added;
		}
	}
}

void Graph::readGraphFromInput() {
	clear();
	size_t n, m;
	std::cout << "Введите количество вершин и рёбер (n m): ";
	std::cin >> n >> m;

	for (size_t i = 0; i < n; ++i) addNode();

	std::cout << "Введите " << m << " рёбер (u v):\n";
	for (size_t i = 0; i < m; ++i) {
		int u, v;
		std::cin >> u >> v;
		addEdge(u, v);
	}
}

void Graph::dfs_util(int u, int par) {
	visited[u] = true;
	parent[u] = par;
	disc[u] = timer++;

	for (int v : adjacencyList[u]) {
		if (v == par) continue;

		if (visited[v]) {
			if (disc[u] > disc[v]) {
				cycleAdjacency.insert({ v, u });
			}
		}
		else {
			dfs_util(v, u);
		}
	}
}

void Graph::DFS(int start) {
	timer = 0;
	visited.assign(numNodes, false);
	parent.assign(numNodes, -1);
	disc.assign(numNodes, -1);
	cycleAdjacency.clear();

	if (start >= 0 && start < static_cast<int>(numNodes) && !visited[start]) {
		dfs_util(start, -1);
	}

	// for (size_t i = 0; i < numNodes; ++i)
	//     if (!visited[i]) dfs_util(i, -1);
}

void Graph::getCycle() {
	if (cycleAdjacency.empty()) {
		std::cout << "Циклов нет (граф — лес)\n";
		return;
	}

	for (const auto& p : cycleAdjacency) {
		int anc = p.first;
		int desc = p.second;

		std::vector<int> path;
		for (int curr = desc; curr != anc; curr = parent[curr]) {
			path.push_back(curr);
		}
		path.push_back(anc);
		std::reverse(path.begin(), path.end());

		for (size_t i = 0; i < path.size(); ++i) {
			if (i > 0) std::cout << "-";
			std::cout << path[i];
		}
		std::cout << "-" << anc << std::endl;
	}
}

void Graph::exportToDot(const std::string& filename) const {
	std::ostream* out = &std::cout;
	std::ofstream file;
	if (!filename.empty()) {
		file.open(filename);
		if (!file.is_open()) {
			std::cerr << "Не удалось открыть файл " << filename << std::endl;
			return;
		}
		out = &file;
	}

	*out << "strict graph G {\n";
	*out << "    overlap=false;\n";
	*out << "    splines=true;\n";
	*out << "    node [shape=circle, style=filled, fillcolor=lightgrey, fontsize=14];\n";
	*out << "    edge [fontsize=12];\n\n";

	*out << "    // Tree edges (остовное дерево)\n";
	for (size_t i = 0; i < numNodes; ++i) {
		if (parent[i] != -1) {
			*out << "    " << parent[i] << " -- " << i
				<< " [color=darkgreen, penwidth=2.5, label=\"tree\"];\n";
		}
	}


	*out << "\n    // Back-edges (образуют фундаментальные циклы)\n";
	for (const auto& p : cycleAdjacency) {
		*out << "    " << p.first << " -- " << p.second
			<< " [color=red, penwidth=3, style=dashed, label=\"back\"];\n";
	}

	*out << "\n    // Обычные рёбра\n";
	for (size_t u = 0; u < numNodes; ++u) {
		for (int v : adjacencyList[u]) {
			if (v > static_cast<int>(u)) {
				bool is_tree = (parent[v] == static_cast<int>(u) || parent[u] == v);
				bool is_back = (cycleAdjacency.count({ u, v }) || cycleAdjacency.count({ v, u }));
				if (!is_tree && !is_back) {
					*out << "    " << u << " -- " << v << " [color=gray, penwidth=1];\n";
				}
			}
		}
	}

	*out << "}\n";

	if (&file) {
		file.close();
		std::cout << "Граф сохранён в " << filename << "\n";
	}
}