#include "FileParser.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Function to parse operations from a file
std::vector<Operation> FileParser::parseOperations(const std::string& filename) {
    std::vector<Operation> operations;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return operations;  // Return empty vector if file cannot be opened
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::vector<std::string> inputs;
        std::string input, output;
        
        // Simple example: "a,b = Operation(c, d, e)"
        if (std::getline(ss, input, '=') && std::getline(ss, output)) {
            std::stringstream inputStream(input);
            std::string token;
            while (std::getline(inputStream, token, ',')) {
                inputs.push_back(token);
            }
            operations.push_back(Operation(inputs, output));
        }
    }

    return operations;
}
