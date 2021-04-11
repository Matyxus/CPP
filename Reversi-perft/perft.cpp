#include "perft.h"

perft::perft(bool parallelize, int to_depth, int num_threads) {
    if (to_depth < 1) {
        std::cerr << "Error, depth cant be smaller than 1!" << std::endl;
        return;
    }
    if (parallelize && omp_get_max_threads() < num_threads) {
        std::cerr << "Error, maximal number of threads is " << omp_get_max_threads() << "!" << std::endl;
        return;
    }
    if (parallelize && num_threads < 2) {
        std::cerr << "Error, less than 2 threads is not parallel!" << std::endl;
        return;
    }
    if (parallelize && to_depth < 10) {
        std::cerr << "Error, depth under 10 takes less than second on sequential, use parallel for higher!" << std::endl;
        return;
    }
    clock_t start, end;
    double cpu_time_used;
    start = clock();
    if (parallelize) {
        std::cout << "Parallel search to depth " << 
            to_depth << " with " << num_threads << " threads." << std::endl;
        parallel(to_depth, num_threads);
    } else {
        std::cout << "Sequential search to depth " << to_depth << std::endl;
        node_count = sequential(WHITE_BOARD, BLACK_BOARD, to_depth);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    std::cout << "Moves found: " << node_count << std::endl;
    std::cout << "Took " << std::fixed << std::setprecision(5) << cpu_time_used << " seconds to compute." << std::endl;
}

perft::~perft() {};

/*
    Parallel performance and correctness test.
*/
void perft::parallel(int depth, int thread_count) {
    //std::cout << "Performing initial search to depth 3" << std::endl;
    explore(WHITE_BOARD, BLACK_BOARD, 3, 3); // Finds 56 boards.
    std::vector<double> time_per_thread(thread_count, 0);
    const int limit = 2*thread_count;

    // Find as many boards, as is double the amount of threads,
    // each thread after finishing its work, will check if there are enough boards in queue,
    // if not, it will explore and push more boards into queue, than start working, all threads will
    // behave similary, if boards in queue have less than "depth"-8 depth, no more will be pushed into queue.
    
    bool resize = true;   
    #pragma omp parallel reduction(+ : node_count) num_threads(thread_count)
    {   
        bool do_work = true;
        clock_t start = clock();
        clock_t end = 0;
        //int thread_id = omp_get_thread_num();
        while (do_work) {
            // Base case.
            std::tuple<uint64_t, uint64_t, int> work(0, 0, 0);
            #pragma omp critical 
            {   
                if (queue.empty()) {
                    //std::cout << "No work to be done, thread " << thread_id << " exiting." << std::endl;
                    do_work = false;
                    end = clock(); 
                }
                // Explore if one thread finished faster than others.
                if (resize && do_work && queue.size() < limit) {
                    //std::cout << "Resizing queue" << std::endl
                    while (queue.size() < limit) {
                        work = queue.front();
                        if (std::get<2>(work) > depth-8) {
                            resize = false;
                            break;
                        }
                        queue.pop();
                        explore(std::get<0>(work), std::get<1>(work), 1, std::get<2>(work)+1);
                        
                    }
                }
                // Take work from queue.
                if (do_work) {
                    work = queue.front();
                    queue.pop();
                }  
            } // End of critical section.
            // Perform search.
            if (do_work) {
                node_count += sequential(std::get<0>(work), std::get<1>(work), depth-std::get<2>(work));
            }
        }
        double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        time_per_thread[omp_get_thread_num()] = cpu_time_used;
    }
    std::cout << "Queue is empty, work finished." << std::endl;
    for (int i = 0; i < thread_count; i++) {
        std::cout << "Thread " << i << " worked in total " << std::fixed 
            << std::setprecision(5) << time_per_thread[i] << " sec." << std::endl;
    }
    return;
}

/*
    Find and saves all boards, called with depth 1, doesnt work for boards, that
    need to swap players.
*/
void perft::explore(uint64_t curr_board, uint64_t enemy_board, int depth, int to_depth) {
    uint64_t moves_list = generate_moves(curr_board, enemy_board);
    if (moves_list == 0 && depth == 1) {
        queue.push({curr_board, enemy_board, to_depth-1});
        return; // Cant be explored in one pass.
    }
    // End.
    if (depth == 0) {
        queue.push({curr_board, enemy_board, to_depth});
        return;
    }
    // For all moves.
    while (moves_list) {
        uint64_t pos = (ONE << bit_scan_reverse(moves_list));
        moves_list ^= pos;
        uint64_t to_swap = generate_flipped(curr_board, enemy_board, pos);
        // Remove pieces from opponent, Add pieces to current, swap their order.
        explore(enemy_board ^ to_swap, (curr_board | to_swap | pos), depth-1, to_depth);
    }
    return;
}

/*
    Sequential performance and correctness test.
*/
uint64_t perft::sequential(uint64_t curr_board, uint64_t enemy_board, int depth) {
    uint64_t moves_list = generate_moves(curr_board, enemy_board);
    // No move, swap players.
    if (moves_list == 0) { // There is only one move.
        if (depth == 1) { // The only move is to swap players.
            return 1;
        } else { // Switch to opponent player
            return sequential(enemy_board, curr_board, depth-1);
        }
    }
    // End.
    if (depth == 1) {
        return __builtin_popcountll(moves_list);
    }
    // For all moves.
    uint64_t val = 0;
    while (moves_list) {
        uint64_t pos = (ONE << bit_scan_reverse(moves_list));
        moves_list ^= pos;
        uint64_t to_swap = generate_flipped(curr_board, enemy_board, pos);
        // Remove pieces from opponent, Add pieces to current, swap their order.
        val += sequential(enemy_board ^ to_swap, (curr_board | to_swap | pos), depth-1);
    }
    return val;
}

/*
    Generates all moves for "curr_board", and returns them.
*/
uint64_t perft::generate_moves(const uint64_t curr_board, const uint64_t enemy_board) {
    uint64_t moves = 0;
    const uint64_t empty_board = ~(curr_board | enemy_board);
    // North
    uint64_t targets = (curr_board << 8) & enemy_board;
    for (int i = 0; i < 5; i++) {
        targets |= (targets << 8) & enemy_board;
    }
    moves |= (targets << 8) & empty_board;
    // South
    targets = (curr_board >> 8) & enemy_board;
    for (int i = 0; i < 5; i++) {
        targets |= (targets >> 8) & enemy_board;
    }
    moves |= (targets >> 8) & empty_board;
    // East
    targets = ((curr_board & RIGHT_SIDE) >> 1) & enemy_board;
    for (int i = 0; i < 5; i++) {
        targets |= ((targets & RIGHT_SIDE) >> 1) & enemy_board;
    }
    moves |= ((targets & RIGHT_SIDE) >> 1) & empty_board;
    // West
    targets = ((curr_board & LEFT_SIDE) << 1) & enemy_board;
    for (int i = 0; i < 5; i++) {
        targets |= ((targets & LEFT_SIDE) << 1) & enemy_board;
    }
    moves |= ((targets & LEFT_SIDE) << 1) & empty_board;
    // North west
    targets = ((curr_board & LEFT_SIDE) << 9) & enemy_board;
    for (int i = 0; i < 5; i++) {
        targets |= ((targets & LEFT_SIDE) << 9) & enemy_board;
    }
    moves |= ((targets & LEFT_SIDE) << 9) & empty_board;
    // North east
    targets = ((curr_board & RIGHT_SIDE) << 7) & enemy_board;
    for (int i = 0; i < 5; i++) {
        targets |= ((targets & RIGHT_SIDE) << 7) & enemy_board;
    }
    moves |= ((targets & RIGHT_SIDE) << 7) & empty_board;
    // South west
    targets = ((curr_board & LEFT_SIDE) >> 7) & enemy_board;
    for (int i = 0; i < 5; i++) {
        targets |= ((targets & LEFT_SIDE) >> 7) & enemy_board;
    }
    moves |= ((targets & LEFT_SIDE) >> 7) & empty_board;
    // South east
    targets = ((curr_board & RIGHT_SIDE) >> 9) & enemy_board;
    for (int i = 0; i < 5; i++) {
        targets |= ((targets & RIGHT_SIDE) >> 9) & enemy_board;
    }
    moves |= ((targets & RIGHT_SIDE) >> 9) & empty_board;
    return moves;
}

/*
    Generates fliped pieces, if piece was added to "pos", and returns them.
*/
uint64_t perft::generate_flipped(const uint64_t curr_board, const uint64_t enemy_board, const uint64_t pos) {
    uint64_t fliped = 0;
    // North
    uint64_t to_flip = (pos << 8);
    uint64_t targets = 0;
    while (to_flip & enemy_board) {
        targets |= to_flip;
        to_flip = (to_flip << 8);
    }
    if (to_flip & curr_board) {
        fliped |= targets;
    }
    // South
    to_flip = (pos >> 8);
    targets = 0;
    while (to_flip & enemy_board) {
        targets |= to_flip;
        to_flip = (to_flip >> 8);
    }
    if (to_flip & curr_board) {
        fliped |= targets;
    }
    // East
    to_flip = ((pos & RIGHT_SIDE) >> 1);
    targets = 0;
    while (to_flip & enemy_board) {
        targets |= to_flip;
        to_flip = ((to_flip & RIGHT_SIDE) >> 1);
    }
    if (to_flip & curr_board) {
        fliped |= targets;
    }
    // West
    to_flip = ((pos & LEFT_SIDE) << 1);
    targets = 0;
    while (to_flip & enemy_board) {
        targets |= to_flip;
        to_flip = ((to_flip & LEFT_SIDE) << 1);
    }
    if (to_flip & curr_board) {
        fliped |= targets;
    }
    // North west
    to_flip = ((pos & LEFT_SIDE) << 9);
    targets = 0;
    while (to_flip & enemy_board) {
        targets |= to_flip;
        to_flip = ((to_flip & LEFT_SIDE) << 9);
    }
    if (to_flip & curr_board) {
        fliped |= targets;
    }
    // North east
    to_flip = ((pos & RIGHT_SIDE) << 7);
    targets = 0;
    while (to_flip & enemy_board) {
        targets |= to_flip;
        to_flip = ((to_flip & RIGHT_SIDE) << 7);
    }
    if (to_flip & curr_board) {
        fliped |= targets;
    }
    // South west
    to_flip = ((pos & LEFT_SIDE) >> 7);
    targets = 0;
    while (to_flip & enemy_board) {
        targets |= to_flip;
        to_flip = ((to_flip & LEFT_SIDE) >> 7);
    }
    if (to_flip & curr_board) {
        fliped |= targets;
    }
    // South east
    to_flip = ((pos & RIGHT_SIDE) >> 9);
    targets = 0;
    while (to_flip & enemy_board) {
        targets |= to_flip;
        to_flip = ((to_flip & RIGHT_SIDE) >> 9);
    }
    if (to_flip & curr_board) {
        fliped |= targets;
    }
    return fliped;
}
