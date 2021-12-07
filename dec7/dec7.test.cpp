
#include <format>
#include <iostream>
#include <cstdlib>
#include <numeric>
#include <string_view>
#include <vector>

#include <dec7.hpp>

#include <cassert>

namespace dooc
{

    constexpr std::string_view data =
        "16,1,2,0,4,2,7,1,2,14";


    inline int main()
    {
        auto crabs = crabs_from_string(data);
        auto crab_movement = move_crabs(crabs);
        if (crab_movement.first != 37)
        {
            std::format_to(std::ostream_iterator<char>(std::cerr),
                "Expected 37, but had {}", crab_movement.first);
            return EXIT_FAILURE;
        }
        if (crab_movement.second != 168)
        {
            std::format_to(std::ostream_iterator<char>(std::cerr),
                "Expected 168, but had {}", crab_movement.second);
            return EXIT_FAILURE;
        }


        return EXIT_SUCCESS;
    }
}


int main()
{
    return dooc::main();
}
