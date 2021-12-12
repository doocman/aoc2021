
#include <format>
#include <iostream>
#include <cstdlib>
#include <numeric>
#include <string_view>
#include <vector>

#include <dec11.hpp>

#include <cassert>

namespace dooc
{

    constexpr std::string_view data =
        "5483143223\n"
        "2745854711\n"
        "5264556173\n"
        "6141336146\n"
        "6357385478\n"
        "4167524645\n"
        "2176841721\n"
        "6882881134\n"
        "4846848554\n"
        "5283751526\n";


    inline int main()
    {
        octopus_map_t o_map(data);

        auto o_map_100 = step_octopuses(std::move(o_map), 100);

        if (o_map_100.flashes_ != 1656)
        {
            std::format_to(std::ostream_iterator<char>(std::cerr),
                "Expected 1656, had {}", o_map_100.flashes_);
            return EXIT_FAILURE;
        }



        return EXIT_SUCCESS;
    }
}


int main()
{
    return dooc::main();
}
