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
    stack<OperationsGraph *> loopStack;                // Stack for handling loop scopes
    unordered_map<string, int> variableToOperationMap; // Map to track the operation of a variable
    unordered_map<int, vector<int>> adjList;           // Adjacency list for graph representation
    vector<int> inDegree;                              // In-degree of nodes for topological sort

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
                continue;

            // Handle FOR_BEGIN and FOR_END as operations with label -1
            if (line.find("FOR_BEGIN") != string::npos)
            {
                addOperation(Operation(-1, {}, {}, "FOR_BEGIN"));
                continue;
            }
            else if (line.find("FOR_END") != string::npos)
            {
                addOperation(Operation(-1, {}, {}, "FOR_END"));
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

    // Recursively process a loop as a subgraph
    void handleLoopSubgraph(OperationsGraph *subgraph)
    {
        // Ensure the subgraph starts with FOR_BEGIN and ends with FOR_END
        subgraph->addOperation(Operation(-1, {}, {}, "FOR_BEGIN"));

        // Shuffle operations inside the loop (subgraph)
        subgraph->shuffleOperationsStruct(subgraph->operations);

        // Add loop operations to the main graph
        for (const auto &op : subgraph->operations)
        {
            operations.push_back(op);
        }

        // Ensure the subgraph ends with FOR_END
        subgraph->addOperation(Operation(-1, {}, {}, "FOR_END"));
    }

    void addOperation(const Operation &op)
    {
        if (!loopStack.empty())
        {
            loopStack.top()->addOperation(op); // Add operation inside the current loop subgraph
        }
        else
        {
            operations.push_back(op); // Add operation outside any loop
        }

        int opIndex = operations.size() - 1;

        // Handle the case where the operation is FOR_BEGIN or FOR_END to avoid adding wrong dependencies
        if (op.expression.find("FOR_BEGIN") != string::npos || op.expression.find("FOR_END") != string::npos)
        {
            // Do not add dependencies for FOR_BEGIN and FOR_END markers.
            return;
        }

        // Add dependencies for operations inside loops or main graph
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

        // Initialize in-degree and check for any cycles directly
        vector<int> inDegreeCopy = inDegree;
        for (int i = 0; i < operations.size(); ++i)
        {
            // Ignore FOR_BEGIN and FOR_END for topological sorting, these aren't treated as operations
            if (operations[i].expression.find("FOR_BEGIN") == string::npos && operations[i].expression.find("FOR_END") == string::npos)
            {
                if (inDegreeCopy[i] == 0)
                {
                    q.push(i);
                }
            }
        }

        // Check for initial cycle in the graph
        if (q.empty())
        {
            throw runtime_error("Error: Initial graph has circular dependencies.");
        }

        bool insideForLoop = false;
        vector<int> loopOperations; // Will hold operations inside the loop

        while (!q.empty())
        {
            vector<int> zeroInDegreeOps;
            while (!q.empty())
            {
                zeroInDegreeOps.push_back(q.front());
                q.pop();
            }

            // Separate operations inside the loop (subgraph) from other operations
            vector<int> nonLoopOps;
            for (int opIndex : zeroInDegreeOps)
            {
                // Exclude FOR_BEGIN and FOR_END from the sorting process
                if (operations[opIndex].expression.find("FOR_BEGIN") == string::npos && operations[opIndex].expression.find("FOR_END") == string::npos)
                {
                    nonLoopOps.push_back(opIndex);
                }
            }

            // Shuffle non-loop operations (operations outside the loop)
            shuffleOperations(nonLoopOps);

            for (int opIndex : nonLoopOps)
            {
                // Handle FOR_BEGIN and FOR_END separately
                if (operations[opIndex].expression.find("FOR_BEGIN") != string::npos)
                {
                    if (insideForLoop)
                    {
                        throw runtime_error("Error: Nested loops are not supported.");
                    }
                    insideForLoop = true;
                    loopOperations.clear();                           // Clear previous loop's operations
                    result.push_back(operations[opIndex].expression); // Add FOR_BEGIN to the result
                    continue;
                }
                else if (operations[opIndex].expression.find("FOR_END") != string::npos)
                {
                    if (!insideForLoop)
                    {
                        throw runtime_error("Error: FOR_END encountered without corresponding FOR_BEGIN.");
                    }
                    insideForLoop = false;

                    // Shuffle operations inside the loop (subgraph)
                    shuffleOperations(loopOperations);
                    for (int loopOpIndex : loopOperations)
                    {
                        result.push_back(operations[loopOpIndex].expression);
                    }

                    result.push_back(operations[opIndex].expression); // Add FOR_END to the result
                    continue;
                }

                // If inside the loop, add the operation to loopOperations (subgraph)
                if (insideForLoop)
                {
                    loopOperations.push_back(opIndex);
                }
                else
                {
                    result.push_back(operations[opIndex].expression);
                }

                // Process neighbors for topological sort
                for (int neighborIndex : adjList[opIndex])
                {
                    if (--inDegreeCopy[neighborIndex] == 0)
                    {
                        q.push(neighborIndex);
                    }
                }
            }
        }

        // If the number of processed operations doesn't match, a cycle exists
        if (result.size() != operations.size())
        {
            throw runtime_error("Error: Circular dependency detected after processing.");
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

    // Print all operations in the subgraph
    void printSubgraph()
    {
        cout << "Subgraph operations:\n";
        for (const auto &op : operations)
        {
            cout << op.expression << endl;
        }
    }
};

int OperationsGraph::nextLabel = 0;
