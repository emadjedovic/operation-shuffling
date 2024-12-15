#include "OperationsGraph.h"

Operation::Operation(int l, vector<string> in, vector<string> out)
	: label(l), inputs(in), outputs(out) {
}

void OperationsGraph::addOperation(const Operation& op) {
	operations.push_back(op);
	inDegree[op.label] = 0;
	adjList[op.label] = vector<int>();

	cout << "Adding operation " << op.label << " with inputs: ";
	for (const auto& input : op.inputs) cout << input << " ";
	cout << "and outputs: ";
	for (const auto& output : op.outputs) cout << output << " ";
	cout << endl;

	// Check dependencies for inputs
	for (const auto& input : op.inputs) {
		if (lastProducer.find(input) != lastProducer.end()) {
			int producer = lastProducer[input];
			cout << "Operation " << producer << " produces input " << input << " for operation " << op.label << endl;

			// Add dependency
			adjList[producer].push_back(op.label);
			inDegree[op.label]++;
		}
	}

	// Update lastProducer for outputs
	for (const auto& output : op.outputs) {
		lastProducer[output] = op.label;
	}

	// Print debugging info
	cout << "Current in-degree vector: ";
	for (const auto& entry : inDegree) {
		cout << entry.second << " ";
	}
	cout << endl;

	cout << "Current adjList vector: " << endl;
	for (const auto& entry : adjList) {
		cout << entry.first << " -> ";
		for (int neighbor : entry.second) {
			cout << neighbor << " ";
		}
		cout << endl;
	}
}

vector<int> OperationsGraph::topologicalSort() {
	vector<int> sortedOps;
	queue<int> q;

	// Start with operations that have zero in-degree (no dependencies)
	for (const auto& op : operations) {
		if (inDegree[op.label] == 0) {
			q.push(op.label);
			cout << "Adding operation " << op.label << " to processing queue (zero in-degree)" << endl;
		}
	}

	unordered_set<int> sortedSet;  // To track sorted operations

	while (!q.empty()) {
		vector<int> currentZeroInDegreeOps;

		// Gather all current zero in-degree operations
		while (!q.empty()) {
			currentZeroInDegreeOps.push_back(q.front());
			q.pop();
		}

		// Debug: Show operations with zero in-degree before processing
		cout << "Sorting operations with zero in-degree: ";
		for (int op : currentZeroInDegreeOps) {
			cout << op << " ";
		}
		cout << endl;

		// Sort or process these in the correct order respecting dependencies
		sortZeroInDegreeOps(currentZeroInDegreeOps, sortedSet);

		// Process each operation with zero in-degree
		for (const auto& current : currentZeroInDegreeOps) {
			sortedOps.push_back(current);
			sortedSet.insert(current);
			cout << "Processing operation " << current << endl;

			// Update in-degree of all dependent operations
			for (const auto& neighbor : adjList[current]) {
				inDegree[neighbor]--;
				cout << "Decreased in-degree of operation " << neighbor << " to " << inDegree[neighbor] << endl;
				if (inDegree[neighbor] == 0) {
					q.push(neighbor);
					cout << "Adding operation " << neighbor << " to processing queue (zero in-degree)" << endl;
				}
			}
		}
	}

	if (sortedOps.size() != operations.size()) {
		throw runtime_error("Circular dependency detected.");
	}

	return sortedOps;
}

void OperationsGraph::sortZeroInDegreeOps(vector<int>& ops, unordered_set<int>& sortedSet) {
	random_device rd;
	mt19937 g(rd());
	shuffle(ops.begin(), ops.end(), g);
}
