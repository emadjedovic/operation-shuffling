#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <random>
#include <stdexcept>
#include <sstream>
#include <cstdlib>
#include <direct.h>

using namespace std;

// boilerplate
string getCurrentWorkingDirectory()
{
#ifdef _WIN32
    char buffer[FILENAME_MAX];
    _getcwd(buffer, FILENAME_MAX);
    return string(buffer);
#else
    char buffer[FILENAME_MAX];
    getcwd(buffer, FILENAME_MAX);
    return string(buffer);
#endif
}

struct Operation
{
    int label;
    vector<string> inputs;
    vector<string> outputs;
    string originalExpression;

    Operation(int l, vector<string> in, vector<string> out, const string &expr)
        : label(l), inputs(in), outputs(out), originalExpression(expr) {}
};

class OperationsGraph
{
    vector<Operation> operations;
    unordered_map<int, int> inDegree; // can we use vector<int> instead?
    unordered_map<int, vector<int>> adjList; // can we use vector<int> instead?
    unordered_map<string, int> lastProducer; // what is this? rename it for better understanding

    void shuffleZeroIndegreeOps(vector<int> &ops)
    {
        random_device rd;
        mt19937 g(rd());
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
            lastProducer[output] = op.label;
    }

    vector<string> topologicalSort()
    {
        vector<string> sortedOps;
        queue<int> q;

        // Start with operations that have zero in-degree (no dependencies)
        for (const auto &op : operations)
        {
            if (inDegree[op.label] == 0)
            {
                q.push(op.label);
            }
        }

        unordered_set<int> sortedSet;

        while (!q.empty())
        {
            vector<int> currentZeroIndegreeOps;

            // Gather all current zero in-degree operations
            while (!q.empty())
            {
                currentZeroIndegreeOps.push_back(q.front());
                q.pop();
            }

            // with respect to dependencies
            shuffleZeroIndegreeOps(currentZeroIndegreeOps);

            for (const auto &current : currentZeroIndegreeOps)
            {
                sortedOps.push_back(operations[current].originalExpression);
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
            // cout something instead of throwing errors
            throw runtime_error("Circular dependency detected.");
        }

        return sortedOps;
    }
};

Operation parseInput(const string &line, int label)
{
    stringstream ss(line);
    string outputs, inputs;
    size_t equalsPos = line.find('=');

    if (equalsPos == string::npos)
    {
        // cout something instead of throwig errors
        throw invalid_argument("Invalid input format. Missing '='.");
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
    {
        outputVars.push_back(output);
    }

    // Parse inputs (inside parentheses)
    size_t start = inputs.find('(');
    size_t end = inputs.find(')');

    if (start == string::npos || end == string::npos || start >= end)
    {
        throw invalid_argument("Invalid input format. Missing parentheses.");
    }

    string inputVars = inputs.substr(start + 1, end - start - 1);
    vector<string> inputVarsList;
    stringstream inputSS(inputVars);
    string input;
    while (getline(inputSS, input, ','))
    {
        inputVarsList.push_back(input);
    }

    return Operation(label, inputVarsList, outputVars, line);
}

void processInput(istream &inputStream, OperationsGraph &graph)
{
    string line;
    int label = 0;

    while (getline(inputStream, line))
    {
        if (line.empty())
        {
            continue;
        }

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

int main()
{
    OperationsGraph graph;

    cout << "Enter '1' for console input or '2' for file input: ";
    int choice;
    cin >> choice;
    cin.ignore(); // Clear newline character from input buffer

    if (choice == 1)
    {
        cout << "Enter operations (empty line to finish):\n";
        processInput(cin, graph);  // Standard input (console)
    }
    else if (choice == 2)
    {
        string filename;
        cout << "Enter the filename (e.g., test1.txt): ";
        getline(cin, filename);

        // Dynamically determine the file's absolute path
        string currentPath = getCurrentWorkingDirectory();
        // Adjust file path to be relative to the project's root directory
        // Assuming "test-files" is in the root of your project
        // Navigate one level up from the "output" directory to the project root
        string filePath = currentPath.substr(0, currentPath.find_last_of("\\/")) + "/test-files/" + filename;

        cout << "Current working directory: " << currentPath << endl;
cout << "Attempting to open file: " << filePath << endl;


        // Open the file
        ifstream file(filePath);
        if (!file.is_open())
        {
            cout << "Error: Could not open file at " << filePath << endl;
            return 1;
        }

        cout << "Reading operations from file: " << filePath << endl;
        processInput(file, graph);  // File input (file stream)
        file.close();  // Close the file after processing
    }
    else
    {
        cout << "Invalid choice. Exiting." << endl;
        return 1;
    }

    // Perform topological sorting
    try
    {
        vector<string> sortedOps = graph.topologicalSort();
        cout << "\nOBFUSCATED:\n";
        for (const auto &op : sortedOps)
        {
            cout << op << endl;
        }
    }
    catch (const runtime_error &e)
    {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}
