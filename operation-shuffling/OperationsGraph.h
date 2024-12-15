#pragma once

#ifndef OPERATIONSGRAPH_H
#define OPERATIONSGRAPH_H

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <random>
#include <stdexcept>

using namespace std;

struct Operation {
	int label;
	vector<string> inputs;
	vector<string> outputs;

	Operation(int l, vector<string> in, vector<string> out);
};

class OperationsGraph {


	vector<Operation> operations;
	unordered_map<int, int> inDegree;  // Map to handle arbitrary labels
	unordered_map<int, vector<int>> adjList;  // Adjacency list to handle arbitrary labels
	unordered_set<string> outputsSet;
	unordered_map<string, int> lastProducer;

	void sortZeroInDegreeOps(vector<int>& ops, unordered_set<int>& sortedSet);

public:
	void addOperation(const Operation& op);
	vector<int> topologicalSort();

};

#endif

