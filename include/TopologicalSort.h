#ifndef TOPOLOGICALSORT_H
#define TOPOLOGICALSORT_H

#include <vector>
#include "Operation.h"
#include "DependencyGraph.h"

class TopologicalSort {
public:
    // Sort operations in a topologically valid order
    std::vector<Operation> sort(const DependencyGraph& graph);
};

#endif // TOPOLOGICALSORT_H
