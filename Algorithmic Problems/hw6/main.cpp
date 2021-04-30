#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <queue>
#include <assert.h>
#include <limits>
#include <algorithm>

using namespace std;
// g++ -std=c++11 -pipe -Wall -O3 -c main.cpp
// g++ -std=c++11 main.o -o main

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

int global_max = numeric_limits<int>::min();
int global_min = numeric_limits<int>::max();
int best_score = numeric_limits<int>::min();

int main() {
    int N; // Number of days
    getInt(N);
    int tmp;
    vector<int> array;
    for (int i = 0; i < N; i++) {
        getInt(tmp);
        array.push_back(tmp);
    }
    vector<int> helper_array; // Stores A
    /* Maximalize function min(B-A, C-B), where A is before B, which is before C in array */
    /* Take each node as B -> middle day, find best A and C */
    // find best min for each Node
    for (int i = 1; i < N-1; i++) { // B = array[i]
        global_min = min(array[i-1], global_min); // A
        helper_array.push_back(global_min); // store A
    }
    // find best max
    for (int i = N-2; i > 0; i--) { // B = array[i]
        global_max = max(array[i+1], global_max); // C
        // Maximalize   function    min(B-A, C-B)
        best_score = max(min(array[i]-helper_array[i-1], global_max-array[i]), best_score);
    }
    printf("%d\n", best_score);
    return 0;
}
