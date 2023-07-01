#pragma once
#include <iostream>
#include <string>
#include <tuple>
#include <stdint.h>

#define MAX_WIDTH 1000
#define MAX_HEIGHT 1000

/**
 * Class representing Maze, contains Maze structure with parameters
*/
class Maze {
    private:
        unsigned int width, height;
        unsigned int start_x, start_y;
        unsigned int end_x, end_y;
        bool **labyrinth = nullptr;

    public:
        /**
         * @param size of maze (width, height)
         * @param entry point of maze (x, y)
         * @param exit point of maze (x, y)
        */
        Maze(
            std::tuple<unsigned int, unsigned int> size, 
            std::tuple<unsigned int, unsigned int> entry, 
            std::tuple<unsigned int, unsigned int> exit
        );

        /**
         * @return True if Maze was successfully constructed, false otherwise
        */
        bool is_valid() {return labyrinth != nullptr;};
        
        /**
         * 
        */
        bool check_coord(unsigned int x, unsigned int y) {return x < width && y < height;};
        bool check_coord(std::tuple<unsigned int, unsigned int> coord) {return check_coord(std::get<0>(coord), std::get<1>(coord));};
        /**
         * 
        */
        ~Maze();

    /**
     * @param os output stream
     * @param maze class representing maze
     * @return Formatted string of Maze representation
    */
    friend auto operator<<(std::ostream& os, Maze const& maze) -> std::ostream& { 
        // Check maze
        if (maze.labyrinth == nullptr) {
            std::cerr << "Error, cannot check coordinate if Maze is invalid!" << std::endl;
            return os;
        }
        // Start printing maze
        std::string ret_val = std::string(maze.width + 2, '#') + "\n"; // Roof
        int row_lenght = ret_val.length();
        for (unsigned int row = 0; row < maze.width; row++) {
            ret_val += '#';
            for (unsigned int col = 0; col < maze.height; col++) {
                ret_val +=  ((char) (maze.labyrinth[row][col] * 11 + '#'));
            }
            ret_val += "#\n";
        }
        ret_val += (std::string(maze.width + 2, '#') + "\n");
        // Add starting + ending position
        ret_val.replace(row_lenght + (maze.start_x + 1) + maze.start_y * row_lenght, 1, "S");
        ret_val.replace(row_lenght + (maze.end_x + 1) + maze.end_y * row_lenght, 1, "E");

        // Idea, since we have NxM matric of 0 & 1
        // Create char matrix, which has 2 more rows (for ceiling and floor)
        // All chars are initialized as "#" -> Wall
        // Add the original matrix (multiply by 11) to the char array 
        // 0*11 = 0 + # = #
        // 1*11 = 11 + # = .
        return os << ret_val;
    }
};
