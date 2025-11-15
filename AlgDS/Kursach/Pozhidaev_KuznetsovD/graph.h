#pragma once

#include <cstddef> 
#include<iostream>
#include <set>
#include <stdexcept> 
#include <vector>

class Graph {
private:
	std::vector<std::set<int>> adjacencyList;
	std::set<std::pair<int, int>> mainAdjacency;
	std::set<std::pair<int, int>> adjacency;
	std::set<std::pair<int, int>> cycleAdjacency;
	size_t numNodes;

public:
	Graph(size_t initialCapacity = 10) : numNodes(0) {
		adjacencyList.reserve(initialCapacity);
	}
	int addNode();
	void addEdge(int u, int v);
	size_t getNumNodes() const;
	void DFSunit(int u, std::set<int>& seen);
	void DFS(int u);
	bool search(int u, int v);
	void getCycle();
	std::set<std::pair<int, int>> getCycleAdjacency();
};