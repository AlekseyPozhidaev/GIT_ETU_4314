#include"graph.h"

int Graph::addNode() {
	adjacencyList.emplace_back();
	return numNodes++;
}

void Graph::addEdge(int u, int v) {
	if (u < 0 || u >= static_cast<int>(numNodes) || v < 0 || v >= static_cast<int>(numNodes)) {
		throw std::out_of_range("Узел не существует");
	}
	for (auto i : adjacencyList[u]) {
		if (i == v) return;
	}
	adjacencyList[u].insert(v);
	adjacencyList[v].insert(u);
}

size_t Graph::getNumNodes() const {
	return numNodes;
}

void Graph::DFSunit(int u, std::set<int>& seen) {
	seen.insert(u);
	for (auto i : this->adjacencyList[u]) {
		if (seen.find(i) == seen.end()) {
			this->DFSunit(i, seen);
			this->mainAdjacency.insert({ u, i });
		}
		if (this->adjacency.find({ i,u }) == this->adjacency.end())this->adjacency.insert({ u, i });
	}
}

void Graph::DFS(int u) {
	std::set<int> seen;
	this->DFSunit(u, seen);
	for (auto i : this->adjacency) {
		if (this->mainAdjacency.find({ i.second,i.first }) == this->mainAdjacency.end())this->cycleAdjacency.insert(i);
	}
}

bool Graph::search(int u, int v)
{
	for (auto i : this->mainAdjacency) {
		if (i.first == u) {
			if (i.second == v) {
				std::cout << "-" << i.first;
				return true;
			}
			if (this->search(i.second, v)) {
				std::cout << "-" << i.first;
				return true;
			}
		}
	}
	return false;
}

void Graph::getCycle() {
	for (auto i : this->cycleAdjacency) {
		std::cout << i.second << "-" << i.first;
		search(i.second, i.first);
		std::cout << std::endl;
	}
}


std::set<std::pair<int, int>> Graph::getCycleAdjacency()
{
	return this->cycleAdjacency;
}


