#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <random>
#include <unordered_map>
#include <queue>

using namespace std;

struct Operation
{
    int id;
    vector<string> inputs;
    vector<string> outputs;
    string expression;

    Operation(int id, vector<string> inputs, vector<string> outputs, string expr)
        : id(id), inputs(inputs), outputs(outputs), expression(expr) {}
};


class OperationsGraph
{
private:
    vector<Operation> operations;                      // Store all operations
    stack<vector<Operation>> loopStack;                // Stack for handling loop scopes
    unordered_map<string, int> variableToOperationMap; // Map to track the operation of a variable
    unordered_map<int, vector<int>> adjList;           // Adjacency list for graph representation
    vector<int> inDegree;                              // In-degree of nodes for topological sort

public:
    // Constructor to initialize adjacency list and in-degree vector
    OperationsGraph() : inDegree(1000, 0) {}

    static int nextLabel;

    // Function to process operations input (from console or file)
    void processInput(istream &inputStream)
    {
        string line;
        while (getline(inputStream, line))
        {
            if (line.empty())
            {
                continue;
            }
            // Handle FOR_BEGIN and FOR_END logic here
            if (line.find("FOR_BEGIN") != string::npos)
            {
                handleForBegin();
                continue;
            }
            else if (line.find("FOR_END") != string::npos)
            {
                handleForEnd();
                continue;
            }

            // Parse the operation in the form "a,b = Operacija(c, d, e)"
            size_t pos = line.find("=");
            if (pos != string::npos)
            {
                string operationDetails = line.substr(pos + 1);
                vector<string> inputs;
                size_t start = operationDetails.find("(") + 1;
                size_t end = operationDetails.find(")");
                string inputStr = operationDetails.substr(start, end - start);
                size_t commaPos = 0;
                while ((commaPos = inputStr.find(',')) != string::npos)
                {
                    inputs.push_back(inputStr.substr(0, commaPos));
                    inputStr.erase(0, commaPos + 1);
                }
                if (!inputStr.empty())
                {
                    inputs.push_back(inputStr);
                }

                // Add the parsed operation
                addOperation(Operation(nextLabel++, inputs, {}, line));
            }
        }
    }

    void shuffleOperationsStruct(vector<Operation> &ops)
    {
        random_device rd;
        mt19937 g(rd());
        shuffle(ops.begin(), ops.end(), g);
    }

    void shuffleOperations(vector<int> &ops)
    {
        random_device rd;
        mt19937 g(rd());
        shuffle(ops.begin(), ops.end(), g);
    }

    // Add operation to the current loop or global list
    void addOperation(const Operation &op)
    {
        if (!loopStack.empty())
        {
            loopStack.top().push_back(op); // Add operation inside the current loop
        }
        else
        {
            operations.push_back(op); // Add operation to the global list
        }

        // Map variables to the operation they belong to
        int opIndex = operations.size() - 1;
        for (const string &inputVar : op.inputs)
        {
            if (variableToOperationMap.find(inputVar) != variableToOperationMap.end())
            {
                adjList[variableToOperationMap[inputVar]].push_back(opIndex);
                inDegree[opIndex]++;
            }
        }

        // Track each output variable to this operation
        for (const string &outputVar : op.outputs)
        {
            variableToOperationMap[outputVar] = opIndex;
        }
    }

    // Handle FOR_BEGIN
    void handleForBegin()
    {
        loopStack.push(vector<Operation>{Operation(nextLabel++, {}, {}, "FOR_BEGIN")}); // Start a new loop scope
    }

    // Handle FOR_END - shuffle the loop's operations
    void handleForEnd()
    {
        if (!loopStack.empty())
        {
            vector<Operation> &loopOps = loopStack.top();
            shuffleOperationsStruct(loopOps);
            operations.insert(operations.end(), loopOps.begin(), loopOps.end()); // Move operations to global list
            operations.push_back(Operation(nextLabel++, {}, {}, "FOR_END"));
            loopStack.pop();                                                     // End the loop scope
        }
    }

    // Function to sort operations and apply shuffling obfuscation
    vector<string> topSortWithShuffle()
    {
        if (operations.empty())
        {
            throw runtime_error("No operations to process.");
        }

        // Perform topological sort on the operations
        vector<string> result;
        queue<int> q;

        // Enqueue operations with no incoming edges (in-degree 0)
        for (int i = 0; i < operations.size(); ++i)
        {
            if (inDegree[i] == 0)
            {
                q.push(i);
            }
        }

        // Process nodes in topological order
        while (!q.empty())
        {
            vector<int> zeroInDegreeOps;
            while (!q.empty())
            {
                zeroInDegreeOps.push_back(q.front());
                q.pop();
            }

            // Shuffle the operations with zero in-degree for obfuscation
            shuffleOperations(zeroInDegreeOps);

            for (int opIndex : zeroInDegreeOps)
            {
                result.push_back(operations[opIndex].expression);

                // Reduce in-degree for neighboring operations and enqueue them if they reach in-degree 0
                for (int neighborIndex : adjList[opIndex])
                {
                    if (--inDegree[neighborIndex] == 0)
                    {
                        q.push(neighborIndex);
                    }
                }
            }
        }

        if (result.size() != operations.size())
        {
            throw runtime_error("Error: Circular dependency detected.");
        }

        return result;
    }

    // Helper function to join a vector of strings into a single string with commas
    string join(const vector<string> &vec, const string &delimiter)
    {
        string result;
        for (size_t i = 0; i < vec.size(); ++i)
        {
            result += vec[i];
            if (i != vec.size() - 1)
            {
                result += delimiter;
            }
        }
        return result;
    }
};

int OperationsGraph::nextLabel = 0;
