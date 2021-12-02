
#include <format>
#include <iostream>
#include <cstdlib>
#include <string_view>
#include <fstream>
#include <vector>

#include <dec2.hpp>

namespace
{
    void print_usage()
    {
        std::format_to(std::ostream_iterator<char>(std::cout),
            "Parses movement.\n"
            "Reads simple newline - separated list of values and returns a counter for number of times incrementsand decrements are done.\n"
            "Usage: 'dec2_main FILE [WINDOW_LENGTH (int)]'");
    }
}

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        print_usage();
        std::exit(EXIT_SUCCESS);
    }
    if (argc > 3)
    {
        std::cerr << "Too many arguments\n";
        print_usage();
        std::exit(EXIT_FAILURE);
    }
    bool use_aim = false;
    if (argc == 3)
    {
        use_aim = true;
    }

    std::ifstream input(argv[1]);
    if (!input.is_open())
    {
        std::format_to(std::ostream_iterator<char>(std::cerr),
            "Failed to open file '{}'", argv[1]);
        std::exit(EXIT_FAILURE);
    }

    std::vector<char> buffer;
    buffer.resize(200);
    dooc::pos res{};
    while (!input.eof())
    {
        input.getline(buffer.data(), std::ssize(buffer));
        res = dooc::process_movement(buffer.data(), res, use_aim);
    }

    std::format_to(std::ostream_iterator<char>(std::cout),
        "Final position: [{}, {}, {}] (product: '{}')", res.horizontal, res.vertical, res.aim, res.horizontal * res.vertical);
}

