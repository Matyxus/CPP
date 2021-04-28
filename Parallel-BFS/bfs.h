#pragma once

#include "../state.h"
#include "bst_tree.h"
#include "LinkedQueue.h"
#include <omp.h>
#include <iostream>
#include <vector>
#include <algorithm>

/*
    Each thread pushes to its own LinkedList, after that, all linked lists are linked
    together, LinkedLists are swaped. Threads synchronize to their own respective start,
    they start deleting nodes from linked lists, pushing neighbours to now empty LinkedLists
    ( since they were swaped ), and so they continue until they find solution. 
*/
std::shared_ptr<const state> bfs(std::shared_ptr<const state> root);
void parBfsThread(const int start, const int end, LinkedQueue::node *begin, LinkedQueue& Q_out);

