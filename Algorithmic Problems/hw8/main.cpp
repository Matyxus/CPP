#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <map>

// Check hw8_image for graphical representation of first test case (.in file).
/*
Villages V_1 ... V_n.
P_i describes difference between production and consumption of village V_i.
To construct road between V_j and V_(j+1):
    V_i is village with lowest index, that is reachable from V_j by already constructed roads.
    V_k is village with highest index, that is reachable from V_(j+1) by already constructed roads.
    The cost of road between V_j and V_(j+1) is given by production and consumption of villages V_i...V_k.
    S_1 = P_i + ... + P_j (difference between consumption and production of villages V_i...V_j)
    S_2 = P_(j+1) + ... + P_k (difference between consumption and production of villages V_(j+1)..V_k)
    S_min  = min{S1, S2}, S_max = max{S1, S2}, S = min{|S_min|, |S_max|} and L = k − i.
    if S_min < 0 < S_max and S > L, than the cost of road will be S-L, else 0.

Given numbers P_i describing difference of production and consumption in villages.
Given cost of construction for one road.
It is known which village is the main one.
Find optimal plan for road construction that will lead to highest profit, return the profit.
*/

int main(int argc, char const *argv[]) {
    int N = 0; // Number of villages 
    int H = 0; // Which village is the main one
    int C = 0; // Cost of road !(negative)
    scanf("%d %d %d", &N, &H, &C);
    C *= -1; 
    // production differences from village A->N, B->N ... N->N
    int **cost = (int **) malloc(N*sizeof(int*));
    int **result = (int **) malloc(N*sizeof(int*));
    for (int i = 0; i < N; i++) {
        cost[i] = (int *) calloc(N, sizeof(int));
        result[i] = (int *) calloc(N, sizeof(int));
    }
    // load
    int temp;
    for (int i = 0; i < N; i++) {
        scanf("%d", &temp);
        cost[i][i] = temp;
    }
    // Sum of production from A->N...N->N
    for (int i = 0; i < N; i++) {
        for (int j = i+1; j < N; j++) {
            cost[i][j] = cost[i][j-1]+cost[j][j];
        }
    }
    // calculate cost of building roads between villages
    int S_1;
    int S_2;
    int S_min;
    int S_max;
    int S;
    int L = 1; // road count between connected villages
    for (int i = N-2; i >= 0; i--) {
        for (int j = i; j < N-1; j++) {
            std::pair<int, int> from(i, j);
            std::pair<int, int> to(j+1, j+1);
            result[i][j+1] = std::numeric_limits<int>::min();
            while (true) {
                S_1 = cost[from.first][from.second];
                S_2 = cost[to.first][to.second];
                S_min = std::min(S_1, S_2);
                S_max = std::max(S_1, S_2);
                temp = C;
                if (S_min < 0 && 0 < S_max) {
                    S = std::min(abs(S_1), abs(S_2));
                    if (S > L) {
                        temp = S-L+C;
                    }
                }
                temp += (result[from.first][from.second]+result[to.first][to.second]);
                result[i][j+1] = std::max(result[i][j+1], temp);
                from.second--;
                to.first--;
                if (from.second < from.first) {
                    break;
                }
            }
            L++;
        }
        L = 1;
    }
    // find max, search in sub_matrix
    int best = std::numeric_limits<int>::min();
    for (int i = 0; i < H; i++) {
        for (int j = H-1; j < N; j++) {
            best = std::max(best, result[i][j]);
        }
    }
    printf("%d\n", best);
    
    for (int i = 0; i < N; i++) {
        free(cost[i]);
        free(result[i]);
    }
    free(cost);
    free(result);
    return 0;
}

