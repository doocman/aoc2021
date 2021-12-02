

cmake_minimum_required (VERSION 3.8)

add_executable(aoc2021_dec1 dec1_main.cpp)

target_link_libraries(
		aoc2021_dec1
		dooc::dooc.lib.views
)
