#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>

// Check hw2_image for graphical representation of first and second testing cases (.in files).
// Red agent gets 1 point for every non-empty neighbour.
// Blue agent gets 1 point for every empty neighbours.
// First line of .in file are numbers N, M, A, B, the rest is
// graph of problem.


#define EMPTY 0
#define RED 1
#define BLUE 2

typedef struct node {
    int index = 0;
    int occup = EMPTY;
    std::vector<int> connections;
    int edges = 0;
    public:
        node(int id) {index = id;}
} node;

bool edge_sort(const node* a, const node* b);

bool edge_sort(const node* a, const node* b) {
    return ((a->edges != b->edges) ?  (a->edges > b->edges): (a->index > b->index));
}

class graph {
    private:
        int N; // num of nodes
        int M; // num of edges
        int A; // num of red agents
        int B; // num of blue agents
        std::vector<node*> nodes;
        std::vector<node*> node_pointer;
        int maximum = 0;
        int red_index = 0;
        int blue_index = 0;
        int counter = 0;
    public:
        graph() {}

        int eval() {
            int temp = 0;
            for (int i = 0; i < N; i++) {
                if (nodes[i]->occup != 0) {
                    if (nodes[i]->occup == RED) {
                        for (auto const &j: nodes[i]->connections) {
                            if ( node_pointer[j]->occup != EMPTY ) {
                                temp++;
                            }
                        }
                    } else {
                        for (auto const &j: nodes[i]->connections) {
                            if ( node_pointer[j]->occup == EMPTY ) {
                                temp++;
                            }
                        }
                    }
                }
            }
            return temp;
        }

        void init() {
            scanf("%d %d %d %d", &N, &M, &A, &B);
            for (int i = 0; i < N; i++) {
                nodes.push_back(new node(i+1));
            }
            int j, k;
            for (int i = 0; i < M; i++) {
                scanf("%d %d", &j, &k);
                nodes[j-1]->connections.push_back(k-1);
                nodes[k-1]->connections.push_back(j-1);
            }
            for (int i = 0; i < N; i++) {
                nodes[i]->edges = nodes[i]->connections.size();
            }
            // pointer to nodes, used as indexing
            node_pointer = nodes;
        }

        void start() {
            // sort
            std::sort(nodes.begin(), nodes.end(), edge_sort);
            explore(A, B, 0, 0);
            printf("%d\n", maximum);
        }
        
        int eval_node(int index) {
            int temp = 0;
            if (nodes[index]->occup == RED) {
                for (auto const &j: nodes[index]->connections) {
                    // red loses point gained for blue neighbour
                    if ( node_pointer[j]->occup == RED) {
                        temp += 2; // (red-red == 2 points)
                    }
                }
            } else { // Blue
                for (auto const &j: nodes[index]->connections) {
                    if ( node_pointer[j]->occup == BLUE) {
                        temp--;
                    } else { // blue-empty and blue-red -> +1 score
                        temp++;
                    }
                }
            }
            return temp;
        }
        
        int place(int color, int index) {
            nodes[index]->occup = color;
            return eval_node(index);
        }

        void remove(int index) {
            nodes[index]->occup = EMPTY;
            return;
        }

        ~graph() {
            for ( int i = 0; i < N; i++) {
                delete nodes[i];
            }
        }

        void explore(int red, int blue, int index, int curr_score) {
            // nothing to place
            if (red+blue == 0) {
                if (curr_score > maximum){
                    maximum = curr_score;
                }
                return;
            }
            // out of bounds
            if (red+blue+index > N) {
                return;
            }
            // Upper Bound
            int upper_bound = 0;
            // Upper - red
            for (int i = index; i < red+index; i++) {
                upper_bound += nodes[i]->edges*2;
            }
            // Upper - blue
            for (int i = index; i < blue+index; i++) {
                upper_bound += nodes[i]->edges;
            }
            // Cut-off
            if (curr_score+upper_bound <= maximum) {
                return;
            }
            // ----- Place ----- 
            int score_change = 0;
            // place - red
            if (red != 0) {
                score_change = place(RED, index);
                explore(red-1, blue, index+1, curr_score+score_change);
                remove(index);
            }
            if (curr_score+upper_bound <= maximum) {
                return;
            }
            // place - blue
            if (blue != 0) {
                score_change = place(BLUE, index);
                explore(red, blue-1, index+1, curr_score+score_change);
                remove(index);
            }
            // check index
            if (red+blue+index >= N) {
                return;
            }
            // dont place any
            return explore(red, blue, index+1, curr_score);
        }
};

int main(int argc, char const *argv[]) {
    graph G;
    G.init();
    G.start();
    return 0;
}
