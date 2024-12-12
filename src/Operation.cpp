#include "Operation.h"

// Constructor to initialize the operation with input and output variables
Operation::Operation(const std::vector<std::string>& inputs, const std::string& output)
    : inputs(inputs), output(output) {}

// Getter for input variables
const std::vector<std::string>& Operation::getInputs() const {
    return inputs;
}

// Getter for output variable
const std::string& Operation::getOutput() const {
    return output;
}
