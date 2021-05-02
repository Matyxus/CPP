#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <map>

//Check hw7_image for graphical representation of second test case (.in file).

/*
Given lists of cities and connections between pairs of cities including cost of travel, 
list of cities that are on the prime Meridian.
Find lowest cost of trip, where you start on one of the cities that are on the prime Meridian, and you have to
travel to "P" cities that are also on the prime Meridian (including the last city.).
Return the lowest cost of this trip.
*/

typedef struct node {
    int curr_min_distance; // current minimal distance ofund
    int search; // what was being search for on this node
    int id; // id
    bool end_node; // is 0th meridian city ?
    std::vector<std::pair<int, int>> edge_cost; // pair.first = city, pair.second = cost of the path
    std::vector<int> in_neighbours; // in
    std::vector<int> out_neighbours; // out, empty for cities connected to polar cities
    node(int num) {
        end_node = false;
        search = 0;
        id = num;
    }
} node;

typedef struct helper {
    int *cache; // array, size of P, where P is circulation count and cache[i] is distance
} helper;


class solver {
    private:
        int P = 0; // Number of circulations
        int N = 0; // Number of cities on 0th meridian
        int M = 0; // Number of all cities, numbers from 1 to M
        int T = 0; // Number of edges
        // M ≤ 10^5, T ≤ 10^6, 2 ≤ N ≤ 10, P ≤ 10^3
        int dist = std::numeric_limits<int>::max(); // shortest distance
        helper *memory; // saving distance on given city, on given circulation
        int **shortest_path; // shortest paths for all cities, NxN matrix
        std::map<int, int> my_map; // Mapping cities on meridian to 0...10
        std::vector<int> target_city; // Cities on 0th meridian
        std::vector<node*> graph; // graph containing all connections etc.
        std::vector<node*> topsort; // topological sort of graph
    public:
        solver(/* args */);
        ~solver();
        inline void getInt(int &n);
        void init();
        void topological_sort();
        void find_shorest_paths(const int from);
        void shortest_circulation_path(int from, int curr_circ, int curr_dist);
};

solver::solver(){};

solver::~solver() {
    // free
    for (int i = 0; i < M+1; i++) {
        delete graph[i];
    }
    //
    for (int i = 0; i < N; i++) {
        free(shortest_path[i]);
    }
    free(shortest_path);
    //
    for (int i = 0; i < N; i++){
        free(memory[i].cache);
    }
    free(memory);
}

inline void solver::getInt(int &n) {
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
/* Kahn’s algorithm for Topological Sorting */
void solver::topological_sort() {
    std::queue<node*> helper_queue;
    // push meridian cities into queue
    for (const auto &town: target_city) {
        helper_queue.push(graph[town]);
    }
    while(!helper_queue.empty()) {
        node* top = helper_queue.front();   //remove the first element from the graph
        topsort.push_back(top);
        helper_queue.pop();
        for (const auto &city: top->out_neighbours) {
            node* neighbour = graph[city];
            neighbour->in_neighbours.erase(find(neighbour->in_neighbours.begin(), neighbour->in_neighbours.end(), top->id));
            if ( neighbour->in_neighbours.size() == 0 ) {
                helper_queue.push(neighbour);
            }
        }
    }
    return;
}

void solver::shortest_circulation_path(int from, int curr_circ, int curr_dist) {
    if (curr_circ == P) { // done P circulations
        dist = std::min(dist, curr_dist);
        return;
    }
    for (int to = 0; to < N; to++) {
        // if there is a path, if visited on same iteration, path needs to be smaller than previously found
        if (shortest_path[from][to] > 0 && memory[to].cache[curr_circ] > curr_dist+shortest_path[from][to]) {
            memory[to].cache[curr_circ] = curr_dist+shortest_path[from][to];
            shortest_circulation_path(to, curr_circ+1, curr_dist+shortest_path[from][to]);
        }
    }
    return;
}

void solver::find_shorest_paths(const int from) {
    // init nodes with distance
    for (const auto &pair: graph[from]->edge_cost) {
        graph[pair.first]->curr_min_distance = pair.second; // only pair second, polar city is starting pos
        graph[pair.first]->search = from; // going from this city
    }
    // find shortest path, will be saved in meridian cities
    for (auto node: topsort) {
        if (node->search == from) {
            for (const auto &pair: graph[node->id]->edge_cost) {
                if (graph[pair.first]->search == from) { // visited already ? -> compare distance
                    if (node->curr_min_distance + pair.second < graph[pair.first]->curr_min_distance) {
                        graph[pair.first]->curr_min_distance = node->curr_min_distance + pair.second;
                    }
                } else { // not visited
                    graph[pair.first]->curr_min_distance = node->curr_min_distance+pair.second;
                    graph[pair.first]->search = from;
                }
            }
        }
    }
    return;
}


void solver::init() {
    scanf("%d %d %d %d", &M, &T, &N, &P);
    // init graph
    for (int i = 0; i < M+1; i++) {
        graph.push_back(new node(i));
    }
    int from, to, cost;
    // load cities on meridian
    for (int i = 0; i < N; i++) {
        getInt(from); 
        target_city.push_back(from);
        // mark cities on meridian as such
        graph[from]->end_node = true;
        graph[from]->curr_min_distance = std::numeric_limits<int>::max();
        my_map.insert(std::pair<int, int>(from, i)); 
    }
    // load edges
    for (int i = 0; i < T; i++) {
        getInt(from);
        getInt(to);
        getInt(cost);
        // If this city, has connection to polar city, remove such a connection
        // to create DAG
        if (!graph[to]->end_node) {
            graph[from]->out_neighbours.push_back(to);
        }
        graph[from]->edge_cost.push_back({to, cost});
        graph[to]->in_neighbours.push_back(from);
    }
    
    // clear in_neighbours of polar cities, to make them starting point for topological sort -> DAG
    for (const auto &town: target_city) {
        graph[town]->in_neighbours.clear();
    }
    
    topological_sort();

    shortest_path = (int ** ) malloc(N*sizeof(int*));
    for (int i = 0; i < N; i++) {
        shortest_path[i] = (int *) calloc(N, sizeof(int));
    }

    for (const auto &town: target_city) {
        find_shorest_paths(town);
        // write shortest found paths to array
        for (const auto &city: target_city) {
            if (graph[city]->search == town) { // if there is path
                
                shortest_path[my_map[town]][my_map[city]] = graph[city]->curr_min_distance;
            }
        }
    }
    
    memory = (helper *) malloc(N*sizeof(helper));
    for (int i = 0; i < N; i++) {
        memory[i].cache = (int *) malloc(P*sizeof(int));
        for (int j = 0; j < P; j++) {
            memory[i].cache[j] = std::numeric_limits<int>::max();
        }
    }
    for (int i = 0; i < N; i++) {
        shortest_circulation_path(i, 0, 0);
    }
    printf("%d\n", dist);
    return;
}

int main(int argc, char const *argv[]) {
    solver solve;
    solve.init();
    return 0;
}
