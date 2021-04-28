#include "bfs.h"



bst_tree tree; // Visited nodes.
std::shared_ptr<const state> sol = nullptr; // Goal.
int found_sol = 0; // Found any goal.
const int thread_count = omp_get_max_threads(); // Maximal amount of threads.


std::shared_ptr<const state> bfs(std::shared_ptr<const state> root) {
    std::vector<LinkedQueue> *Q_in  = new std::vector<LinkedQueue>(thread_count); // In queue.
    std::vector<LinkedQueue> *Q_out = new std::vector<LinkedQueue>(thread_count); // Out queue.
    tree.root = new bst_tree::node(root->get_identifier()); // Assign root.
    auto const &neighbours = root->next_states();
    const int neigh_size = neighbours.size();
    //std::cout << "Root has " << neigh_size << " neighbours." << std::endl;
    #pragma omp parallel for
    for (int i = 0; i < neigh_size; i++) {
        if (!tree.insert(neighbours[i]->get_identifier())) {
            (*Q_out)[omp_get_thread_num()].push(neighbours[i]);
        }
    }
    //std::cout << "Linking Queues." << std::endl;
    LinkedQueue::node *curr = nullptr;
    LinkedQueue::node *start = nullptr;
    // Link queues, empty ones have to be skipped.
    for (int i = 0; i < thread_count; i++) {
        if (curr == nullptr && (*Q_out)[i].size != 0) {
            start = (*Q_out)[i].head;
            curr = (*Q_out)[i].tail;
        } else if (curr != nullptr && (*Q_out)[i].size != 0) {
            curr->next = (*Q_out)[i].head;
            curr = (*Q_out)[i].tail;
        }
    }
    int size = neigh_size;
    int temp_size = 0;
    int layer = 1;
    while(sol == nullptr) {
        //std::cout << "Starting parallel section with size: " << size <<std::endl;
        #pragma omp parallel reduction(+ : temp_size)
        {
            const int thread_id = omp_get_thread_num();
            const int chunk_size = 1+(size/thread_count);
            const int start_at = (thread_id*chunk_size);
            const int end_at = std::min(chunk_size*(thread_id+1), size);
            /*
            #pragma omp critical
            {
                std::cout << "Thread " << thread_id << " starting at " << start_at << " ending at " << end_at << std::endl; 
            }
            */
            parBfsThread(start_at, end_at, start, (*Q_in)[thread_id]);
            temp_size += (*Q_in)[thread_id].size;
            #pragma omp barrier
            (*Q_out)[thread_id].head = nullptr; // reset pointers
            (*Q_out)[thread_id].tail = nullptr; // reset pointers
            (*Q_out)[thread_id].size = 0; // reset size
        }
        // ----- Info -----
        /*
        std::cout << "Total size: " << temp_size << std::endl;
        for (int i = 0; i < thread_count; i++)
        {
            std::cout << "Size of Queue: " << i << " is " << (*Q_in)[i].size << std::endl;
        }
        */
        size = temp_size;
        temp_size = 0;
        // ----- End of info -----
        if (sol != nullptr) {
            //std::cout << "Solution found" << std::endl;
            break;
        }
        //std::cout << "Linking Queues." << std::endl;
        // Link queues, empty ones have to be skipped.
        curr = nullptr;
        for (int i = 0; i < thread_count; i++) {
            if (curr == nullptr && (*Q_in)[i].size != 0) {
                start = (*Q_in)[i].head;
                curr = (*Q_in)[i].tail;
            } else if (curr != nullptr && (*Q_in)[i].size != 0) {
                curr->next = (*Q_in)[i].head;
                curr = (*Q_in)[i].tail;
            }
        }
        //std::cout << "Swaping queues." << std::endl;
        // Swap queues.
        auto temp = Q_in;
        Q_in = Q_out;
        Q_out = temp;
        layer++;
    }
    //std::cout << "Freeing queus, tree." << std::endl;    
    delete Q_in;
    delete Q_out;
    //std::cout << "Found sol at layer: " << layer << std::endl;
    //std::cout << "Freed" << std::endl;
    return sol;
}

void parBfsThread(const int start, const int end, LinkedQueue::node *begin, LinkedQueue& Q_out) {
    // Set pointer to correct starting position.
    if (start < end) {
        for (int i = 0; i < start; i++) {
            begin = begin->next;
        }
    }
    // Wait for all threads to start at correct position.
    #pragma omp barrier
    // Solution of each thread in given layer.
    std::shared_ptr<const state> sol_temp = nullptr;
    if (start < end) {
        for (int i = start; i < end; i++) {
            auto vert = begin; // Current pointer.
            // Solution.
            if (vert->data->is_goal()) {
                // If there is no solution, or if its not optimal.
                if (sol_temp == nullptr || sol_temp->get_identifier() > vert->data->get_identifier()) {
                    sol_temp = vert->data;
                }
                // Let each thread assign only once to atomic found_sol true.
                if (found_sol == 0) {
                    #pragma omp atomic
                    found_sol++;
                }
            }
            // Neighbours, only if solution wasnt already found.
            if (found_sol == 0) {
                for (auto const &neighbour : vert->data->next_states()) {
                    if (!tree.insert(neighbour->get_identifier())) { // Visited.
                        Q_out.push(neighbour);
                    }
                }
            }
            // Move begin pointer.
            begin = begin->next;
            // Free current pointer.
            delete vert;
        }
    }
    // Compare with global solution.
    if (sol_temp != nullptr) { 
        // If there is no global solution, or if its not optimal.
        #pragma omp critical
        {
            if (sol == nullptr || sol->get_identifier() > sol_temp->get_identifier()) {
                sol = sol_temp;
            }
        }
    }
    return;
}







