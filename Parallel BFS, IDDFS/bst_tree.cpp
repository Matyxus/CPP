#include "bst_tree.h"


void bst_tree::cleanup(bst_tree::node *n) {
    if(n != nullptr) {
        cleanup(n->left);
        cleanup(n->right);
        delete n;
    }
}

bool bst_tree::insert(unsigned long long data) {
    node * new_node = new node(data);
    node* tmp = root;
    while (true) {
        if ( tmp->data < data ) { // Going to right tree.
            if( tmp->right != nullptr ) {
                tmp = tmp->right; // Move pointer down one node.
            } else if(tmp->right.compare_exchange_strong(nullnode, new_node)) {
                return false; // Insert new node, since pointer is null.
            }
        }
        else if (tmp->data > data ) { // Going to left tree.
            if( tmp->left != nullptr ) {
                tmp = tmp->left; // Move pointer down one node.
            } else if (tmp->left.compare_exchange_strong(nullnode, new_node)) {
                return false; // Insert new node, since pointer is null.
            }
        } else { // Node present -> tmp->data == data.
            break;
        }
    }
    return true;
}