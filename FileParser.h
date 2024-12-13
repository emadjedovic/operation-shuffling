#ifndef FILEPARSER_H_INCLUDED
#define FILEPARSER_H_INCLUDED

#include <vector>
#include <string>
#include "Operation.h"

class FileParser {
public:
    // Function to parse operations from a file
    static std::vector<Operation> parseOperations(const std::string& filename);
};

#endif // FILEPARSER_H_INCLUDED
