
#include <format>
#include <iostream>
#include <cstdlib>
#include <string_view>
#include <fstream>
#include <vector>

#include <dec3.hpp>

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
    if (argc > 2)
    {
        std::cerr << "Too many arguments\n";
        print_usage();
        std::exit(EXIT_FAILURE);
    }

    std::ifstream input(argv[1]);
    if (!input.is_open())
    {
        std::format_to(std::ostream_iterator<char>(std::cerr),
            "Failed to open file '{}'", argv[1]);
        std::exit(EXIT_FAILURE);
    }

    std::vector<char> buffer;
    constexpr std::string_view delims = " \n\0";
    while (std::none_of(begin(delims), end(delims), [&input](char c) { return c == input.peek(); }))
    {
        buffer.push_back(input.get());
    }
    
    int bit_width = buffer.size();

    using in_it = std::istream_iterator<char>;
    std::copy_if(in_it(input), in_it(), std::back_inserter(buffer), [delims](char c) { return std::none_of(begin(delims), end(delims), [c](char cd) {return cd == c; }); });

    auto res = dooc::create_gamma_eps({ buffer.data(), buffer.size() }, bit_width);

    std::format_to(std::ostream_iterator<char>(std::cout),
        "Final result [{1:0{0}b}, {2:0{0}b}] = {3}.\n", bit_width, res.gamma, res.eps, res.gamma * res.eps);

    auto ox_co = dooc::create_oxy_co2({ buffer.data(), buffer.size() }, bit_width);

    std::format_to(std::ostream_iterator<char>(std::cout),
        "Final result oxygen gen && co2 [{1:0{0}b}, {2:0{0}b}] = {3}.\n", bit_width, ox_co.oxygen_gen, ox_co.co2_scrub, ox_co.oxygen_gen * ox_co.co2_scrub);
    return EXIT_SUCCESS;
}

