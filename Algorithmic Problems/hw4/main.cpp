#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <queue>
#include <assert.h>
#include <limits>

int x_change[] = {1, 0, -1, 0};
int y_change[] = {0, 1, 0, -1};


// g++ -std=c++11 -pipe -Wall -O3 -c main.cpp
// g++ -std=c++11 main.o -o main


typedef struct node {
    int search; // what is being searched for on this node
    int distance; // distance from previous number ( if occup == 1 -> from robot)
    // when backtracking
    int occup; // occupied by number
    bool found; // for backtracking
} node;

class Graph {
    private:
        int R = 0; // number of rows
        int S = 0; // number of cols
        int P = 0; // number of obstacles
        int T = 0; // range for points (10 -> 1...10)
        int R_0 = 0, S_0 = 0; // location of robot
        node **matrix = nullptr; // graph
        bool **visited = nullptr; // tracks visited nodes
        int steps = std::numeric_limits<int>::max();
    public:
        Graph() {};
        void init() {
            //printf("INIT\n");
            scanf("%d %d %d %d", &R, &S, &P, &T);
            //printf("R = %d, S = %d, P = %d, T = %d\n", R, S, P, T);
            matrix = (node **) malloc(R*sizeof(node*));
            for (int i = 0; i < R; i++) {
                matrix[i] = (node *) malloc(S*sizeof(node));
                for (int j = 0; j < S; j++) {
                    matrix[i][j].distance = std::numeric_limits<int>::max();
                    matrix[i][j].found = false;
                    matrix[i][j].occup = 0;
                    matrix[i][j].search = 0;
                }
            }
            visited = (bool**) malloc(R*sizeof(bool*));
            for (int i = 0; i < R; i++) {
                visited[i] = (bool *) calloc(S, sizeof(bool));
            }
            scanf("%d %d", &R_0, &S_0); // robot location
            int j, k;
            // scan for wall
            for (int i = 0; i < P; i++) {
                getInt(j);
                getInt(k);
                j = abs(j-R); // rotation
                visited[j][(k-1)] = true; // WALL
                matrix[j][(k-1)].search = T+1; // Wall, dont search here
            }
            // scan for points
            int temp;
            for (int i = 0; i < T; i++) {
                getInt(temp); // number of I-types
                for (int l = 0; l < temp; l++) {
                    getInt(j);
                    getInt(k);
                    j = abs(j-R); // rotation
                    matrix[j][(k-1)].occup = i+1; // I-point
                }
            }
            //printer();
            explore(abs(R_0-R), S_0-1);
        }
        void printer() {
            for (int i = 0; i < R; i++) {
                for (int j = 0; j < S; j++) {
                    printf("%d ", matrix[i][j].occup);
                }
                printf("\n");
            }
            printf("\n");
        }

        void explore(int x, int y) {
            int curr_steps = 0;
            std::queue<std::pair<int, int>> q;
            int topx, topy;
            int j = 1, k = 0;
            q.push({x, y});
            visited[x][y] = true; 
            //
            while(!q.empty()) {
                // start, get first element in queue
                std::pair<int, int> tmp = q.front();
                // works as parent node
                topx = tmp.first;
                topy = tmp.second;
                q.pop();
                j--; // decrease number of items in queue
                for(int i = 0; i < 4; i++) { // possible neighbours
                    x = topx + x_change[i];
                    y = topy + y_change[i];
                    if(in_bounds(x, y)) { 
                        // first BFS, searching for 1's, giving them their real distance from Robot.
                        if (visited[x][y] == false) { /* Process node */
                            visited[x][y] = true;
                            matrix[x][y].search = 1;
                            // found 1, add it for backtracking
                            if (matrix[x][y].occup == 1) {
                                matrix[x][y].found = true;
                                matrix[x][y].search = 2;
                                matrix[x][y].distance = curr_steps+1;
                            }
                            q.push({x, y});
                            k++; // adding num of items to queue
                        }
                        // Backtracking, compare what is being search for, explore if its <=
                        if (matrix[topx][topy].found == true) {
                            // found searched number
                            if (matrix[x][y].occup == matrix[topx][topy].search) {
                                // is last ?
                                if (matrix[x][y].occup == T) {
                                    // update distance
                                    if (matrix[topx][topy].distance+1 < steps) {
                                        steps = matrix[topx][topy].distance+1;
                                    } 
                                } else { // is not last
                                    // should be added ?
                                    // 1) if search is less on this node, explore it ( -> was explored only by initial BFS )
                                    if (matrix[x][y].search < matrix[x][y].occup+1) {
                                        matrix[x][y].search = matrix[topx][topy].search+1;
                                        matrix[x][y].found = true;
                                        matrix[x][y].distance = matrix[topx][topy].distance+1;
                                        k++;
                                        q.push({x, y});
                                    }
                                    // 2) if search on this node == node.occup+1, than compare distances
                                    if (matrix[x][y].search == matrix[x][y].occup+1) {
                                        // distance comparison
                                        if (matrix[x][y].distance > matrix[topx][topy].distance+1) {
                                            matrix[x][y].search = matrix[topx][topy].search+1;
                                            matrix[x][y].found = true;
                                            matrix[x][y].distance = matrix[topx][topy].distance+1;
                                            k++;
                                            q.push({x, y});
                                        }
                                    }
                                    // 3) if search is higher on this node, dont add it
                                }
                            } else { // is not occupied, or is occupied by something i am not searching for
                                // should be added ?
                                // 1) what is being search for on this node is less, than current search
                                if (matrix[x][y].search < matrix[topx][topy].search) { 
                                    // add it
                                    matrix[x][y].search = matrix[topx][topy].search;
                                    matrix[x][y].found = true;
                                    matrix[x][y].distance = matrix[topx][topy].distance+1;
                                    k++;
                                    q.push({x, y});
                                }
                                // 2) if search on this node == current search, than compare distances
                                if (matrix[x][y].search == matrix[topx][topy].search) { 
                                    // distance comparison
                                    if (matrix[x][y].distance > matrix[topx][topy].distance+1) {
                                        // add it
                                        matrix[x][y].search = matrix[topx][topy].search;
                                        matrix[x][y].found = true;
                                        matrix[x][y].distance = matrix[topx][topy].distance+1;
                                        k++;
                                        q.push({x, y});
                                    }
                                }
                                // 3)  if search on this node, is already higher, than what i am looking for, stop search
                            }
                        }
                    }
                }
                // if num of items added previously is 0 -> all were processed -> steps+1
                if (j == 0) {
                    j = k;
                    k = 0;
                    curr_steps++;
                }
            }
            printf("%d\n", steps);
            return;
        }
        bool in_bounds(const int &x, const int &y) {
            if (x > -1 && y > -1) {
                if (x < R && y < S) {
                    return true;
                }
            }
            return false;
        }
        ~Graph() {
            for (int i = 0; i < R; i++) {
                free(matrix[i]);
                free(visited[i]);
            }
            free(matrix);
            free(visited);
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
};


int main(int argc, char const *argv[]) {
    Graph g;
    g.init();
    return 0;
}



