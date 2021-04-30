#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <map>
// Hledání optimální trojnožky v binárním černo-bílém stromu
// Search for optimal tripode in red and black trees.

// g++ -std=c++11 -pipe -Wall -O3 -c main.cpp
// g++ -std=c++11 main.o -o main
// code is mainly from homework 5
using namespace std;
 
//node declaration
typedef struct avl {
    int num;
    int best_top;
    int best_left;
    int best_right;
    bool black;
    struct avl *l;
    struct avl *r;
    struct avl *parent;
} avl;
// class
class avl_tree {
    private:
        avl *root = nullptr;
        bool black = false;
        int N = 0; // number of inputs
        vector<avl*> array; // third degree nodes
    public:
        void print_tree(avl* root, int space);
        void insert(avl **, avl* parent, const int);
        void dealoc(avl *);
        void inorder_print(avl *);
        void init();
        void find_degree(avl* node);
        void top(avl* node);
        int best_left_right(avl* node);
        avl_tree() {};
        ~avl_tree() { dealoc(root);};
};
/*
    Prints tree, not recomanded for trees over 100 nodes.
*/
void avl_tree::print_tree(avl* root, int space) {
    if (root == nullptr) {
        return;
    }
    space += 4;
    print_tree(root->r, space);
    printf("\n");
    printf("%*d", space, root->num);
    print_tree(root->l, space);
}
/*
    Insert into binary tree.
*/
void avl_tree::insert(avl **r, avl* parent, const int v) {
    if (*r == nullptr) {
        *r = (avl*) malloc(sizeof(avl));
        (*r)->best_top = 0;
        (*r)->best_left = 0;
        (*r)->best_right = 0;
        (*r)->num = v;
        (*r)->l = nullptr;
        (*r)->r = nullptr;
        (*r)->parent = parent;
        if (black) {
            (*r)->black = true;
        } else {
            (*r)->black = false;
        }
        return;
    } else if (v < (*r)->num) { // insert to left
        //printf("%d Has now %d left, %d right\n", (*r)->num, (*r)->left_count, (*r)->right_count);
        insert(&((*r)->l), *r, v);
    } else if (v > (*r)->num) { // insert to right
        //printf("%d Has now %d left, %d right\n", (*r)->num, (*r)->left_count, (*r)->right_count);
        insert(&((*r)->r), *r, v);
    }
    return;
}
/*
    Inorder print of binary tree.
*/
void avl_tree::inorder_print(avl *r) {
    if (r == nullptr) {
        return;
    }
    inorder_print(r->l);
    printf("%d ", r->num);
    inorder_print(r->r);
}
/*
    Finds third degree nodes, starts from left of root, and right of root
*/
void avl_tree::find_degree(avl* node) {
    if (node->l != nullptr && node->r != nullptr) {
        //printf("%d is 3degree\n", node->num);
        array.push_back(node);
        find_degree(node->l);
        find_degree(node->r);
    } else if (node->l != nullptr) {
        find_degree(node->l);
    } else if (node->r != nullptr) {
        find_degree(node->r);
    }
    return;
}
/*
    Free memory.
*/
void avl_tree::dealoc(avl* r) {
    if (r == nullptr) {
        return;
    }
    dealoc(r->l);
    dealoc(r->r);
    assert(r != nullptr);
    free(r);
    r = nullptr;
}
/*
    "Main".
*/
void avl_tree::init() {
    int temp = 0;
    scanf("%d", &N);
    // load input
    for (int i = 0; i < N; i++) {
        scanf("%d", &temp);
        black = false;
        if (temp > 0) {
            black = true;
        }
        insert(&root, nullptr, abs(temp));
    }
    // find 3rd degrees
    find_degree(root->r);
    find_degree(root->l);
    // left, right
    best_left_right(root);
    // top path
    top(root);
    int best = 0;
    for (const auto &node: array) { // itarate over third degree nodes and find the best one
        best = max(node->best_left+node->best_right+node->best_top+(node->black==true), best);
    }
    printf("%d\n", best);
}
/*
    Finds best path from top, if going to left child, compare with current top and right,
    if going to right child, compare with current top and right paths.
*/
void avl_tree::top(avl* node) {
    if (node == nullptr) {
        return;
    }
    // left
    if (node->l != nullptr) {
        node->l->best_top = max(node->best_top + (node->black == true), node->best_right + (node->black == true));
        top(node->l);
    }
    // right
    if (node->r != nullptr) {
        node->r->best_top = max(node->best_top + (node->black == true), node->best_left + (node->black == true));
        top(node->r);
    }
}
/*
    Finds best right and left path (similar to depth function).
*/  
int avl_tree::best_left_right(avl* node) {  
    if (node == nullptr) {
        return 0; 
    } else  { 
        /* compute the depth of each subtree */
        node->best_left = best_left_right(node->l); 
        node->best_right = best_left_right(node->r); 
 
        /* use the larger one */
        if (node->best_left > node->best_right) {
            return(node->best_left+(node->black == true));
        } else  {
            return(node->best_right+(node->black == true));
        }
    }
}


int main(int argc, char const *argv[]) {
    avl_tree tree;
    tree.init();
    return 0;
}


