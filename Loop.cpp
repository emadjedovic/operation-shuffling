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

    static int nextLabel;

public:
    OperationsGraph()
    {
        adjList.resize(1000); // Reserve space for 1000 operations
        inDegree.resize(1000);
    }

    vector<string> topSortWithShuffle();
    void processInputRecursively(istream &inputStream, int &label, vector<Operation> &loopOps, bool insideLoop);
    void validateAndAddOperation(const Operation &op);
    void shuffleAndValidateLoop(vector<Operation> &loopOps);
    void enterLoopScope();
    void exitLoopScope(const unordered_set<string> &outerScope);
    void processSubgraph(const vector<Operation> &loopOps);

    void addOperation(Operation &op, vector<Operation> &loopOps, bool insideLoop);
    int getNextLabel();
};

int OperationsGraph::nextLabel = 0;

int OperationsGraph::getNextLabel()
{
    return nextLabel++; // Return the current label and then increment it
}

void OperationsGraph::addOperation(Operation &op, vector<Operation> &loopOps, bool insideLoop)
{
    cout << "[DEBUG] Adding operation: " << op.originalExpression << endl;

    if (insideLoop)
    {
        cout << "[DEBUG] Operation " << op.originalExpression << " is inside a loop." << endl;
    }

    for (const auto &dep : op.inputs)
    {
        cout << "[DEBUG] Operation " << op.originalExpression << " depends on variable: " << dep << endl;
    }

    // Add operation to the vector or graph
    loopOps.push_back(op);
    operations.push_back(op);
}

bool OperationsGraph::isCircularDependency(const string &var)
{
    cout << "[DEBUG] Checking if " << var << " is part of the current processing stack." << endl;

    bool isCircular = find(processingVars.begin(), processingVars.end(), var) != processingVars.end();

    if (isCircular)
    {
        cout << "[DEBUG] Circular dependency detected for variable: " << var << endl;
    }
    else
    {
        cout << "[DEBUG] No circular dependency for variable: " << var << endl;
    }

    return isCircular;
}

void OperationsGraph::enterLoopScope()
{
    cout << "[DEBUG] Entering loop scope. Creating a new empty scope." << endl;

    // Before creating a new scope, log the current state of defined variables
    cout << "[DEBUG] Current definedVarsStack size: " << definedVarsStack.size() << endl;
    if (!definedVarsStack.empty())
    {
        cout << "[DEBUG] Current defined variables: ";
        for (const auto &var : definedVarsStack.back())
        {
            cout << var << " ";
        }
        cout << endl;
    }

    // Create a new empty scope for the loop
    definedVarsStack.push_back(unordered_set<string>());

    cout << "[DEBUG] New definedVarsStack size: " << definedVarsStack.size() << endl;
    cout << "[DEBUG] Loop scope initialized. No variables defined yet in this scope." << endl;
}

void OperationsGraph::exitLoopScope(const unordered_set<string> &outerScope)
{
    cout << "[DEBUG] Exiting loop scope. Current definedVarsStack size: " << definedVarsStack.size() << endl;

    if (definedVarsStack.size() > 1)
    {
        cout << "[DEBUG] Removing loop scope from stack." << endl;
        definedVarsStack.pop_back(); // Pop the loop scope
    }

    // Debug: Check the state of the variableScopeMap before cleaning up
    cout << "[DEBUG] Current state of variableScopeMap before cleanup: " << endl;
    for (const auto &entry : variableScopeMap)
    {
        cout << "Variable: " << entry.first << " Scope: " << entry.second << endl;
    }

    // Clean up variables that are not in the outer scope
    for (auto it = variableScopeMap.begin(); it != variableScopeMap.end();)
    {
        if (outerScope.find(it->first) == outerScope.end())
        {
            cout << "[DEBUG] Removing variable " << it->first << " from scope map." << endl;
            it = variableScopeMap.erase(it); // Remove variables not part of the outer scope
        }
        else
        {
            ++it;
        }
    }

    // Debug: Check the state of variableScopeMap after cleanup
    cout << "[DEBUG] ExitLoopScope complete. Remaining variableScopeMap size: " << variableScopeMap.size() << endl;
    for (const auto &entry : variableScopeMap)
    {
        cout << "Variable: " << entry.first << " Scope: " << entry.second << endl;
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

    for (const auto &op : operations)
        if (inDegree[op.label] == 0)
            q.push(op);

    while (!q.empty())
    {
        vector<Operation> currentZeroIndegreeOps;
        while (!q.empty())
        {
            currentZeroIndegreeOps.push_back(q.front());
            q.pop();
        }

        shuffleOperations(currentZeroIndegreeOps);

        for (const auto &current : currentZeroIndegreeOps)
        {
            newOrder.push_back(operations[current.label].originalExpression);

            for (const auto &neighbor : adjList[current.label])
            {
                inDegree[neighbor.label]--;
                if (inDegree[neighbor.label] == 0)
                {
                    q.push(neighbor);
                }
            }
        }
    }

    if (newOrder.size() != operations.size())
    {
        throw runtime_error("Error: Circular dependency detected.");
    }
    cout << "newOrder size: " << newOrder.size() << endl;

    return newOrder;
}

void OperationsGraph::validateAndAddOperation(const Operation &op)
{
    int operationLabel = nextLabel++;
    Operation opWithLabel = op;
    opWithLabel.label = operationLabel;
    cout << "operations size from validateAndAddOperations: " << operations.size() << endl;
    cout << "operationLabel: " << operationLabel << endl;
    operations.push_back(opWithLabel);

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
                variableScopeMap[inputVar] = i;
                break;
            }
        }
        if (!found)
        {
            definedVarsStack.back().insert(inputVar);
            variableScopeMap[inputVar] = definedVarsStack.size() - 1;
        }
    }

    for (const string &inputVar : op.inputs)
    {
        bool isNewVar = variableScopeMap.find(inputVar) == variableScopeMap.end();

        if (!isNewVar)
        {
            for (const string &outputVar : op.outputs)
            {
                variableScopeMap[outputVar] = definedVarsStack.size() - 1;
                variableToOperationMap[outputVar] = operationLabel;

                for (const string &inputVar : op.inputs)
                {
                    if (variableToOperationMap.count(inputVar))
                    {
                        int dependentOpLabel = variableToOperationMap[inputVar];
                        adjList[dependentOpLabel].push_back(opWithLabel);
                        inDegree[operationLabel]++;
                    }
                }
            }
        }
    }

    for (const string &inputVar : op.inputs)
        processingVars.erase(remove(processingVars.begin(), processingVars.end(), inputVar), processingVars.end());
}

void OperationsGraph::processSubgraph(const vector<Operation> &loopOps)
{
    vector<Operation> shuffledOps = loopOps;
    shuffleOperations(shuffledOps);
    for (const Operation &op : shuffledOps)
    {
        validateAndAddOperation(op);
    }
}

void OperationsGraph::shuffleAndValidateLoop(vector<Operation> &loopOps)
{
    if (loopOps.size() < 2)
        return;

    Operation forBegin = loopOps.front();
    Operation forEnd = loopOps.back();

    queue<Operation> loopQueue;
    loopQueue.push(forBegin);

    vector<Operation> innerOps(loopOps.begin() + 1, loopOps.end() - 1);
    shuffleOperations(innerOps);

    for (Operation &op : innerOps)
    {
        loopQueue.push(op);
    }

    loopQueue.push(forEnd);

    while (!loopQueue.empty())
    {
        Operation op = loopQueue.front();
        loopQueue.pop();
        validateAndAddOperation(op);
    }

    processSubgraph(innerOps); // Process loop subgraph
}

Operation parseInput(const string &line, int label)
{
    // Skip the check for '=' for special operations like FOR_BEGIN or FOR_END
    if (line.find("FOR_BEGIN") != string::npos || line.find("FOR_END") != string::npos)
    {
        // Handle FOR_BEGIN and FOR_END without parsing the usual input format
        return Operation(label, {}, {}, line); // Just return the operation as is with no input/output
    }

    // Now handle normal operation with '='
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

    // MAX_RECURSION_DEPTH
    if (recursionDepth > 100000)
    {
        throw std::runtime_error("Error: Maximum recursion depth exceeded.");
    }

    string line;

    // Capture outer scope variables for reference
    unordered_set<string> outerScopeVars = definedVarsStack.empty() ? unordered_set<string>() : definedVarsStack.back();

    while (getline(inputStream, line))
    {
        if (line.empty())
            continue;

        // Handle FOR_BEGIN: Enter loop scope and start new subgraph
        if (line.find("FOR_BEGIN") != string::npos)
        {
            enterLoopScope();

            Operation forBeginOp(label++, {}, {}, "FOR_BEGIN");
            addOperation(forBeginOp, loopOps, insideLoop);

            // Process nested loop operations recursively
            vector<Operation> nestedLoopOps;
            processInputRecursively(inputStream, label, nestedLoopOps, true);

            shuffleAndValidateLoop(nestedLoopOps); // Shuffle and validate loop operations

            // Add the shuffled operations back to the loop operations
            for (Operation &op : nestedLoopOps)
                addOperation(op, loopOps, insideLoop);

            exitLoopScope(outerScopeVars); // Restore outer scope after the loop ends
            continue;
        }

        // Handle FOR_END: End the loop
        if (line.find("FOR_END") != string::npos)
        {
            // Add FOR_END and exit the loop
            Operation forEndOp(label++, {}, {}, "FOR_END");
            addOperation(forEndOp, loopOps, insideLoop);
            recursionDepth--;
            return; // Correctly exit
        }

        // Handle Regular Operation (not FOR_BEGIN or FOR_END)
        try
        {
            // Skip the parsing logic for FOR_BEGIN/END, as they are not regular operations
            if (line.find('=') != string::npos)
            {
                Operation op = parseInput(line, label);
                addOperation(op, loopOps, insideLoop);
            }
        }
        catch (const invalid_argument &e)
        {
            cout << "[ERROR] Invalid input format: " << e.what() << endl;
        }
    }

    // Final cleanup for scope at the end of the input
    if (!insideLoop)
    {
        exitLoopScope(outerScopeVars);
    }

    recursionDepth--;
}

void processInput(istream &input, OperationsGraph &graph)
{
    string line;
    while (getline(input, line))
    {
        // Debugging: Print the raw line that is being processed
        cout << "[DEBUG] Processing line: " << line << endl;

        if (line.empty())
        {
            cout << "[DEBUG] Skipping empty line." << endl;
            continue; // Skip empty lines
        }

        try
        {
            // Debugging: Attempt to parse the operation
            cout << "[DEBUG] Attempting to parse operation from line: " << line << endl;

            // Since you have a function 'parseInput', use it to parse the line
            int label = graph.getNextLabel(); // Assuming there's a function to get the next label.
            vector<Operation> loopOps;        // Assuming an empty vector for operations in the loop (if inside a loop).
            bool insideLoop = false;          // Set to true if processing inside a loop.

            // Parse the line to get the operation
            Operation op = parseInput(line, label);

            // Debugging: Print parsed operation details
            cout << "[DEBUG] Parsed operation: " << op.originalExpression << endl;
            cout << "[DEBUG] Operation label: " << op.label << endl;
            cout << "[DEBUG] Inputs: ";
            for (const auto &inputVar : op.inputs)
                cout << inputVar << " ";
            cout << endl;

            cout << "[DEBUG] Outputs: ";
            for (const auto &outputVar : op.outputs)
                cout << outputVar << " ";
            cout << endl;

            // Now, add the operation to the graph
            cout << "[DEBUG] Adding operation to the graph." << endl;
            graph.addOperation(op, loopOps, insideLoop);
        }
        catch (const exception &e)
        {
            // Debugging: Print the error message when parsing fails
            cout << "[ERROR] Error processing line: " << e.what() << endl;
            cout << "[DEBUG] Line that caused the error: " << line << endl;
        }
    }
}
