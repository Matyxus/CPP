#pragma once
#include "state.h"


class LinkedQueue { 
public:
    class node {
    public:
        node *next = nullptr; // Pointer to next node.
        std::shared_ptr<const state> data; // Saved data.
        node(std::shared_ptr<const state> data) : data(data) {}
    };
    int size = 0; // Current size

    node *head = nullptr; // Pointer to start of linked list
    node *tail = nullptr; // Pointer to end of linked list

    LinkedQueue() {};
    ~LinkedQueue();
    // Checks how many nodes are linked together.
    void check_link();
    // Standard push into linked list.
    void push(std::shared_ptr<const state> data);
};




