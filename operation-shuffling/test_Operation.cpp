#include <iostream>
#include <cassert>
#include "Operation.h"

void testConstructorAndGetters() {
    vector<string> inputs = {"a", "b", "c"};
    string output = "result";
    Operation op(inputs, output);

    // If the condition evaluates to false, the program terminates
    // and reports the failed condition along with information to help debug.
    assert(op.getInputs().size() == 3);
    assert(op.getInputs()[0] == "a");
    assert(op.getInputs()[1] == "b");
    assert(op.getInputs()[2] == "c");
    assert(op.getOutput() == "result");

    cout << "testConstructorAndGetters passed!" << endl;
}

void testPrintOperation() {
    vector<string> inputs = {"x", "y"};
    string output = "sum";
    Operation op(inputs, output);

    cout << "Expected: sum = x, y" << endl;
    cout << "Actual: ";
    op.printOperation();
}

void testEqualityOperator() {
    vector<string> inputs1 = {"a", "b"};
    string output1 = "sum";
    Operation op1(inputs1, output1);

    vector<string> inputs2 = {"a", "b"};
    string output2 = "sum";
    Operation op2(inputs2, output2);

    assert(op1 == op2);

    cout << "testEqualityOperator passed!" << endl;
}

int main() {
    testConstructorAndGetters();
    testPrintOperation();
    testEqualityOperator();

    return 0;
}
