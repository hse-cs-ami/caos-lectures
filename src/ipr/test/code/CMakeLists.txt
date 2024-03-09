cmake_minimum_required(VERSION 3.10)
project(MyProject VERSION 1.0)

# Specify C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Add the executable
add_executable(MyExecutable src/main.c src/dumbfunction.c)

# Enable testing and add the tests directory
enable_testing()
add_subdirectory(tests)

