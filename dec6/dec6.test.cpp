
#include <format>
#include <iostream>
#include <cstdlib>
#include <numeric>
#include <string_view>
#include <vector>

#include <dec6.hpp>

#include <cassert>

namespace dooc
{

    constexpr std::string_view data =
        "3,4,3,1,2";


    inline int main()
    {
        auto fish_v = fish_from_string(data);
        fish_v = pass_fish_time(std::move(fish_v), 80);
        auto fish_count = std::reduce(begin(fish_v), end(fish_v));
        if (fish_count != 5934)
        {
            std::format_to(std::ostream_iterator<char>(std::cerr),
                "Expected 5934, but had {}", fish_count);
            return EXIT_FAILURE;
        }


        return EXIT_SUCCESS;
    }
}


int main()
{
    return dooc::main();
}
