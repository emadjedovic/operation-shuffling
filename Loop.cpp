#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <random>
#include <stdexcept>
#include <queue>
#include <unordered_set>

using namespace std;

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
private:
    vector<Operation> operations;
    vector<int> inDegree;
    vector<vector<Operation>> adjList;
    unordered_map<string, int> variableLastOrigin;
    vector<unordered_set<string>> definedVarsStack;
    unordered_map<string, int> variableScopeMap;
    void shuffleOperations(vector<Operation> &ops);
    vector<string> processingVars;
    bool isCircularDependency(const string &var);

    // Static variable to track the next available label
    static int nextLabel;

public:
    vector<string> topSortWithShuffle();
    void processInputRecursively(istream &inputStream, int &label, vector<Operation> &loopOps, bool insideLoop);
    void validateAndAddOperation(const Operation &op, vector<Operation> &destination);
    void shuffleAndValidateLoop(vector<Operation> &loopOps, vector<Operation> &mainOps);
    void enterLoopScope();
    void exitLoopScope(const unordered_set<string> &outerScope);
};

int OperationsGraph::nextLabel = 0;

bool OperationsGraph::isCircularDependency(const string &var)
{
    return find(processingVars.begin(), processingVars.end(), var) != processingVars.end();
}

void OperationsGraph::enterLoopScope()
{
    definedVarsStack.push_back(unordered_set<string>(definedVarsStack.back()));
}

void OperationsGraph::exitLoopScope(const unordered_set<string> &outerScope)
{
    definedVarsStack.pop_back();
    definedVarsStack.push_back(outerScope);
}

void OperationsGraph::shuffleOperations(vector<Operation> &ops)
{
    random_device rd;
    mt19937 g(rd());
    shuffle(ops.begin(), ops.end(), g);
}

vector<string> OperationsGraph::topSortWithShuffle()
{
    vector<string> newOrder;
    queue<Operation> q;

    cout << "operations.size: " << operations.size() << endl;
    
    // Debugging inDegree
    cout << "inDegree before processing:\n";
    for (int i = 0; i < inDegree.size(); ++i)
    {
        cout << "inDegree[" << i << "]: " << inDegree[i] << endl;
    }
    cout<<"adjlist[0].size: "<<adjList[0].size()<<endl;
    cout<<"AdjList:\n";
    for (int i = 0; i < adjList.size(); ++i)
    {
        for(int j=0; j<adjList[i].size(); j++)
        cout << "adjList[" << i << "]["<<j<<"]: " << adjList[i][j].label << endl;
    }

    // Initialize queue with operations that have no incoming dependencies (inDegree == 0)
    for (const auto &op : operations)
    {
        if (inDegree[op.label] == 0)
        {
            q.push(op);
        }
    }

    while (!q.empty())
    {
        vector<Operation> currentZeroIndegreeOps;
        while (!q.empty())
        {
            currentZeroIndegreeOps.push_back(q.front());
            q.pop();
        }

        // Shuffle operations with in-degree of 0
        shuffleOperations(currentZeroIndegreeOps);

        // Process each operation
        for (const auto &current : currentZeroIndegreeOps)
        {
            cout << "operations[current.label].originalExpression: " << operations[current.label].originalExpression << endl;

            newOrder.push_back(operations[current.label].originalExpression);

            // Process neighbors (dependencies)
            cout<<"adjList[current.label].label = "<<adjList[current.label][0].label<<endl;
            for (const auto &neighbor : adjList[current.label])
            {
                cout << "Processing neighbor: " << neighbor.originalExpression << endl;

                inDegree[neighbor.label]--;  // Decrease the in-degree for the dependent operation
                if (inDegree[neighbor.label] == 0)
                {
                    q.push(neighbor);  // If in-degree is now 0, push it to the queue
                }
            }
        }
    }

    cout << "newOrder size: " << newOrder.size() << endl;
    cout << "operations size: " << operations.size() << endl;

    if (newOrder.size() != operations.size())
    {
        throw runtime_error("Error: Circular dependency detected.");
    }

    cout << "Returning newOrder with size: " << newOrder.size() << endl;
    return newOrder;
}


void OperationsGraph::validateAndAddOperation(const Operation &op, vector<Operation> &destination)
{
    int operationLabel = nextLabel++; // Assign current label, then increment it

    // Add the operation to the destination list with the new label
    Operation opWithLabel = op; // Make a copy of the operation
    opWithLabel.label = operationLabel;
    if (operations.size() <= operationLabel)
    {
        // Push the operation to the main operations list
        operations.push_back(opWithLabel); // Now the operations vector is updated
    }

    if (definedVarsStack.empty())
    {
        definedVarsStack.push_back(unordered_set<string>());
    }

    for (const string &inputVar : op.inputs)
    {
        if (isCircularDependency(inputVar))
            throw std::runtime_error("Circular dependency detected for variable: " + inputVar);

        processingVars.push_back(inputVar);
        bool found = false;

        if (variableScopeMap.find(inputVar) != variableScopeMap.end() &&
            variableScopeMap[inputVar] == definedVarsStack.size() - 1)
        {
            found = true;
        }
        else
        {
            for (int i = definedVarsStack.size() - 2; i >= 0; --i)
                if (i >= 0 && definedVarsStack[i].find(inputVar) != definedVarsStack[i].end())
                {
                    found = true;
                    break;
                }
        }

        if (!found)
        {
            if (definedVarsStack.empty())
            {
                cout << "definedVarsStack is empty!" << endl;
                // return; // Exit or handle the error gracefully
            }
            definedVarsStack.back().insert(inputVar);
            variableScopeMap[inputVar] = definedVarsStack.size() - 1;
        }
    }

    destination.push_back(opWithLabel);

    // Dynamically resize adjList to ensure we have a corresponding entry for the new operation
    if (adjList.size() <= operationLabel)
    {
        adjList.resize(operationLabel + 1);
    }
    if (inDegree.size() <= operationLabel)
    {
        inDegree.resize(operationLabel + 1);
    }

    for (const string &outputVar : op.outputs)
    {
        definedVarsStack.back().insert(outputVar);
        variableScopeMap[outputVar] = definedVarsStack.size() - 1; // Track this output variable in the current scope

        // Now, for each operation that depends on this outputVar, we increment their inDegree
        for (int i = 0; i < operations.size(); ++i)
        {
            for (const string &inputVar : operations[i].inputs)
            {
                if (inputVar == outputVar)
                {
                    // Increment the inDegree for any operation that depends on this outputVar
                    if (i >= inDegree.size())
                        inDegree.resize(i + 1, 0); // Resize inDegree if needed
                    inDegree[i]++;

                    // Add the dependent operation to the adjacency list of the current operation
                    adjList[operationLabel-1].push_back(operations[i]);
                    // Debug: print adjacency list
                    cout << "Adding neighbor: " << operations[i].originalExpression << " to " << opWithLabel.originalExpression << endl;
                }
            }
        }
    }

    for (const string &inputVar : op.inputs)
        processingVars.erase(remove(processingVars.begin(), processingVars.end(), inputVar), processingVars.end());
}

void OperationsGraph::shuffleAndValidateLoop(vector<Operation> &loopOps, vector<Operation> &mainOps)
{
    Operation forBegin = loopOps.front();
    Operation forEnd = loopOps.back();
    vector<Operation> shuffledOps(loopOps.begin() + 1, loopOps.end() - 1);

    shuffleOperations(shuffledOps);

    vector<Operation> validatedOps = {forBegin};
    for (Operation &op : shuffledOps)
        validateAndAddOperation(op, validatedOps);
    validatedOps.push_back(forEnd);

    for (const Operation &op : validatedOps)
        mainOps.push_back(op);
}

Operation parseInput(const string &line, int label)
{
    stringstream ss(line);
    string outputs, inputs;
    size_t equalsPos = line.find('=');

    if (equalsPos == string::npos)
        throw invalid_argument("Error: Invalid input format. Missing '='.");

    outputs = line.substr(0, equalsPos);
    inputs = line.substr(equalsPos + 1);

    outputs.erase(remove(outputs.begin(), outputs.end(), ' '), outputs.end());
    inputs.erase(remove(inputs.begin(), inputs.end(), ' '), inputs.end());

    vector<string> outputVars;
    stringstream outputSS(outputs);
    string output;
    while (getline(outputSS, output, ','))
        outputVars.push_back(output);

    size_t start = inputs.find('(');
    size_t end = inputs.find(')');

    if (start == string::npos || end == string::npos || start >= end)
        throw invalid_argument("Error: Invalid input format. Missing parentheses.");

    string inputVars = inputs.substr(start + 1, end - start - 1);
    vector<string> inputVarsList;
    stringstream inputSS(inputVars);
    string input;
    while (getline(inputSS, input, ','))
        inputVarsList.push_back(input);

    return Operation(label, inputVarsList, outputVars, line);
}

void OperationsGraph::processInputRecursively(istream &inputStream, int &label, vector<Operation> &loopOps, bool insideLoop)
{
    static int recursionDepth = 0;
    recursionDepth++;
    string line;
    unordered_set<string> outerScopeDefinedVars = definedVarsStack.empty() ? unordered_set<string>() : definedVarsStack.back();

    while (getline(inputStream, line))
    {
        if (line.empty())
            continue;

        if (line.find("FOR_BEGIN") != string::npos)
        {
            enterLoopScope();
            Operation forBeginOp(label++, {}, {}, "FOR_BEGIN");
            if (insideLoop)
                loopOps.push_back(forBeginOp);
            else
                validateAndAddOperation(forBeginOp, operations);

            vector<Operation> nestedLoopOps;
            processInputRecursively(inputStream, label, nestedLoopOps, true);

            exitLoopScope(outerScopeDefinedVars);
            shuffleAndValidateLoop(nestedLoopOps, operations);
            continue;
        }

        if (line.find("FOR_END") != string::npos)
        {
            if (insideLoop)
            {
                loopOps.push_back(Operation(label++, {}, {}, "FOR_END"));
                exitLoopScope(outerScopeDefinedVars);
                return;
            }
            else
            {
                Operation forEndOp(label++, {}, {}, "FOR_END");
                validateAndAddOperation(forEndOp, operations);
            }

            continue;
        }

        try
        {
            Operation op = parseInput(line, label);

            if (insideLoop)
                validateAndAddOperation(op, loopOps);
            else
                validateAndAddOperation(op, operations);
        }
        catch (const invalid_argument &e)
        {
            cout << "[ERROR] Invalid input format: " << e.what() << endl;
        }
    }

    recursionDepth--;
    exitLoopScope(outerScopeDefinedVars);
}

void processInput(istream &inputStream, OperationsGraph &graph)
{
    if (!inputStream)
    {
        cout << "[ERROR] Input stream is not valid!" << endl;
        return;
    }

    string line;
    int label = 0;
    vector<Operation> loopOps;
    graph.processInputRecursively(inputStream, label, loopOps, false);
}
