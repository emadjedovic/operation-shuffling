#ifndef SHUFFLER_H
#define SHUFFLER_H

#include <vector>
#include "Operation.h"

class Shuffler {
public:
    static std::vector<Operation> shuffle(const std::vector<Operation>& operations);
};

#endif // SHUFFLER_H
