
#include <format>
#include <iostream>
#include <cstdlib>
#include <numeric>
#include <string_view>
#include <vector>

#include <dec9.hpp>

#include <cassert>

namespace dooc
{

    constexpr std::string_view data =
        "2199943210\n"
        "3987894921\n"
        "9856789892\n"
        "8767896789\n"
        "9899965678\n"
        "9999999999\n";


    inline int main()
    {
        auto height_map = map_height(data);
        auto risk = risk_level(height_map);
        if (risk.first != 15)
        {
            std::format_to(std::ostream_iterator<char>(std::cerr),
                "Expected 15, had {}", risk.first);
            return EXIT_FAILURE;
        }
        if (risk.second != 1134)
        {
            std::format_to(std::ostream_iterator<char>(std::cerr),
                "Expected 1134, had {}", risk.second);
            return EXIT_FAILURE;
        }


        return EXIT_SUCCESS;
    }
}


int main()
{
    return dooc::main();
}
