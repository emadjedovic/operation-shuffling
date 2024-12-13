#include "Operation.h"
#include <iostream>

// Constructor to initialize the operation with input and output variables
Operation::Operation(const vector<string>& inputs, const string& output)
    : inputs(inputs), output(output) {}

// Getter for input variables
const vector<string>& Operation::getInputs() const {
    return inputs;
}

// Getter for output variable
const string& Operation::getOutput() const {
    return output;
}

// Method to print the operation (for easy display)
void Operation::printOperation() const {
    cout << output << " = ";
    for (size_t i = 0; i < inputs.size(); ++i) {
        cout << inputs[i];
        if (i < inputs.size() - 1) {
            cout << ", ";
        }
    }
    cout << endl;
}

// Method to check equality of operations (optional)
bool Operation::operator==(const Operation& other) const {
    return (inputs == other.inputs && output == other.output);
}
