#include "maze.hpp"

Maze::Maze(
    std::tuple<unsigned int, unsigned int> size, 
    std::tuple<unsigned int, unsigned int> entry, 
    std::tuple<unsigned int, unsigned int> exit
){  
    // Check size
    if (std::get<0>(size) > MAX_WIDTH || std::get<1>(size) > MAX_HEIGHT) {
        size = {MAX_WIDTH, MAX_HEIGHT};
    }
    width = std::get<0>(size);
    height = std::get<1>(size);
    // Check entry point
    if (!check_coord(entry)) {
        std::cerr << "Error, entry point (" << std::get<0>(entry) << "," << std::get<1>(entry) << ") is not in maze!" << std::endl;
        return;
    } 
    start_x = std::get<0>(entry);
    start_y = std::get<1>(entry);
    // Check exit point
    if (!check_coord(exit)) {
        std::cerr << "Error, exit point (" << std::get<0>(exit) << "," << std::get<1>(exit) << ") is not in maze!" << std::endl;
        return;
    }
    end_x = std::get<0>(exit);
    end_y = std::get<1>(exit);
    // Allocate memory for maze
    std::cout << "Initializing maze of size: (" << width << "," << height << ") allocating memory ..." << std::endl;
    labyrinth = (bool **) malloc(height * sizeof(bool*));
    for (unsigned int col = 0; col < height; col++) {
        labyrinth[col] = (bool *) calloc(width, sizeof(bool));
    }
    std::cout << "Successfully initialize Maze!" << std::endl;
}


Maze::~Maze() {
    std::cout << "Exiting Maze, freeing memory ..." << std::endl;
    if (is_valid()) {
        for (unsigned int i = 0; i < height; i++) {
            free(labyrinth[i]);
        }
        free(labyrinth);
    }
    std::cout<< "Memory freed" << std::endl;
};


