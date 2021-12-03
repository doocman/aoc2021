

cmake_minimum_required (VERSION 3.8)

add_executable(aoc2021_dec3 dec3_main.cpp)

add_executable(aoc2021_dec3.test dec3.test.cpp)

add_library(aoc2021_dec3.headers INTERFACE)
add_library(ns::aoc2021_dec3.headers ALIAS aoc2021_dec3.headers)

target_include_directories(aoc2021_dec3.headers
		INTERFACE
		""
)

target_link_libraries(aoc2021_dec3.headers
		INTERFACE
		dooc::dooc.lib.views
		)

target_link_libraries(aoc2021_dec3
		PRIVATE
		ns::aoc2021_dec3.headers
)

target_link_libraries(aoc2021_dec3.test
		PRIVATE
		ns::aoc2021_dec3.headers
)
