# CMakeList.txt : CMake project for AdventOfCode2021, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("AdventOfCode2021")

set(CMAKE_CXX_STANDARD 20 CACHE INTERNAL "")

# Add source to this project's executable.
add_executable (AdventOfCode2021 "AdventOfCode2021.cpp" "AdventOfCode2021.h")

add_subdirectory(dec1)
add_subdirectory(dec2)
add_subdirectory(dec3)
add_subdirectory(dec4)
add_subdirectory(dec5)
add_subdirectory(dec6)
add_subdirectory(dec7)
add_subdirectory(dec8)
add_subdirectory(dec9)
add_subdirectory(dec10)
add_subdirectory(dec11)

add_subdirectory(library)

# TODO: Add tests and install targets if needed.
