#pragma once
#include <atomic>
class bst_tree { // Binary search tree.
public:
    class node {
    public:
        std::atomic<node *> left { nullptr };  // Pointer to left sub-tree.
        std::atomic<node *> right { nullptr }; // Pointer to right sub-tree.
        unsigned long long data; // Stored data.
        node(unsigned long long data) : data(data) {}
    };

    // Root of tree, has to be assigned before inserting.
    std::atomic<node *> root { nullptr };
    node * nullnode = nullptr; // Needed, since nullptr is not valid in compare_exchange_strong.
 
    // Destructor, which frees allocated memory.
    ~bst_tree() { cleanup(root);}
    // Free pointers.
    void cleanup(bst_tree::node *n);
    // Parallel insert, returns true if data is already present, else false.
    bool insert(unsigned long long data); 
};

