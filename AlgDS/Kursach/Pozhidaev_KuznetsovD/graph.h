#pragma once

#include <cstddef>
#include <iostream>
#include <set>
#include <stdexcept>
#include <vector>

class Graph {
private:
	std::vector<std::set<int>> adjacencyList;
	std::set<std::pair<int, int>> cycleAdjacency;

	std::vector<int> parent;
	std::vector<bool> visited;
	std::vector<int> disc;
	int timer;
	size_t numNodes;

	void dfs_util(int u, int par);

public:
	Graph(size_t initialCapacity = 10);
	~Graph() = default;

	void clear();
	int addNode();
	void addEdge(int u, int v);
	size_t getNumNodes() const;

	void generateRandomTree(size_t n);
	void generateRandomConnectedGraph(size_t n, int additionalEdges = 5);
	void readGraphFromInput();

	void exportToDot(const std::string& filename = "graph.dot") const;
	void DFS(int start = 0);
	void getCycle();
};