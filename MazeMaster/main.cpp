#include <stdio.h>
#include <iostream>
#include <tuple>
#include "src/maze/maze.hpp"


int main(int argc, char const *argv[]) {
    
    std::cout << "Hello" << std::endl;
    Maze temp = Maze({10, 10}, {0, 0}, {9, 9});
    std::cout << temp;
    // char temp = '#';
    // std::cout << (char) (temp + 0) << std::endl;
    // std::cout << (char) (temp + 11) << std::endl;

    return 0;
}











