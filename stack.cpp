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
#include <sstream>

using namespace std;

class LoopMarker
{
public:
    string type; // "FOR_BEGIN" or "FOR_END"

    LoopMarker(const string &type) : type(type) {}
};

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
    vector<LoopMarker> loopMarkers;

public:
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

    void handleForBegin()
    {
        loopMarkers.push_back(LoopMarker("FOR_BEGIN"));
        loopStack.push(vector<Operation>());
    }

    void handleForEnd()
    {
        if (!loopStack.empty())
        {
            vector<Operation> loopOps = loopStack.top();

            // Debugging output: Operations inside the loop
            cout << "\nInside FOR loop before shuffling:\n";
            for (const auto &op : loopOps)
                cout << op.expression << endl;

            // Shuffle the loop operations
            shuffleOperationsStruct(loopOps);

            // Debugging output: Operations after shuffling
            cout << "\nInside FOR loop after shuffling:\n";
            for (const auto &op : loopOps)
                cout << op.expression << endl;

            // Add FOR_BEGIN and loop operations
            operations.push_back(Operation(-1, {}, {}, "FOR_BEGIN(5)"));
            for (const auto &op : loopOps)
            {
                operations.push_back(op);
            }

            // Add FOR_END
            operations.push_back(Operation(-1, {}, {}, "FOR_END"));

            // Pop the loop stack to close the loop scope
            loopStack.pop();
        }
    }

    void addOperation(const Operation &op)
    {
        if (!loopStack.empty())
        {
            loopStack.top().push_back(op); // Add operation inside the current loop
        }
        else
        {
            operations.push_back(op); // Add operation outside any loop
        }

        int opIndex = operations.size() - 1;
        for (const string &inputVar : op.inputs)
        {
            if (variableToOperationMap.find(inputVar) != variableToOperationMap.end())
            {
                adjList[variableToOperationMap[inputVar]].push_back(opIndex);
                inDegree[opIndex]++;
            }
        }

        for (const string &outputVar : op.outputs)
        {
            variableToOperationMap[outputVar] = opIndex;
        }
    }

    vector<string> topSortWithShuffle()
    {
        if (operations.empty())
        {
            throw runtime_error("No operations to process.");
        }

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

        while (!q.empty())
        {
            vector<int> zeroInDegreeOps;
            while (!q.empty())
            {
                zeroInDegreeOps.push_back(q.front());
                q.pop();
            }

            // Shuffle the operations with zero in-degree
            shuffleOperations(zeroInDegreeOps);

            for (int opIndex : zeroInDegreeOps)
            {
                result.push_back(operations[opIndex].expression);

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
                result += delimiter;
        }
        return result;
    }
};

int OperationsGraph::nextLabel = 0;
