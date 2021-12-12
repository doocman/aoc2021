
#include <format>
#include <iostream>
#include <cstdlib>
#include <numeric>
#include <string_view>
#include <vector>

#include <dec12.hpp>

#include <cassert>

namespace dooc
{

    constexpr std::string_view data =
        "fs - end\n"
        "he - DX\n"
        "fs - he\n"
        "start - DX\n"
        "pj - DX\n"
        "end - zg\n"
        "zg - sl\n"
        "zg - pj\n"
        "pj - he\n"
        "RW - he\n"
        "fs - DX\n"
        "pj - RW\n"
        "zg - RW\n"
        "start - pj\n"
        "he - WI\n"
        "zg - he\n"
        "pj - fs\n"
        "start - RW\n";


    inline int main()
    {
        auto cave_map = parse_caves(data);

        auto part1 = part_1(cave_map);

        if (part1 != 226)
        {
            std::format_to(std::ostream_iterator<char>(std::cerr),
                "Expected 226, had {}", part1);
            return EXIT_FAILURE;
        }

        auto part2 = part_2(cave_map);

        if (part2 != 3509)
        {
            std::format_to(std::ostream_iterator<char>(std::cerr),
                "Expected 3509, had {}", part2);
            return EXIT_FAILURE;
        }



        return EXIT_SUCCESS;
    }
}


int main()
{
    return dooc::main();
}
