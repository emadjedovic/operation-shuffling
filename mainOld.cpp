#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <random>
#include <stdexcept>

using namespace std;

struct Operation
{
    int label;
    vector<string> inputs;
    vector<string> outputs;

    Operation(int l, vector<string> in, vector<string> out)
        : label(l), inputs(in), outputs(out) {}
};

class OperationsGraph
{
    vector<Operation> operations;
    unordered_map<int, int> inDegree;        // Map to handle arbitrary labels
    unordered_map<int, vector<int>> adjList; // Adjacency list to handle arbitrary labels
    unordered_set<string> outputsSet;
    unordered_map<string, int> lastProducer;

    void sortZeroInDegreeOps(vector<int> &ops)
    {
        random_device rd;
        mt19937 g(3); // change input seer number manually or use rd()
        shuffle(ops.begin(), ops.end(), g);
    }

public:
    void addOperation(const Operation &op)
    {
        operations.push_back(op);
        inDegree[op.label] = 0;
        adjList[op.label] = vector<int>();

        // Check dependencies for inputs
        for (const auto &input : op.inputs)
        {
            if (lastProducer.find(input) != lastProducer.end())
            {
                int producer = lastProducer[input];

                // Add dependency
                adjList[producer].push_back(op.label);
                inDegree[op.label]++;
            }
        }

        // Update lastProducer for outputs
        for (const auto &output : op.outputs)
        {
            lastProducer[output] = op.label;
        }
    }

    vector<int> topologicalSort()
    {
        vector<int> sortedOps;
        queue<int> q;

        // Start with operations that have zero in-degree (no dependencies)
        for (const auto &op : operations)
        {
            if (inDegree[op.label] == 0)
            {
                q.push(op.label);
            }
        }

        unordered_set<int> sortedSet; // To track sorted operations

        while (!q.empty())
        {
            vector<int> currentZeroInDegreeOps;

            // Gather all current zero in-degree operations
            while (!q.empty())
            {
                currentZeroInDegreeOps.push_back(q.front());
                q.pop();
            }

            // Sort or process these in the correct order respecting dependencies
            sortZeroInDegreeOps(currentZeroInDegreeOps);

            // Process each operation with zero in-degree
            for (const auto &current : currentZeroInDegreeOps)
            {
                sortedOps.push_back(current);
                sortedSet.insert(current);

                // Update in-degree of all dependent operations
                for (const auto &neighbor : adjList[current])
                {
                    inDegree[neighbor]--;
                    if (inDegree[neighbor] == 0)
                    {
                        q.push(neighbor);
                    }
                }
            }
        }

        if (sortedOps.size() != operations.size())
        {
            throw runtime_error("Circular dependency detected.");
        }

        return sortedOps;
    }
};

int main()
{
    // ------------------------ TEST CASE 1 ------------------------
    OperationsGraph graph1;

    graph1.addOperation(Operation(0, {"c", "d", "e"}, {"a", "b"}));
    graph1.addOperation(Operation(1, {"b", "h"}, {"d", "f"}));
    graph1.addOperation(Operation(2, {"d", "k"}, {"s"}));
    graph1.addOperation(Operation(3, {"s", "t"}, {"d"}));
    graph1.addOperation(Operation(4, {"u", "v"}, {"y", "g"}));
    graph1.addOperation(Operation(5, {"d", "g"}, {"b"}));

    /*
    The input should be in format
    a, b = Operation(c, d, e)
d, f = Operation(b, h)
s = Operation(d, k)
d = Operation(s, t)
y, g = Operation(u, v)
b = Operation(d, g)
    */

    // Perform topological sorting
    try
    {
        vector<int> sortedOps = graph1.topologicalSort();
        cout << "Test Case 1 - Topologically sorted operations:\n";
        for (int op : sortedOps)
        {
            cout << op << " ";
        }
        cout << endl;
    }
    catch (const runtime_error &e)
    {
        cout << "Test Case 1 - Error: " << e.what() << endl;
    }

    // ------------------------ TEST CASE 2 ------------------------
    OperationsGraph graph2;

    graph2.addOperation(Operation(0, {"b", "c"}, {"a"}));
    graph2.addOperation(Operation(1, {"b"}, {"d"}));
    graph2.addOperation(Operation(2, {"c"}, {"e"}));
    graph2.addOperation(Operation(3, {"d", "e"}, {"f"}));
    graph2.addOperation(Operation(4, {"a", "f"}, {"g"}));

    // Perform topological sorting
    try
    {
        vector<int> sortedOps = graph2.topologicalSort();
        cout << "Test Case 2 - Topologically sorted operations:\n";
        for (int op : sortedOps)
        {
            cout << op << " ";
        }
        cout << endl;
    }
    catch (const runtime_error &e)
    {
        cout << "Test Case 2 - Error: " << e.what() << endl;
    }

    // ------------------------ TEST CASE 3 ------------------------
    OperationsGraph graph3;

    graph3.addOperation(Operation(0, {"c", "d"}, {"a", "b"}));
    graph3.addOperation(Operation(1, {"e", "f"}, {"x", "y"}));
    graph3.addOperation(Operation(2, {"b", "y"}, {"e"}));
    graph3.addOperation(Operation(3, {"d"}, {"c"}));
    graph3.addOperation(Operation(4, {"a", "x"}, {"e"}));

    // Perform topological sorting
    try
    {
        vector<int> sortedOps = graph3.topologicalSort();
        cout << "Test Case 3 - Topologically sorted operations:\n";
        for (int op : sortedOps)
        {
            cout << op << " ";
        }
        cout << endl;
    }
    catch (const runtime_error &e)
    {
        cout << "Test Case 3 - Error: " << e.what() << endl;
    }

    return 0;
}
