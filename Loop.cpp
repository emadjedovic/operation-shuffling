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
    unordered_map<string, int> variableToOperationMap;

    void shuffleOperations(vector<Operation> &ops);
    vector<string> processingVars;
    bool isCircularDependency(const string &var);

    // Static variable to track the next available label
    static int nextLabel;

public:
    OperationsGraph()
    {
        adjList.resize(1000); // Example: Reserve space for 1000 operations
        inDegree.resize(1000);
    }

    vector<string> topSortWithShuffle();
    void processInputRecursively(istream &inputStream, int &label, vector<Operation> &loopOps, bool insideLoop);
    void validateAndAddOperation(const Operation &op);
    void shuffleAndValidateLoop(vector<Operation> &loopOps);
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
    definedVarsStack.push_back(definedVarsStack.back()); // Copy outer scope
}

void OperationsGraph::exitLoopScope(const unordered_set<string> &outerScope)
{
    definedVarsStack.pop_back(); // Remove inner scope

    // Restore variable mappings to the outer scope
    for (const auto &var : outerScope)
    {
        if (variableScopeMap.find(var) != variableScopeMap.end())
        {
            variableScopeMap[var] = definedVarsStack.size() - 1;
        }
    }

    // Retain variables that are still used outside the loop
    unordered_set<string> retainedVars;
    for (const auto &var : variableScopeMap)
    {
        if (outerScope.find(var.first) != outerScope.end())
        {
            retainedVars.insert(var.first);
        }
    }

    // Remove invalid mappings while retaining outer-scope variables
    for (auto it = variableScopeMap.begin(); it != variableScopeMap.end();)
    {
        if (retainedVars.find(it->first) == retainedVars.end())
        {
            it = variableScopeMap.erase(it);
        }
        else
        {
            ++it;
        }
    }
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
            cout << "current.label: " << current.label << endl;
            cout << "operations[current.label].originalExpression: " << operations[current.label].originalExpression << endl;

            newOrder.push_back(operations[current.label].originalExpression);

            for (const auto &neighbor : adjList[current.label])
            {
                cout << "Processing neighbor: " << neighbor.originalExpression << endl;

                inDegree[neighbor.label]--; // Decrease the in-degree for the dependent operation
                if (inDegree[neighbor.label] == 0)
                {
                    q.push(neighbor); // If in-degree is now 0, push it to the queue
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

void OperationsGraph::validateAndAddOperation(const Operation &op)
{
    int operationLabel = nextLabel++; // Assign current label, then increment it
    cout << "operationLabel: " << operationLabel << endl;

    // Add the operation to the destination list with the new label
    Operation opWithLabel = op; // Make a copy of the operation
    opWithLabel.label = operationLabel;
    if (operations.size() <= operationLabel)
    {
        operations.push_back(opWithLabel);
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
        for (int i = definedVarsStack.size() - 1; i >= 0; --i)
        {
            if (definedVarsStack[i].find(inputVar) != definedVarsStack[i].end())
            {
                found = true;
                variableScopeMap[inputVar] = i; // Update to correct scope
                break;
            }
        }
        if (!found)
        {
            // Variable not found, add it to the current scope
            definedVarsStack.back().insert(inputVar);
            variableScopeMap[inputVar] = definedVarsStack.size() - 1;
        }
    }

    for (const string &inputVar : op.inputs)
    {
        cout << "inputVar: " << inputVar << endl;
        bool isNewVar = variableScopeMap.find(inputVar) == variableScopeMap.end(); // Check if the variable is new

        if (isNewVar)
        {
            // This is a new variable, so don't increment inDegree yet.
            // You can optionally track new variables if needed.
            cout << "is new variable!" << endl;
        }
        else
        {
            cout << "operations size: " << operations.size() << endl;

            for (const string &outputVar : op.outputs)
            {
                // Update variable scope map
                variableScopeMap[outputVar] = definedVarsStack.size() - 1;

                // Map variable to operation
                variableToOperationMap[outputVar] = operationLabel;

                // Handle dependencies for input variables
                if (variableToOperationMap.count(inputVar))
                {
                    int dependentOpLabel = variableToOperationMap[inputVar];
                    adjList[dependentOpLabel].push_back(opWithLabel);
                    inDegree[operationLabel]++;
                }
            }
        }
    }

    for (const string &inputVar : op.inputs)
        processingVars.erase(remove(processingVars.begin(), processingVars.end(), inputVar), processingVars.end());
}

void OperationsGraph::shuffleAndValidateLoop(vector<Operation> &loopOps)
{
    Operation forBegin = loopOps.front();
    Operation forEnd = loopOps.back();
    vector<Operation> shuffledOps(loopOps.begin() + 1, loopOps.end() - 1);

    shuffleOperations(shuffledOps);

    vector<Operation> validatedOps = {forBegin};
    for (Operation &op : shuffledOps)
        validateAndAddOperation(op);
    validatedOps.push_back(forEnd);
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

static const int MAX_RECURSION_DEPTH = 1000; // Example limit

void OperationsGraph::processInputRecursively(istream &inputStream, int &label, vector<Operation> &loopOps, bool insideLoop)
{
    static int recursionDepth = 0;
    recursionDepth++;
    if (recursionDepth > MAX_RECURSION_DEPTH)
    {
        throw std::runtime_error("Error: Maximum recursion depth exceeded.");
    }

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
                validateAndAddOperation(forBeginOp);

            vector<Operation> nestedLoopOps;
            processInputRecursively(inputStream, label, nestedLoopOps, true);

            exitLoopScope(outerScopeDefinedVars);
            shuffleAndValidateLoop(nestedLoopOps);
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
                validateAndAddOperation(forEndOp);
            }

            continue;
        }

        try
        {
            Operation op = parseInput(line, label);

            if (insideLoop)
                loopOps.push_back(op);
            else
                validateAndAddOperation(op);
        }
        catch (const invalid_argument &e)
        {
            cout << "[ERROR] Invalid input format: " << e.what() << endl;
        }
    }

    recursionDepth--;
    cout << "Outer scope before exiting loop: ";
    for (const auto &var : outerScopeDefinedVars)
        cout << var << " ";
    cout << endl;

    cout << "Variables in definedVarsStack: ";
    for (const auto &scope : definedVarsStack)
    {
        for (const auto &var : scope)
            cout << var << " ";
        cout << "| ";
    }
    cout << endl;

    // Propagate variables defined inside the loop to the outer scope
    for (const auto &var : definedVarsStack.back())
    {
        outerScopeDefinedVars.insert(var);
    }

    loopOps.clear();
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
