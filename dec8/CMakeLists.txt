

cmake_minimum_required (VERSION 3.8)

set(CURRENT_EXT dec8)

add_executable(aoc2021_${CURRENT_EXT} ${CURRENT_EXT}_main.cpp)

add_executable(aoc2021_${CURRENT_EXT}.test ${CURRENT_EXT}.test.cpp)

add_library(aoc2021_${CURRENT_EXT}.headers INTERFACE)
add_library(ns::aoc2021_${CURRENT_EXT}.headers ALIAS aoc2021_${CURRENT_EXT}.headers)

target_include_directories(aoc2021_${CURRENT_EXT}.headers
		INTERFACE
		""
)

target_link_libraries(aoc2021_${CURRENT_EXT}.headers
		INTERFACE
		dooc::dooc.lib.views
		)

target_link_libraries(aoc2021_${CURRENT_EXT}
		PRIVATE
		ns::aoc2021_${CURRENT_EXT}.headers
)

target_link_libraries(aoc2021_${CURRENT_EXT}.test
		PRIVATE
		ns::aoc2021_${CURRENT_EXT}.headers
)
