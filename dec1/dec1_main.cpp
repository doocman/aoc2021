
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <format>
#include <numeric>
#include <span>
#include <vector>
#include <ranges>

#include <dooc/views.hpp>

namespace
{
    void print_usage()
    {
        std::format_to(std::ostream_iterator<char>(std::cout),
            "Increase and decrease counter.\n"
            "Reads simple newline - separated list of values and returns a counter for number of times incrementsand decrements are done.\n"
            "Usage: 'change_counter FILE [WINDOW_LENGTH (int)]'");
    }

    struct result_t
    {
        std::int64_t increments;
        std::int64_t total_elements;
    };

    inline constexpr result_t& operator+=(result_t& lhs, result_t rhs) noexcept
    {
        lhs.increments += rhs.increments;
        lhs.total_elements += rhs.total_elements;
        return lhs;
    }

    inline constexpr result_t operator+(result_t lhs, result_t rhs) noexcept
    {
        result_t res = lhs;
        res += rhs;
        return res;
    }

    void print_result(result_t res)
    {
        std::format_to(std::ostream_iterator<char>(std::cout),
            "Values increased '{}' times.\nTotal elements: '{}'.\n", res.increments, res.total_elements);
    }
}

int main(int argc, char** argv)
{
    using namespace dooc;
    using std::begin;
    int window_length = 1;
    if (argc == 1)
    {
        print_usage();
        std::exit(EXIT_SUCCESS);
    }
    if (3 < argc)
    {
        std::cerr << "Too many arguments";
        print_usage();
        std::exit(EXIT_FAILURE);
    }
    if (argc == 3)
    {
        auto cur_arg_end = argv[2] + std::strlen(argv[2]);
        auto conv_res = std::from_chars(argv[2], cur_arg_end, window_length);
        if (conv_res.ec != std::errc())
        {
            std::format_to(std::ostream_iterator<char>(std::cerr),
                "Failed to parse window length argument '{}'", argv[2]);
            std::exit(EXIT_FAILURE);
        }
        if (window_length < 1)
        {
            std::format_to(std::ostream_iterator<char>(std::cerr),
                "Invalid value '{}'", argv[2]);
            std::exit(EXIT_FAILURE);
        }
    }
    std::ifstream input(argv[1]);
    if (!input.is_open())
    {
        std::format_to(std::ostream_iterator<char>(std::cerr),
            "Failed to open file '{}'", argv[1]);
        std::exit(EXIT_FAILURE);
    }

    std::vector<std::int64_t> data;

    {
        using input_it_t = std::istream_iterator<std::int64_t>;
        input_it_t in_it(input);
        std::copy(in_it, input_it_t(), std::back_inserter(data));
    }

    if (std::ssize(data) <= window_length)
    {
        print_result({ 0, 1 });
        std::exit(EXIT_SUCCESS);
    }

    {
        auto data_bunch = data | bunch_view(window_length);
        auto data_end = std::transform(begin(data_bunch), end(data_bunch), begin(data), [](auto window) { return std::reduce(begin(window), end(window), std::int64_t()); });
        data.erase(data_end, end(data));
    }

    auto result = std::transform_reduce(std::next(begin(data)), end(data), begin(data), result_t(0, 1), std::plus(), [](auto lhs, auto rhs)
        {
            int did_increment = lhs > rhs;
            return result_t{ did_increment, 1 };
        });

    print_result(result);

    std::exit(EXIT_SUCCESS);
}
