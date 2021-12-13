
#include <format>
#include <iostream>
#include <cstdlib>
#include <numeric>
#include <string_view>
#include <vector>

#include <dec13.hpp>

#include <cassert>

namespace dooc
{

    constexpr std::string_view data =
        "6,10\n"
"0,14\n"
"9,10\n"
"0,3\n"
"10,4\n"
"4,11\n"
"6,0\n"
"6,12\n"
"4,1\n"
"0,13\n"
"10,12\n"
"3,4\n"
"3,0\n"
"8,4\n"
"1,10\n"
"2,14\n"
"8,10\n"
"9,0\n"
"\n"
"fold along y=7\n"
"fold along x=5";


    inline int main()
    {
        auto dot_map = parse_dot_map(data);

        auto fold_once = pop_fold(dot_map);

        auto dot_count_p1 = std::ssize(fold_once.dots);

        if (dot_count_p1 != 17)
        {
            std::format_to(std::ostream_iterator<char>(std::cerr),
                "Expected 17, had {}", dot_count_p1);
            return EXIT_FAILURE;
        }

        dot_map = fold_all(std::move(dot_map));

        format_to(dot_map.dots, std::cout);


        return EXIT_SUCCESS;
    }
}


int main()
{
    return dooc::main();
}
