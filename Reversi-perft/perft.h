#pragma once
#include <stdint.h>
#include <time.h>
#include <iostream>
#include <iomanip>
#include <queue>
#include <tuple>
#include <omp.h>

class perft {
    private:
        const uint64_t LEFT_SIDE = 0x7F7F7F7F7F7F7F7F; // For West side.
        const uint64_t RIGHT_SIDE = 0xFEFEFEFEFEFEFEFE; // For East side.
        const uint64_t ONE = 1; // One uint64 for bit shifting.
        const uint64_t WHITE_BOARD = 0x810000000; // White starting board.
        const uint64_t BLACK_BOARD = 0x1008000000; // Black starting board.
        uint64_t node_count = 0; // Node count for perft.
        /*
            Queue storing board for both players and depth at which they were found.
        */
        std::queue<std::tuple<uint64_t, uint64_t, int>> queue;
    public:
        /*
            Sequential search up to ceratin depth, returns number of found moves.
        */
        uint64_t sequential(uint64_t curr_board, uint64_t enemy_board, int depth);
        /*
            Parallelized sequential function, stores result of found moves in "node_count".
        */
        void parallel(int depth, int num_threads);
        /*
            Pushes boards found at depth into queue, resulting depth is in "to_depth".
        */
        void explore(uint64_t curr_board, uint64_t enemy_board, int depth, int to_depth);
        
        uint64_t generate_moves(const uint64_t curr_board, const uint64_t enemy_board);
        uint64_t generate_flipped(const uint64_t curr_board, const uint64_t enemy_board, const uint64_t pos);
        /*
            Returns the most significant set bit, -1 if number is zero.
        */
        const inline int bit_scan_reverse(const uint64_t num) {
            return 63-__builtin_clzll(num);
        }

        perft(bool parallelize, int to_depth, int num_threads=2);
        ~perft();
};
/*
-------- Testing Move Generator -----------
Time measured on i7-87000, compiled with O3, runned on all 12 threads.
https://www.chessprogramming.org/Perft
https://www.aartbik.com/strategy.php
Symbol "\" means, there is no point in using parallel search.
 ________________________________________________________________
| DEPTH |   LEAF_NODES        | ETA(sec.)   | ETA(sec.) Parallel |
|=======|=====================|=============|====================|
|   1   |                4    |    ~ 0      |       ~ \          |      
|   2   |               12    |    ~ 0      |       ~ \          |
|   3   |               56    |    ~ 0      |       ~ \          |
|   4   |              244    |    ~ 0      |       ~ \          |
|   5   |            1.396    |    ~ 0      |       ~ \          |
|   6   |            8.200    |    ~ 0      |       ~ \          |
|   7   |           55.092    |    ~ 0      |       ~ \          |
|   8   |          390.216    |    ~ 0      |       ~ \          |
|   9   |        3.005.288    |    ~ 0      |       ~ \          |
|  10   |       24.571.284    |    ~ 1      |       ~ \          |
|  11   |      212.258.800    |    ~ 3      |       ~ 0.5        |
|  12   |    1.939.886.636    |    ~ 24     |       ~ 4          |
|  13   |   18.429.641.748    |    ~ ?      |       ~ 34         |
|  14   |  184.042.084.512    |    ~ ?      |       ~ 380        |
|=======|=====================|=============|====================|
*/

