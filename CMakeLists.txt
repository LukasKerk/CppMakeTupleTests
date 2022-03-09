cmake_minimum_required(VERSION 3.16)

project(work)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
 
# needed for youCompleteMe (more precise: clangd)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(TupleTesting TupleTesting.cpp)

add_custom_target(visualize
  SOURCES scripts/formatting.sh scripts/evaluate.py
  DEPENDS TupleTesting
  BYPRODUCTS visualisation.pdf
  COMMAND "${CMAKE_BINARY_DIR}/TupleTesting"
    | "${CMAKE_CURRENT_SOURCE_DIR}/scripts/formatting.sh"
    | "${CMAKE_CURRENT_SOURCE_DIR}/scripts/evaluate.py"
)
