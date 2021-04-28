#include "LinkedQueue.h"

LinkedQueue::~LinkedQueue() {
    node *temp = head;
    //std::cout << "Trying to delete Queue" << std::endl;
    int count = 0;
    while (temp!= nullptr) {   
        delete temp;
        head = head->next;
        temp = head;
        count++;
    }
    //std::cout << "Deleted " << count << " elements." << std::endl;
    head = nullptr;
}
// Checks how many nodes are linked together.
void LinkedQueue::check_link() {
    node *temp = head;
    int count = 0;
    while (temp != nullptr) {
        count++;
        temp = temp->next;
    }
    //std::cout << "Read " << count << " elements." << std::endl;
}
// Standard push into linked list.
void LinkedQueue::push(std::shared_ptr<const state> data) {
    node * new_node = new node(data);
    size++;
    if (head == nullptr) {
        tail = new_node;
        head = new_node;
    } else {
        tail->next = new_node;
        tail = new_node;
    }
}
