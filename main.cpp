#include <fstream>
#include "OperationsGraph.cpp"
#include <direct.h> // _getcwd

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

    // dva moguca nacina za input programa za obfuskaciju
    cout << "1 FOR CONSOLE / 2 FOR FILE INPUT: ";
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
        cout << "Enter the filename (with .txt): ";
        getline(cin, filename);

        string currentPath = getCurrentWorkingDirectory();
        // izaci iz output foldera i uci u test-files
        string filePath = currentPath.substr(0, currentPath.find_last_of("\\/")) + "/test-files/" + filename;

        ifstream file(filePath);
        if (!file.is_open())
        {
            cout << "Error opening the file: " << filePath << endl;
            return 1; // error
        }

        cout << "\nReading file contents from: " << filePath << endl;
        processInput(file, graph);
        file.close();
    }
    else
    {
        cout << "Invalid choice. Exiting." << endl;
        return 1; // error
    }

    try
    {
        graph.obfuscate();
    }
    catch (const runtime_error &e)
    {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}
