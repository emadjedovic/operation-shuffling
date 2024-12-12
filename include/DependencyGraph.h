#ifndef DEPENDENCYGRAPH_H
#define DEPENDENCYGRAPH_H

#include <vector>
#include "Operation.h"

class DependencyGraph {
public:
    void addOperation(const Operation& operation);
    void addDependencies();
    
    // Other necessary methods for building graph, checking dependencies
private:
    std::vector<Operation> operations;
    // A data structure to store dependencies can be added
};

#endif // DEPENDENCYGRAPH_H
