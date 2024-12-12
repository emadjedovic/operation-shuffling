#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <vector>
#include <string>
#include "Operation.h"

class FileParser {
public:
    // Function to parse operations from a file
    static std::vector<Operation> parseOperations(const std::string& filename);
};

#endif // FILEPARSER_H
