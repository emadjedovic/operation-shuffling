#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <random>
#include <stdexcept>
#include <cstdlib>
#include <queue>
#include <unordered_set>

using namespace std;

struct Operation
{
    int label;
    vector<string> inputs;
    vector<string> outputs;
    string originalExpression;

    Operation(int l, vector<string> in, vector<string> out, const string &expr);
};

class OperationsGraph
{
private:
    vector<Operation> operations;
    vector<int> inDegree;
    vector<vector<int>> adjList;
    unordered_map<string, int> variableLastOrigin; // for input/output relationships

    void shuffleZeroIndegreeOps(vector<int> &ops);

public:
    void addOperation(const Operation &op);
    vector<string> topSortWithShuffle();
};

// IMPLEMENTATIONS

Operation::Operation(int l, vector<string> in, vector<string> out, const string &expr)
    : label(l), inputs(in), outputs(out), originalExpression(expr) {}

void OperationsGraph::shuffleZeroIndegreeOps(vector<int> &ops)
{
    random_device rd;
    mt19937 g(rd()); // we can manually enter a seed number here
    shuffle(ops.begin(), ops.end(), g);
}

void OperationsGraph::addOperation(const Operation &op)
{
    operations.push_back(op);
    inDegree.push_back(0);
    adjList.push_back(vector<int>());

    // Check dependencies for inputs
    for (const auto &input : op.inputs)
    {
        if (variableLastOrigin.find(input) != variableLastOrigin.end())
        {
            int variableOriginOp = variableLastOrigin[input];

            // Add dependency
            adjList[variableOriginOp].push_back(op.label);
            inDegree[op.label]++;
        }
    }

    // Update variableLastOrigin for outputs
    for (const auto &output : op.outputs)
        variableLastOrigin[output] = op.label;
}

vector<string> OperationsGraph::topSortWithShuffle()
{
    vector<string> newOrder;
    queue<int> q;

    // Start with operations that have zero in-degree (no dependencies)
    for (const auto &op : operations)
        if (inDegree[op.label] == 0)
            q.push(op.label);


    while (!q.empty())
    {
        vector<int> currentZeroIndegreeOps;

        while (!q.empty())
        {
            currentZeroIndegreeOps.push_back(q.front());
            q.pop();
        }

        // shuffle only zero in-degree operations (no dependencies)
        shuffleZeroIndegreeOps(currentZeroIndegreeOps);

        for (const auto &current : currentZeroIndegreeOps)
        {
            newOrder.push_back(operations[current].originalExpression);

            // Update in-degree of all dependent operations
            for (const auto &neighbor : adjList[current])
            {
                inDegree[neighbor]--;
                if (inDegree[neighbor] == 0)
                    q.push(neighbor);
            }
        }
    }

    if (newOrder.size() != operations.size())
    {
        cout << "Error: Circular dependency detected." << endl;
        exit(1); // end program
    }

    return newOrder;
}

Operation parseInput(const string &line, int label)
{
    stringstream ss(line);
    string outputs, inputs;
    size_t equalsPos = line.find('=');

    if (equalsPos == string::npos)
    {
        cout << "Error: Invalid input format. Missing '='." << endl;
        exit(1); // end program
    }

    outputs = line.substr(0, equalsPos);
    inputs = line.substr(equalsPos + 1);

    // Trim spaces
    outputs.erase(remove(outputs.begin(), outputs.end(), ' '), outputs.end());
    inputs.erase(remove(inputs.begin(), inputs.end(), ' '), inputs.end());

    // Parse outputs (comma-separated)
    vector<string> outputVars;
    stringstream outputSS(outputs);
    string output;
    while (getline(outputSS, output, ','))
        outputVars.push_back(output);

    // Parse inputs (inside parentheses)
    size_t start = inputs.find('(');
    size_t end = inputs.find(')');

    if (start == string::npos || end == string::npos || start >= end)
    {
        cout << "Error: Invalid input format. Missing parentheses." << endl;
        exit(1); // end program
    }

    string inputVars = inputs.substr(start + 1, end - start - 1);
    vector<string> inputVarsList;
    stringstream inputSS(inputVars);
    string input;
    while (getline(inputSS, input, ','))
        inputVarsList.push_back(input);

    return Operation(label, inputVarsList, outputVars, line);
}

void processInput(std::istream &inputStream, OperationsGraph &graph)
{
    string line;
    int label = 0;

    while (getline(inputStream, line))
    {
        if (line.empty())
            continue;

        try
        {
            Operation op = parseInput(line, label);
            graph.addOperation(op);
            label++;
        }
        catch (const invalid_argument &e)
        {
            cout << "Error: " << e.what() << endl;
        }
    }
}
