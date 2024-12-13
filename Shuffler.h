#ifndef SHUFFLER_H_INCLUDED
#define SHUFFLER_H_INCLUDED

#include <vector>
#include "Operation.h"

class Shuffler {
public:
    static std::vector<Operation> shuffle(const std::vector<Operation>& operations);
};

#endif // SHUFFLER_H_INCLUDED
