#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <queue>
#include <assert.h>
#include <limits>
#include <algorithm>
  
using namespace std;

// Check hw5_image for graphical representation of first test case (.in file).
/*
Modified AVL-tree.
If parent has two children, delete always looks for substitute in right-subtree.
If inserting value that is already in tree, dont do anything. (positive number on input)
If deleting value that is not in tree, dont do anything. (negative number on input)
L = number of nodes in left-subtree, R = number of nodes in right-subtree of node U.
If |L−R| > ⌊(max(L,R) + 1)/2⌋, reorganize sub-tree of U, to be perfectly balances Binary Tree.

Return depth of tree, number of nodes in Left-subtree, Right-subtree. (from root)
*/

//node declaration
typedef struct avl {
    int left_count;
    int right_count;
    int num;
    struct avl *l;
    struct avl *r;
} avl;
 
class avl_tree {
    private:
        avl *root = nullptr;
        avl **to_balance = nullptr;
        bool found = false;
    public:
        int depth(avl *);
        int log_2(int num);
        vector<int> sorted_array;
        avl* sortedArrayToBST(int start, int end);
        avl* minValueNode(avl* node);
        avl* deleteNode(avl** root, int key);
        void print_tree(avl* root, int space);
        void insert(avl **, const int);
        void inorder_to_array(avl *);
        void balance(avl **);
        void dealoc(avl *);
        void inorder_print(avl *);
        void init();
        bool check_balance(avl *node);
        bool is_present(avl *node, const int key);
        avl_tree() {};
};
 
int avl_tree::log_2(int num) {
    int power = 1;
    while (power < num) {
        power <<= 1;
    }
    return (power <= num) ? power:(power >> 1);
}


avl* avl_tree::sortedArrayToBST(int start, int end)  { 
    /* Base Case */
    if (start >= end) { // error, >= ?
        return nullptr; 
    }
    /* find root for left balanced tree */
    int M = log_2(end-start);
    int R = (end-start) - (M-1);
    int LT, RT;
    if (R <= (M/2)) {
        LT = ((M-2)/2) + R;
        RT = (M-2)/2;
    } else {
        LT = ((M-2)/2) + (M/2);
        RT = ((M-2)/2) + R - (M/2);
    }
    //printf("M = %d, R = %d\n", M, R);
    //printf("End = %d, start = %d\n", end, start);
    /* Get the middle element and make it root */
    assert(start+LT < end);
    ////printf("Mid = %d\n", mid);
    avl *root = (avl*) malloc(sizeof(avl));
    root->num = sorted_array[start+LT];
    root->right_count = RT;
    root->left_count = LT;
    assert(RT >= 0);
    assert(LT >= 0);
    //printf("Node %d, will have %d left, %d right\n", root->num, root->left_count, root->right_count);
    /* Recursively construct the left subtree and make it left child of root */
    if (LT == 1) {
        root->l = (avl*) malloc(sizeof(avl));
        root->l->num = sorted_array[start+LT-1];
        root->l->right_count = 0;
        root->l->left_count = 0;
        root->l->l = nullptr;
        root->l->r = nullptr;
        //printf("Node on left %d, will have %d left, %d right\n", root->l->num, root->l->left_count, root->l->right_count);
    } else {
        root->l = sortedArrayToBST(start, start+LT); 
    }
     
    /* Recursively construct the right subtree and make it right child of root */
    if (RT == 1) {
        root->r = (avl*) malloc(sizeof(avl));
        root->r->num = sorted_array[start+LT+1];
        root->r->right_count = 0;
        root->r->left_count = 0;
        root->r->r = nullptr;
        root->r->l = nullptr;
        //printf("Node on Right %d, will have %d left, %d right\n", root->r->num, root->r->left_count, root->r->right_count);
    } else {
        root->r = sortedArrayToBST(end-RT, end); 
    }
    return root; 
}
 
void avl_tree::print_tree(avl* root, int space) {
    if (root == nullptr) {
        return;
    }
    space += 4;
    print_tree(root->r, space);
    //printf("\n");
    //printf("%*d", space, root->num);
    print_tree(root->l, space);
}
 
int avl_tree::depth(avl* node) {  
    if (node == nullptr)   {
        return 0; 
    } else  { 
        /* compute the depth of each subtree */
        int lDepth = depth(node->l); 
        int rDepth = depth(node->r); 
 
        /* use the larger one */
        if (lDepth > rDepth) {
            return(lDepth+1);
        } else  {
            return(rDepth+1);
        }
    }
}
 
void avl_tree::insert(avl **r, const int v) {
    if (*r == nullptr) {
        *r = (avl*) malloc(sizeof(avl));
        (*r)->left_count = 0;
        (*r)->right_count = 0;
        (*r)->num = v;
        (*r)->l = nullptr;
        (*r)->r = nullptr;
        return;
    } else if (v < (*r)->num) { // insert to left
        (*r)->left_count++;
        //printf("%d Has now %d left, %d right\n", (*r)->num, (*r)->left_count, (*r)->right_count);
        if (!check_balance(*r)){
            to_balance = r;
        } 
        insert(&((*r)->l), v);
    } else if (v > (*r)->num) { // insert to right
        (*r)->right_count++;
        //printf("%d Has now %d left, %d right\n", (*r)->num, (*r)->left_count, (*r)->right_count);
        if (!check_balance(*r)){
            to_balance = r;
        } 
        insert(&((*r)->r), v);
    }
    return;
}
 
void avl_tree::inorder_print(avl *r) {
    if (r == nullptr) {
        return;
    }
    inorder_print(r->l);
    //printf("%d ", r->num);
    inorder_print(r->r);
}
 
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
// return false if tree is not balanced, else true
bool avl_tree::check_balance(avl *node) {
    if (!found) {
        assert(node != nullptr);
        int cond1 = abs(node->left_count - node->right_count);
        int cond2 = ((max(node->left_count, node->right_count) + 1) / 2 );
        //printf("Checking node %d\n", node->num);
        ////printf("%d left, %d right\n", node->left_count, node->right_count);
        if ( cond1 > cond2 ) {
            found = true;
            //printf("Unbalanced\n");
            return false;
        }
    }
    return true;
}
 
void avl_tree::balance(avl **node) {
    //printf("Balancing node %d\n", (*node)->num);
    //printf("Node has %d left, %d right\n", (*node)->left_count, (*node)->right_count);
    inorder_to_array(*node);
    dealoc(*node);
    //print();
    *node = sortedArrayToBST(0, sorted_array.size());
    assert(*node != nullptr);
    //printf("Inorder print of new root\n");
    inorder_print(*node);
    //printf("\n");
    //printf("New root of sub-tree = %d, %d left, %d right\n", (*node)->num, (*node)->left_count, (*node)->right_count);
    sorted_array.clear();
    return;
}
 
void avl_tree::inorder_to_array(avl *t) {
    if (t == nullptr) {
        return;
    }
    inorder_to_array(t->l);
    sorted_array.push_back(t->num);
    inorder_to_array(t->r);
}

avl* avl_tree::minValueNode(avl* node) {
    avl* current = node;
    /* loop down to find the leftmost leaf */
    while (current && current->l != nullptr) {
        current = current->l;
    }
    return current;
}
 
/*
    Returns true, if key is in tree, false otherwise.
*/
bool avl_tree::is_present(avl *r, const int key) {
    if (r == nullptr) {
        return false;
    }
    bool val = false;
    if (key < r->num) {
        val = is_present(r->l, key);
    } else if (key > r->num) {
        val = is_present(r->r, key);
    } else {
        assert(r->num == key);
        //printf("%d is in tree\n", key);
        val = true;
    }
    return val;
}
 
/* Given a binary search tree and a key, this function
deletes the key and returns the new root */
avl* avl_tree::deleteNode(avl** r, const int key) {
    // base case
    if (*r == nullptr) {
        //printf("");
        return (*r);
    }
    // If the key to be deleted is smaller than the root's key, then it lies in left subtree
    if (key < (*r)->num) {
        (*r)->left_count--;
        if (!check_balance(*r)) {
            to_balance = r;
        } 
        //printf("%d Has now %d left, %d right\n", (*r)->num, (*r)->left_count, (*r)->right_count);
        (*r)->l = deleteNode(&((*r)->l), key);
    } else if (key > (*r)->num) { // If the key to be deleted is greater than the root's key, then it lies in right subtree
        (*r)->right_count--;
        if (!check_balance(*r)) {
            to_balance = r;
        }
        //printf("%d Has now %d left, %d right\n", (*r)->num, (*r)->left_count, (*r)->right_count);
        (*r)->r = deleteNode(&((*r)->r), key);
    } else {// if key is same as root's key, then This is the node to be deleted
        // node with only one child or no child
        if ((*r)->l == nullptr) {
            avl* temp = (*r)->r;
            free(*r);
            *r = nullptr;
            return temp;
        } else if ((*r)->r == nullptr) {
            avl* temp = (*r)->l;
            free(*r);
            *r = nullptr;
            return temp;
        } // else
        // node with two children: Get the inorder successor
        // (smallest in the right subtree)
        avl* temp = minValueNode((*r)->r);
        // Copy the inorder successor's content to this node
        (*r)->num = temp->num;
        (*r)->right_count--;
        if (!check_balance(*r)) {
            to_balance = &(*r);
        }
        // Delete the inorder successor
        (*r)->r = deleteNode(&((*r)->r), temp->num);
    }
    return (*r);
}
 
void avl_tree::init() {
    int num_of_operations = 0;
    scanf("%d", &num_of_operations);
    //printf("Num of operations = %d\n", num_of_operations);
    int op = 0;
    for (int i = 0; i < num_of_operations; i++) {
        scanf("%d", &op);
        found = false;
        if (op > 0) {
            if (!is_present(root, op)) {
                //printf("Add %d\n", op);
                insert(&root, op);
                //print_tree(root, 0);
                //printf("\n");
            } else {
                //printf("%d already in tree\n", op);
            }
        } else {
            if (is_present(root, -op)) {
                //printf("Remove %d\n", -op);
                root = deleteNode(&root, -op);
                //print_tree(root, 0);
                //printf("\n");
            } else {
                //printf("%d is not in tree\n", -op);
            }
        }
 
        if (found) {
            balance(to_balance);
            to_balance = nullptr;
            //print_tree(root, 0);
            //printf("\n");
        }
    }
    if (found && to_balance != nullptr) {
        balance(to_balance);
        to_balance = nullptr;
    }
    //assert(root != nullptr); // root by mohl být nullptr, pokud se smaže celý strom
    if (root != nullptr) {
        //print_tree(root, 0);
        //printf("\n");
        printf("%d %d %d\n", depth(root)-1, root->left_count, root->right_count);
        dealoc(root);
    } else {
        printf("0 0 0\n");
    }
    return;
}
 
 
int main() {
    avl_tree tree;
    tree.init();
    return 0;
}