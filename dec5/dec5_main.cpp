

#include <iostream>
#include <format>
#include <fstream>
#include <vector>
#include <sstream>

#include <dooc/views.hpp>
#include <dec5.hpp>

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

        std::vector<vent_line> vents;
        convert_to_vents(buffer.view(), std::back_inserter(vents));
        auto h_v_count = count_v_h_intersections(std::move(vents));

        std::cout << "Vent intersections: " << h_v_count.first << ".\n";
        std::cout << "Vent intersections with diags: " << h_v_count.second << ".\n";

        return EXIT_SUCCESS;
    }
    
}

int main(int argc, char** argv)
{
    return dooc::main(argc, argv);
}

