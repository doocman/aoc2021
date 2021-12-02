

cmake_minimum_required (VERSION 3.8)

add_executable(aoc2021_dec2 dec2_main.cpp)

add_executable(aoc2021_dec2.test dec2.test.cpp)

add_library(aoc2021_dec2.headers INTERFACE)
add_library(ns::aoc2021_dec2.headers ALIAS aoc2021_dec2.headers)

target_include_directories(aoc2021_dec2.headers
		INTERFACE
		""
)

target_link_libraries(aoc2021_dec2
		PRIVATE
		ns::aoc2021_dec2.headers
)

target_link_libraries(aoc2021_dec2.test
		PRIVATE
		ns::aoc2021_dec2.headers
)
