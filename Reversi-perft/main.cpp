#include "perft.h"
#include <string>
bool yes_no_error(std::string input);

int main(int argc, char const *argv[]) {
    bool parallelize = false;
    int depth = 0;
    std::string input = "";
    int thread_count = 2;
    // Ask user for parallelization.
    std::cout << "Do you want parallel search? (Yes\\No): ";
    std::cin >> input;
    if (yes_no_error(input)) {
        return 1;
    }
    parallelize = (input == "Yes") ? true:false; 
    std::cout << "Enter maximal depth: ";
    std::cin >> depth;
    // Ask for thread count.
    if (parallelize) {
        std::cout << "Enter how many threads will run: (max is " << omp_get_max_threads() << "). ";
        std::cin >> thread_count;
        // Error handled inside perft class.
    }
    if (depth > 14) {
        bool end = true;
        std::cout << "Depth 15 will take about an hour, do you want to continue? (Yes\\No): ";
        std::cin >> input;
        if (yes_no_error(input) || input == "Yes") {
            return 1;
        }
    }
    perft test = perft(parallelize, depth, thread_count);
    return 0;
}

bool yes_no_error(std::string input) {
    if (input != "Yes" && input != "No") {
        std::cerr << "Error, enter Yes or No, exiting..." << std::endl;
        return true;
    }
    return false;
}
