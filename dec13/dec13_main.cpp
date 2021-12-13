

#include <iostream>
#include <format>
#include <fstream>
#include <vector>
#include <sstream>
#include <numeric>

#include <dec13.hpp>
#include <dooc/views.hpp>

namespace dooc
{
    inline int main(int argc, char** argv)
    {
        if (argc == 1)
        {
            std::cerr << "Missing argument 'data file'\n";
            std::exit(EXIT_FAILURE);
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

        auto dot_map = parse_dot_map(buffer.view());

        auto fold_once = pop_fold(dot_map);

        auto dot_count_p1 = std::ssize(fold_once.dots);

        std::cout << "part 1: " << dot_count_p1 << ".\n";

        dot_map = fold_all(std::move(dot_map));

        format_to(dot_map.dots, std::cout);

        return EXIT_SUCCESS;
    }
    
}

int main(int argc, char** argv)
{
    return dooc::main(argc, argv);
}

