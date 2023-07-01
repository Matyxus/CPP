cmake_minimum_required(VERSION 3.0.0)
project(Maze VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

file(GLOB_RECURSE SOURCES
    src/*.hpp
    src/*.cpp
    main.cpp
)

add_executable(Maze ${SOURCES})
# add_executable(Maze main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
