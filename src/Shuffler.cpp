#include "Shuffler.h"
#include <algorithm>
#include <random>

std::vector<Operation> Shuffler::shuffle(const std::vector<Operation>& operations) {
    std::vector<Operation> shuffledOps = operations;
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(shuffledOps.begin(), shuffledOps.end(), g);
    return shuffledOps;
}
