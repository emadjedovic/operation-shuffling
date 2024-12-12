#include <iostream>
#include <vector>
#include "Operation.h"
#include "DependencyGraph.h"
#include "TopologicalSort.h"
#include "Shuffler.h"
#include "FileParser.h"

int main() {
    // Specify the input file with operations
    std::string filename = "operations.txt";
    
    // Use FileParser to parse the operations from the file
    std::vector<Operation> operations = FileParser::parseOperations(filename);
    
    // Check if parsing was successful
    if (operations.empty()) {
        std::cerr << "Failed to parse operations from the file." << std::endl;
        return 1;
    }

    // Create DependencyGraph
    DependencyGraph depGraph;
    
    // Add operations to DependencyGraph
    for (const auto& op : operations) {
        depGraph.addOperation(op);
    }

    // Identify and add dependencies between operations (simplified)
    // You may need to implement logic to find dependencies based on variable outputs/inputs.
    depGraph.addDependencies();

    // Perform topological sort on the graph (depends on your TopologicalSort class)
    TopologicalSort topoSort;
    std::vector<Operation> sortedOperations = topoSort.sort(depGraph);

    // Shuffle the operations randomly (depends on your Shuffler class)
    Shuffler shuffler;
    std::vector<Operation> shuffledOperations = shuffler.shuffle(sortedOperations);

    // Output shuffled operations (could also be written to a file or manipulated further)
    std::cout << "Shuffled Operations:" << std::endl;
    for (const auto& op : shuffledOperations) {
        std::cout << op.getOutput() << " = ";
        for (const auto& input : op.getInputs()) {
            std::cout << input << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
