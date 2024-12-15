#include "OperationsGraph.h"

int main() {
	OperationsGraph graph;

	graph.addOperation(Operation(0, { "c", "d", "e" }, { "a", "b" }));
	graph.addOperation(Operation(1, { "b", "h" }, { "d", "f" }));
	graph.addOperation(Operation(2, { "d", "k" }, { "s" }));
	graph.addOperation(Operation(3, { "s", "t" }, { "d" }));
	graph.addOperation(Operation(4, { "u", "v" }, { "y", "g" }));
	graph.addOperation(Operation(5, { "d", "g" }, { "b" }));

	try {
		vector<int> sortedOps = graph.topologicalSort();
		cout << "Topologically sorted operations with dependencies respected: ";
		for (int op : sortedOps) {
			cout << op << " ";
		}
		cout << endl;
	}
	catch (const runtime_error& e) {
		cout << e.what() << endl;
	}

	return 0;
}
