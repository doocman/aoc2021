

#include <iostream>
#include <format>
#include <fstream>
#include <vector>
#include <sstream>
#include <numeric>

#include <dec6.hpp>
#include <dooc/views.hpp>

namespace dooc
{
    inline int main(int argc, char** argv)
    {
        if (argc == 1)
        {
            std::exit(EXIT_SUCCESS);
        }
        if (argc > 2)
        {
            std::cerr << "Too many arguments\n";
            std::exit(EXIT_FAILURE);
        }

        std::ifstream input(argv[1]);
        if (!input.is_open())
        {
            std::format_to(std::ostream_iterator<char>(std::cerr),
                "Failed to open file '{}'", argv[1]);
            std::exit(EXIT_FAILURE);
        }

        std::stringstream buffer;
        buffer << input.rdbuf();


        auto fish_v = fish_from_string(buffer.view());
        fish_v = pass_fish_time(std::move(fish_v), 80);
        auto fish_count = std::reduce(begin(fish_v), end(fish_v));

        fish_v = pass_fish_time(std::move(fish_v), 256 - 80);
        auto fish_count_256 = std::reduce(begin(fish_v), end(fish_v));

        std::cout << "Fish count: " << fish_count << ".\n";
        std::cout << "Fish count after 256d: " << fish_count_256 << ".\n";

        return EXIT_SUCCESS;
    }
    
}

int main(int argc, char** argv)
{
    return dooc::main(argc, argv);
}

