
#include <array>
#include <format>
#include <iostream>
#include <cstdlib>
#include <string_view>
#include <vector>

#include <dec5.hpp>

#include <cassert>

namespace dooc
{

    constexpr std::string_view data =
        "0, 9 -> 5, 9\n"
        "8, 0 ->0, 8\n"
        "9, 4 -> 3, 4\n"
        "2, 2 -> 2, 1\n"
        "7, 0 -> 7, 4\n"
        "6, 4 -> 2, 0\n"
        "0, 9 -> 2, 9\n"
        "3, 4 -> 1, 4\n"
        "0, 0 -> 8, 8\n"
        "5, 5 -> 8, 2\n ";


    inline int main()
    {
        std::vector<vent_line> vents;
        vents.reserve(10);
        convert_to_vents(data, std::back_inserter(vents));
        auto h_v_count = count_v_h_intersections(std::move(vents));

        if (h_v_count.first != 5)
        {
            std::format_to(std::ostream_iterator<char>(std::cerr),
                "Expected 5, but had {}", h_v_count.first);
            return EXIT_FAILURE;
        }

        if (h_v_count.second != 12)
        {
            std::format_to(std::ostream_iterator<char>(std::cerr),
                "Expected 12 (with diag), but had {}", h_v_count.second);
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }
}


int main()
{
    return dooc::main();
}
