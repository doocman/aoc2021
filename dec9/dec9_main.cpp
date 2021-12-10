

#include <iostream>
#include <format>
#include <fstream>
#include <vector>
#include <sstream>
#include <numeric>

#include <dec9.hpp>
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

        auto height_map = map_height(buffer.view());
        auto risk = risk_level(height_map);
        std::cout << "risk: " << risk.first << ".\n";
        std::cout << "basin product: " << risk.second << ".\n";

        return EXIT_SUCCESS;
    }
    
}

int main(int argc, char** argv)
{
    return dooc::main(argc, argv);
}

