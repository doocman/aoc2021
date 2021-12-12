

#include <iostream>
#include <format>
#include <fstream>
#include <vector>
#include <sstream>
#include <numeric>

#include <dec12.hpp>
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

        auto cave_map = parse_caves(buffer.view());

        std::cout << "Part 1: " << part_1(cave_map) << ".\n";
        std::cout << "Part 2: " << part_2(cave_map) << ".\n";

        return EXIT_SUCCESS;
    }
    
}

int main(int argc, char** argv)
{
    return dooc::main(argc, argv);
}

