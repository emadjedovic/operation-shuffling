#include "TopologicalSort.h"
#include <queue>
#include <unordered_map>

// Function to perform topological sorting on a DependencyGraph
std::vector<Operation> TopologicalSort::sort(const DependencyGraph& depGraph) {
    std::vector<Operation> sortedOperations;
    std::unordered_map<std::string, int> inDegree;

    // Initialize in-degree for all operations
    for (const auto& op : depGraph.getOperations()) {
        inDegree[op.getOutput()] = 0;  // Set initial in-degree to 0
    }

    // Calculate in-degrees for all operations based on dependencies
    for (const auto& op : depGraph.getOperations()) {
        for (const auto& input : op.getInputs()) {
            inDegree[input]++;
        }
    }

    // Queue for operations with no incoming dependencies (in-degree 0)
    std::queue<Operation> zeroInDegreeQueue;

    for (const auto& op : depGraph.getOperations()) {
        if (inDegree[op.getOutput()] == 0) {
            zeroInDegreeQueue.push(op);
        }
    }

    // Process the operations with zero in-degree
    while (!zeroInDegreeQueue.empty()) {
        Operation currentOp = zeroInDegreeQueue.front();
        zeroInDegreeQueue.pop();
        sortedOperations.push_back(currentOp);

        // Decrease the in-degree of dependent operations
        for (const auto& input : currentOp.getInputs()) {
            inDegree[input]--;
            if (inDegree[input] == 0) {
                zeroInDegreeQueue.push(depGraph.getOperation(input));
            }
        }
    }

    // Check if a cycle exists
    if (sortedOperations.size() != depGraph.getOperations().size()) {
        throw std::runtime_error("Cycle detected in the dependency graph!");
    }

    return sortedOperations;
}
