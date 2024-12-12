#ifndef OPERATION_H_INCLUDED
#define OPERATION_H_INCLUDED

#include <vector>
#include <string>
#include <iostream> // For output stream

using namespace std;

class Operation {
public:
    // Constructor to initialize the operation with input and output variables
    Operation(const vector<string>& inputs, const string& output);

    // Getter for input variables
    const vector<string>& getInputs() const;

    // Getter for output variable
    const string& getOutput() const;

    // Method to print the operation (for easy display)
    void printOperation() const;

    // Method to check equality of operations (optional)
    bool operator==(const Operation& other) const;

private:
    vector<string> inputs;  // Input variables
    string output;               // Output variable
};



#endif // OPERATION_H_INCLUDED
