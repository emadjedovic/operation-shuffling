#include <fstream>
#include "OperationsGraph.cpp"
#include <direct.h> // for _getcwd

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

int main()
{
    OperationsGraph graph;

    cout << "Enter '1' for console input or '2' for file input: ";
    int choice;
    cin >> choice;
    cin.ignore();

    if (choice == 1)
    {
        cout << "Enter operations (empty line to finish):\n";
        processInput(cin, graph);
    }
    else if (choice == 2)
    {
        string filename;
        cout << "Enter the filename (e.g., test1.txt): ";
        getline(cin, filename);

        // Dynamically determine the file's absolute path
        string currentPath = getCurrentWorkingDirectory();
        string filePath = currentPath.substr(0, currentPath.find_last_of("\\/")) + "/test-files/" + filename;

        cout << "Current working directory: " << currentPath << endl;
        cout << "Attempting to open file: " << filePath << endl;

        ifstream file(filePath);
        if (!file.is_open())
        {
            cout << "Error: Could not open file at " << filePath << endl;
            return 1;
        }

        cout << "Reading operations from file: " << filePath << endl;
        processInput(file, graph);
        file.close();
    }
    else
    {
        cout << "Invalid choice. Exiting." << endl;
        return 1;
    }

    try
    {
        vector<string> sortedOps = graph.topSortWithShuffle();
        cout << "\nOBFUSCATED:\n";
        for (const auto &op : sortedOps)
            cout << op << endl;
    }
    catch (const runtime_error &e)
    {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}
