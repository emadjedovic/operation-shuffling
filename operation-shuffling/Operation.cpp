#include "Operation.h"
#include <iostream>

Operation::Operation(const vector<string>& inputs, const string& output)
    : inputs(inputs), output(output) {}

const vector<string>& Operation::getInputs() const {
    return inputs;
}

const string& Operation::getOutput() const {
    return output;
}

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

bool Operation::operator==(const Operation& other) const {
    return (inputs == other.inputs && output == other.output);
}
