#include <iostream>
#include <vector>
#include <queue>
#include <assert.h>

#define ROUTER 0
#define VALIDATOR 1
#define EXTRACTOR 2

// g++ -std=c++11 -pipe -Wall -O3 -c graph.cpp
// g++ -std=c++11 graph.o -o main

typedef struct node {
    int id;
    int type = ROUTER;
    std::vector<int> connections;
    node* left = nullptr;
    node* right = nullptr;
    node* parent = nullptr;
    node (int index) {
        id = index;
    }
} node;

class graph {
    private:
        int search = 0;
        int pairs = 0;
        int P = 0; // number of computers
        int S = 0; // number of pairs connected by cable
        int E = 0; // number of extractors
        int V = 0; // number of validators
        node* root = NULL;
        std::vector<node*> nodes;
        std::vector<std::pair<int, int>> to_remove;
    public:
        graph() {}
        ~graph() {
            for (unsigned int i = 0; i < nodes.size(); i++) {
                delete nodes[i];
            }
        }
        inline void getInt(int &n) {
            n = 0; 
            int ch = getchar(); 
            // if the input character is not a digit 
            while (ch < '0' || ch > '9') { 
                ch = getchar(); 
            } 
            
            while (ch >= '0' && ch <= '9') { 
                n = (n << 3) + (n << 1) + ch - '0';  //multiply by 10 
                ch = getchar(); 
            } 
        }
        void init() {
            scanf("%d %d %d %d", &P, &S, &E, &V);
            for (int i = 0; i < P; i++) {
                nodes.push_back(new node(i));
            }
            int j, k;
            for (int i = 0; i < S; i++) {
                getInt(j);
                getInt(k);
                nodes[j-1]->connections.push_back(k-1);
                nodes[k-1]->connections.push_back(j-1);
            }
            for (int i = 0; i < E; i++) {
                getInt(j);
                nodes[j-1]->type = EXTRACTOR;
            }
            for (int i = 0; i < V; i++) {
                getInt(j);
                nodes[j-1]->type = VALIDATOR;
            }
            // find first router, that has 2 connections, guaranteed to be with routers
            int i = 0;
            for (; i < P; i++) {
                if (nodes[i]->connections.size() == 2) {
                    break;
                } 
            }

            root = nodes[i];
            root->left = nodes[root->connections[0]];
            root->right = nodes[root->connections[1]];
            NodeImpl(nodes[root->connections[0]], root);
            NodeImpl(nodes[root->connections[1]], root);
            for (auto const &pair : to_remove) {
                if (nodes[pair.first]->left->id == pair.second) {
                    nodes[pair.first]->left = nodes[pair.first]->right;
                    nodes[pair.first]->right = nullptr;
                } else {
                    nodes[pair.first]->right = nullptr;
                }
            }
            //printer(root);
            explore(root);
            printf("%d\n", pairs);
        }
        void printer(node* ptr) {
            if( ptr == nullptr ) { 
                return;
            }
            std::queue<node*> q; // init
            q.push(ptr); // root into queue
            while( !q.empty() ) {
                ptr = q.front();
                q.pop();
                printf("%d\n" , ptr->id+1); // process node
                if( ptr->left != nullptr ) {
                    q.push( ptr->left );
                }
                if( ptr->right != nullptr ) {
                    q.push( ptr->right );
                }
            }
        }
        void NodeImpl(node* ptr, node* parent) {
            ptr->parent = parent;
            if (ptr->connections.size() == 1) {
                ptr->left = nullptr;
                ptr->right = nullptr;
                if (ptr->type == ROUTER) {
                    to_remove.push_back({parent->id, ptr->id});
                }
                return;
            } else {
                int index_left;
                int index_right; 
                if (ptr->connections[0] != parent->id) {
                    index_left = ptr->connections[0];
                } else {
                    index_left = ptr->connections[1];
                }
                ptr->left = nodes[index_left];
                assert(ptr->left != nullptr);
                NodeImpl(nodes[index_left], ptr);
                if (ptr->connections.size() == 2) {
                    ptr->right = nullptr;
                } else {
                    if (ptr->connections[2] != parent->id) {
                        index_right = ptr->connections[2];
                    } else {
                        index_right = ptr->connections[1];
                    }
                    ptr->right = nodes[index_right];
                    assert(ptr->right != nullptr);
                    NodeImpl(nodes[index_right], ptr);
                }
            }
        }
        int explore(node* ptr) {
            // edning node, Extracor or Validator, return its type
            if (ptr->connections.size() == 1) {
                return ptr->type;
            }
            int type1 = 0, type2 = 0;
            int ret_val = 0;
            if (ptr->left != nullptr) {
                type1 = explore(ptr->left);
            }
            if (ptr->right != nullptr) {
                type2 = explore(ptr->right);
            }
            // Compare types
            if (type1+type2 == EXTRACTOR+VALIDATOR) { // Extractor == 1, Validator == 2 -> 3
                pairs++;
                ret_val = 0;
            } else { // return found type
                ret_val = (type1 == 0) ? type2:type1;
            }
            return ret_val;
        }
};


int main() {
    graph g;
    g.init();
    return 0;
}


