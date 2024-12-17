#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <algorithm>
#include <random>

using namespace std;

// Structure to hold an operation
struct Operation
{
    string code_line;   // Original code line
    string output_vars; // Variables written to
    string input_vars;  // Variables read from
    bool is_in_loop;    // Flag to check if the operation is inside a loop
    int loop_level;     // To track nesting of loops (optional)
};

// Class to represent the dependency graph of operations
class OperationsGraph
{
private:
    vector<Operation> operations;
    unordered_map<int, vector<int>> graph;   // Dependency graph
    vector<int> inDegree;                    // In-degree for each operation
    unordered_map<string, int> lastProduced; // Tracks last operation producing a variable

public:
    // Add an operation and build the dependency graph
    void addOperation(const Operation &op)
    {
        operations.push_back(op);
        inDegree.push_back(0);

        // Skip loop boundaries (FOR_BEGIN, FOR_END)
        if (op.is_in_loop)
            return;

        // Debugging: Print the operation being added
        // cout << "Adding operation: " << op.code_line << endl;

        // Track dependencies
        for (char var : op.input_vars)
        {
            // If a variable is an input, it must have been produced by a previous operation
            if (lastProduced.count(string(1, var)))
            {
                int depIndex = lastProduced[string(1, var)];
                graph[depIndex].push_back(operations.size() - 1);
                inDegree[operations.size() - 1]++;
            }
        }
        for (char var : op.output_vars)
        {
            lastProduced[string(1, var)] = operations.size() - 1;
        }
    }

    // Perform randomized topological sort to shuffle the operations while respecting dependencies
    vector<int> randomizedTopologicalSort()
    {
        queue<int> q;
        vector<int> result;

        // Step 1: Collect nodes with in-degree = 0
        vector<int> zeroDegreeNodes;
        for (int i = 0; i < operations.size(); i++)
        {
            // Skip loop boundaries in the zero-degree list
            if (inDegree[i] == 0 && !operations[i].is_in_loop)
            {
                zeroDegreeNodes.push_back(i);
            }
        }

        // Debugging: Print initial zero-degree nodes
        cout << "Initial zero-degree nodes: ";
        for (int node : zeroDegreeNodes)
        {
            cout << node << " ";
        }
        cout << endl;

        // Step 2: Randomized processing of nodes
        random_device rd;
        mt19937 gen(rd());

        while (!zeroDegreeNodes.empty())
        {
            // Shuffle nodes with in-degree = 0 before each processing step
            shuffle(zeroDegreeNodes.begin(), zeroDegreeNodes.end(), gen);

            // Debugging: Print zero-degree nodes before processing
            cout << "Shuffling and processing zero-degree nodes: ";
            for (int node : zeroDegreeNodes)
            {
                cout << node << " ";
            }
            cout << endl;

            // Take one node, process it
            int current = zeroDegreeNodes.back();
            zeroDegreeNodes.pop_back();
            result.push_back(current);

            // Debugging: Print current node being processed
            cout << "Processing node: " << current << " (" << operations[current].code_line << ")" << endl;

            // Reduce in-degree of neighbors and collect new zero-degree nodes
            for (int neighbor : graph[current])
            {
                inDegree[neighbor]--;
                if (inDegree[neighbor] == 0 && !operations[neighbor].is_in_loop)
                {
                    zeroDegreeNodes.push_back(neighbor);
                }
            }

            // Debugging: Print zero-degree nodes after processing
            cout << "Zero-degree nodes after processing: ";
            for (int node : zeroDegreeNodes)
            {
                cout << node << " ";
            }
            cout << endl;
        }

        // Step 3: Check for cycles
        if (result.size() != operations.size())
        {
            cerr << "Error: Graph has a cycle!" << endl;
            exit(1);
        }

        return result;
    }

    // Print the shuffled operations
    void printShuffledOperations(const vector<int> &order)
    {
        for (int idx : order)
        {
            cout << operations[idx].code_line << endl;
        }
    }

    // Method to add operations based on input code
    void parseAndAddOperations(const vector<string> &code)
    {
        for (const string &line : code)
        {
            Operation op = parseOperation(line);
            addOperation(op);
        }
    }

private:
    // Simple parser to extract input/output variables from a line
    Operation parseOperation(const string &line)
    {
        Operation op;
        size_t eqPos = line.find("=");
        op.output_vars = line.substr(0, eqPos);
        op.input_vars = line.substr(eqPos + 1);
        op.code_line = line;
        op.is_in_loop = false;
        op.loop_level = 0;

        // Detect if the operation is a loop boundary
        if (line.find("FOR_BEGIN") != string::npos)
        {
            op.is_in_loop = true;
            op.loop_level = 1; // For simplicity, we can use 1 as a flag for top-level loops
        }
        else if (line.find("FOR_END") != string::npos)
        {
            op.is_in_loop = true;
            op.loop_level = -1; // Flag to denote end of a loop
        }
        return op;
    }
};

// Function to obfuscate the algorithm
void obfuscateAlgorithm(vector<string> &code)
{
    OperationsGraph graph;

    // Step 1: Parse the code and add operations to the graph
    graph.parseAndAddOperations(code);

    // Step 2: Perform randomized topological sort
    vector<int> shuffledOrder = graph.randomizedTopologicalSort();

    // Step 3: Print the shuffled operations
    graph.printShuffledOperations(shuffledOrder);
}

int main()
{
    vector<string> code = {
        "a = fun(b, c)",
        "d = fun(b)",
        "e = fun(c)",
        "some1 = fun(bee)",
        "some2 = fun(elk)",
        "some3 = fun(tea)",
        "f = fun(d, e)",
        "g = fun(a, f)"};

    obfuscateAlgorithm(code);
    return 0;
}
