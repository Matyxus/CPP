#include "iddfs.h"

// Max size of vector.
const unsigned long long MAX_SIZE_COST = 25654117; 
// Visited Nodes.
std::vector<Node> visited(MAX_SIZE_COST); 
// Goals each thread found.
std::vector<std::shared_ptr<const state>> goals_cost(omp_get_max_threads(), nullptr);
// Current shortest path cost.
std::atomic<unsigned int> path_cost {UINT32_MAX};

// Insert into hash table, just with simple modulo.
bool inline insert_cost(const unsigned long long &value) {
    const unsigned long long key = value % MAX_SIZE_COST;
    if (visited[key].id == 0) {
        visited[key].id = value;
        return false;
    }
    return (visited[key].id == value);
}

// Remove inserted node.
void inline remove_cost(const unsigned long long &value) {
    const unsigned long long key = value % MAX_SIZE_COST;
    if (visited[key].id == value) {
        visited[key].id = 0;
    }
}
 

std::shared_ptr<const state> iddfs(std::shared_ptr<const state> root) {
    std::shared_ptr<const state> goal = nullptr;
    int max_depth = 1;
    bool end = false;
    visited[root->get_identifier() % MAX_SIZE_COST].id = root->get_identifier();
    while (true)
    {   
        //std::cout << "Exploring to depth: " << max_depth << std::endl;
        #pragma omp parallel
        {
            #pragma omp single
            {
                if (!end) {
                    find_first(root, max_depth);
                } else {
                    find_best(root);
                }
            }
            // Goal found by this thread.
            auto const &tmp_goal = goals_cost[omp_get_thread_num()];
            if (tmp_goal != nullptr) {
                // Change it for best goal, if it is better. (lower identifier, shorter path)
                #pragma omp critical
                {
                    if (goal == nullptr) {
                        goal = tmp_goal;
                    } else {
                        if (goal->current_cost() > tmp_goal->current_cost()) {
                            goal = tmp_goal;
                        } else if (goal->current_cost() == tmp_goal->current_cost() && 
                                goal->get_identifier() > tmp_goal->get_identifier()) {
                            goal = tmp_goal;
                        } 
                    }
                }
            }
        }
        if (end) {
            break;
        }
        if (goal != nullptr) {
            //std::cout << "Find first solution" << std::endl;
            //std::cout << "Path cost: " << path_cost << std::endl;
            end = true;
        }
        max_depth++;
        visited[root->get_identifier() % MAX_SIZE_COST].id = root->get_identifier();
    }
    return goal;
}

void find_best(const std::shared_ptr<const state> &node) {
    if (node->is_goal()) {
        auto &tmp = goals_cost[omp_get_thread_num()];
        if (tmp == nullptr) {
            tmp = node;
        } else if (node->current_cost() < path_cost) {
            tmp = node;
            path_cost = node->current_cost(); 
        } else if (node->current_cost() == path_cost) {
            if (node->get_identifier() < tmp->get_identifier()) {
                tmp = node;
            }
        }
    }
    for (auto const &neighbour : node->next_states()) {
        if (!(insert_cost(neighbour->get_identifier())) && neighbour->current_cost() <= path_cost) {
            #pragma omp task
            find_best(neighbour);
        }
    }
    remove_cost(node->get_identifier());
}
 
void find_first(const std::shared_ptr<const state> &node, const int &depth) {
    if (depth == 1) { // Find goals
        auto &tmp = goals_cost[omp_get_thread_num()];
        for (auto const &neighbour : node->next_states()) {
            if (neighbour->is_goal() && (tmp == nullptr || tmp->get_identifier() > neighbour->get_identifier() || 
                    neighbour->current_cost() < tmp->current_cost() )) {
                tmp = neighbour;
                path_cost = std::min(neighbour->current_cost(), path_cost.load());
            }
        }
    } else { // Explore more
        for (auto const &neighbour : node->next_states()) {
            if (!(insert_cost(neighbour->get_identifier())) && neighbour->current_cost() < path_cost) {
                #pragma omp task
                find_first(neighbour, depth-1);
            }
        }
    }
    remove_cost(node->get_identifier());
}