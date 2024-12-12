#ifndef OPERATION_H
#define OPERATION_H

#include <vector>
#include <string>

class Operation {
public:
    // Constructor to initialize the operation with input and output variables
    Operation(const std::vector<std::string>& inputs, const std::string& output);

    // Getter for input variables
    const std::vector<std::string>& getInputs() const;

    // Getter for output variable
    const std::string& getOutput() const;

private:
    std::vector<std::string> inputs;  // Input variables
    std::string output;               // Output variable
};

#endif // OPERATION_H
