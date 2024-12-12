#include "DependencyGraph.h"
#include <iostream>

// Add an operation to the graph
void DependencyGraph::addOperation(const Operation& operation) {
    operations.push_back(operation);
}

// For simplicity, we'll assume a very basic "dependency" mechanism where an operation
// depends on others that output variables matching its input.
void DependencyGraph::addDependencies() {
    // Loop through operations and set up dependencies (simplified for the example)
    for (const auto& operation : operations) {
        std::cout << "Operation: " << operation.getOutput() << " depends on ";
        for (const auto& input : operation.getInputs()) {
            std::cout << input << " ";
        }
        std::cout << std::endl;
    }
}
