#ifndef OPERATION_H_INCLUDED
#define OPERATION_H_INCLUDED

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Operation {
    vector<string> inputs;
    string output;
public:
    Operation(const vector<string>& inputs, const string& output);
    const vector<string>& getInputs() const;
    const string& getOutput() const;
    void printOperation() const;
    bool operator==(const Operation& other) const;
};

#endif // OPERATION_H_INCLUDED
