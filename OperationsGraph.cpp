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
    string operationString;

    Operation(int l, vector<string> in, vector<string> out, const string &expr) : label(l), inputs(in), outputs(out), operationString(expr) {}
};

class OperationsGraph
{
private:
    vector<Operation> operations;
    vector<int> inDegree;
    vector<vector<int>> adjList;                   // cuvamo samo indekse operacija da ne bismo bezveze zauzimali memoriju sa cijelom Operation strukturom
    unordered_map<string, int> variableLastOrigin; // da utvrdimo veze (zavisnosti) jedne operacije od neke prethodne

    void shuffleZeroIndegreeOps(vector<int> &ops);

public:
    void addOperation(const Operation &op);
    vector<string> topSortWithShuffle();
    void obfuscate();
};

// IMPLEMENTATIONS

void OperationsGraph::shuffleZeroIndegreeOps(vector<int> &ops)
{
    random_device rd;
    mt19937 g(rd()); // umjesto nasumicnog rd() mozemo ubaciti seed recimo g(41)
    shuffle(ops.begin(), ops.end(), g);
}

void OperationsGraph::addOperation(const Operation &op)
{
    operations.push_back(op);
    inDegree.push_back(0);
    adjList.push_back(vector<int>());

    // zabiljeziti nove zavisnosti za dodanu operaciju
    for (const string &input : op.inputs)
    {
        if (variableLastOrigin.find(input) != variableLastOrigin.end())
        {
            // input za ovu operaciju je negdje ranije output za neku prethodnu operaciju
            int variableOriginOp = variableLastOrigin[input];

            // dodajemo granu iz operacije koja je modifikovala ovu varijablu koja je input za trenutnu operaciju
            // origin operacija mora doci prije trenutne operacije
            adjList[variableOriginOp].push_back(op.label);
            // povecavamo ulazni indeks trenutne operacije
            inDegree[op.label]++;
        }
    }

    // prolazimo kroz output varijabl
    for (const auto &output : op.outputs)
    {
        // proci kroz prethodne operacije i za svaku provjeriti input varijable
        // ukoliko se neka prethodna input varijable poklapa s ovom output onda
        // dodati granu iz te prethodne operacije u trenutnu

        for (int prevOp = 0; prevOp < operations.size(); ++prevOp)
        {
            if (prevOp == op.label)
                continue; // ignorisemo trenutnu operaciju

            cout << "Output: " << output << " has dependencies on: "<<endl;
            const Operation &prevOpObj = operations[prevOp];
            for (const string &input : prevOpObj.inputs)
            {
                // ako se ulazna varijabla prethodne operacije poklapa sa trenutnom izlaznom varijablom
                if (input == output)
                {

                    cout << "Op " << prevOp << " ";
                    // dodajemo zavisnost: prethodna operacija mora biti izvrsena prije trenutne
                    adjList[prevOp].push_back(op.label);
                    inDegree[op.label]++; // Povećavamo in-degree trenutne operacije

                    // biljezimo trenutnu operaciju kao onu koja je zadnja utjecala na varijable
                    variableLastOrigin[output] = op.label;
                }
            }
        }
    }
    cout << endl;

    // biljezimo trenutnu operaciju kao onu koja je zadnja utjecala na varijable
    // variableLastOrigin[output] = op.label;
}

// modifikacija topoloskog sortiranja
vector<string> OperationsGraph::topSortWithShuffle()
{
    vector<string> newOrder;
    queue<int> q;

    // stavljamo u red operacije koje ne ovisi od prethodnih
    // ove operacije cemo razmijesati
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

        // kljucni dio
        shuffleZeroIndegreeOps(currentZeroIndegreeOps);

        // nakon sto smo operacije izmijesali, dodajemo ih u rezultat newOrder
        // operationString je originalni format operacije koji smo upisali u konzolu
        for (const int &current : currentZeroIndegreeOps)
        {
            newOrder.push_back(operations[current].operationString);
            // kada smo jednom dodali operacije vise ih ne diramo
            // smanjujemo ulazni stepen svih iducih operacija koje zavise od ovih, tako da ako njihov indegree postane u tom slucaju 0 onda njih mozemo izmijesati i dodati kao narednu turu u rezultat newOrder

            for (const int &neighbor : adjList[current])
            {
                inDegree[neighbor]--;
                if (inDegree[neighbor] == 0)
                    q.push(neighbor);
            }
        }

        // ponavljamo sve dok ne nestane operacija ulaznog stepena 0
    }

    // ako nismo obradili sve operacije onda imamo beskonacnu petlju u programu
    if (newOrder.size() != operations.size())
    {
        cout << "Error: Circular dependency detected." << endl;
        exit(1); // prekidamo program
    }

    return newOrder;
}

// izvlacimo podatke iz operacije koja je unesena, dodajemo joj id "label"
Operation parseInput(const string &line, int label)
{
    stringstream ss(line);
    string outputs, inputs; // stringovi iz kojih izvlacimo varijable
    size_t equalSign = line.find('=');

    // dosli do kraja stringa bez naznake znaka "="
    // potrebno za identifikaciju izlaznih varijabli
    if (equalSign == string::npos)
    {
        cout << "Error: Invalid input format. Missing '='." << endl;
        exit(1);
    }

    outputs = line.substr(0, equalSign); // do znaka jednakosti
    inputs = line.substr(equalSign + 1); // ovo treba dodatno preraditi uzimajuci u obzir zagrade ()

    // Trim spaces
    outputs.erase(remove(outputs.begin(), outputs.end(), ' '), outputs.end());
    inputs.erase(remove(inputs.begin(), inputs.end(), ' '), inputs.end());

    vector<string> outputVars;      // vektor izlaznih varijabli
    stringstream outputSS(outputs); // outputs nije vise string
    string output;                  // varijabla
    // outputs are comma-separated
    while (getline(outputSS, output, ','))
        outputVars.push_back(output);

    size_t start = inputs.find('(');
    size_t end = inputs.find(')');

    // nismo pronasli zagrade (ovo je nuzno jer operacije tu dobijaju parametre nad kojim djeluju)
    if (start == string::npos || end == string::npos || start >= end)
    {
        cout << "Error: Invalid input format. Missing parentheses." << endl;
        exit(1);
    }

    // slicna procedura kao za izlazne varijable
    string inputVars = inputs.substr(start + 1, end - start - 1);
    vector<string> inputVarsList; // vektor ulaznih varijabli
    stringstream inputSS(inputVars);
    string input; // varijabla
    while (getline(inputSS, input, ','))
        inputVarsList.push_back(input);

    // "izvucena" operacija sa svim neophodnih podacima
    // s ovim formatom mozemo raditi
    return Operation(label, inputVarsList, outputVars, line);
}

// iz jedne linije (konzola) ili liniju po liniju iz jednog fajla
// modificiramo graf kako bismo
void processInput(istream &inputStream, OperationsGraph &graph)
{
    string line;
    int label = 0; // prva operacija

    while (getline(inputStream, line))
    {
        if (line.empty())
            break;
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

// konacna funkcija
void OperationsGraph::obfuscate()
{
    vector<string> result = topSortWithShuffle();

    cout << "\nNEW SAME CODE (OBFUSCATED):\n\n";
    for (const auto &op : result)
        cout << op << endl;
    cout << endl;
}
