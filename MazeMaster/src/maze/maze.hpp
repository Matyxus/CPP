#pragma once
#include <iostream>
#include <string>
#include <tuple>
#include <stdint.h>

/**
 * Class representing maze, contains maze structure with parameters
*/
class maze {
    private:
        unsigned int width, height;
        unsigned int start, end;
        bool **labyrinth = nullptr;

    public:
        /**
         * @param size of maze (width, height)
         * @param entry of maze (x, y)
         * @param exit of maze (x, y)
        */
        maze(
            std::tuple<unsigned int, unsigned int> size, 
            std::tuple<unsigned int, unsigned int> entry, 
            std::tuple<unsigned int, unsigned int> exit
        );

        /**
         * @return True if maze was successfully constructed, false otherwise
        */
        bool is_valid() {return labyrinth != nullptr;};
        /**
         * 
        */
        bool check_coord(unsigned int x, unsigned int y) {
            if (!is_valid()) {
                std::cerr << "Error, cannot check coordinate if maze is invalid!" << std::endl;
                return false;
            }
            return x < width && y < height;
        };
        bool check_coord(std::tuple<unsigned int, unsigned int> coord) {return check_coord(std::get<0>(coord), std::get<1>(coord));};
        /**
         * 
        */
        ~maze();

    /**
     * @param os output stream
     * @param maze class representing maze
     * @return Formatted string of maze representation
    */
    friend auto operator<<(std::ostream& os, maze const& maze) -> std::ostream& { 
        std::string ret_val = "";
        return os << ret_val;
    }
};

maze::maze(
    std::tuple<unsigned int, unsigned int> size, 
    std::tuple<unsigned int, unsigned int> entry, 
    std::tuple<unsigned int, unsigned int> exit
){};

maze::~maze() {
    std::cout << "Exiting maze, freeing memory ..." << std::endl;
    if (is_valid()) {
        for (unsigned int i = 0; i < height; i++) {
            free(labyrinth[i]);
        }
        free(labyrinth);
    }
    std::cout<< "Memory freed" << std::endl;
};

    







