#pragma once

#include "../state.h"
#include <omp.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <atomic>
// Finds shortest path in weighted graph.
std::shared_ptr<const state> iddfs(std::shared_ptr<const state> root);

// After goal is found, searches for best goal with shortest path.
void find_best(const std::shared_ptr<const state> &node);

// Finds goal at lowest depth using standard iddfs.
void find_first(const std::shared_ptr<const state> &node, const int &depth);

// Wrapper around std::atomic.
class Node {
public:
    std::atomic<unsigned long long> id {0};
    Node() {};
    ~Node() {};
};


