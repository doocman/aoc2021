
#include <format>
#include <iostream>
#include <cstdlib>
#include <numeric>
#include <string_view>
#include <vector>

#include <dec8.hpp>

#include <cassert>

namespace dooc
{

    constexpr std::string_view data =
        "be cfbegad cbdgef fgaecd cgeb fdcge agebfd fecdb fabcd edb |"
        "fdgacbe cefdb cefbgd gcbe                                   "
        "edbfga begcd cbg gc gcadebf fbgde acbgfd abcde gfcbed gfec |"
        "fcgedb cgb dgebacf gc                                       "
        "fgaebd cg bdaec gdafb agbcfd gdcbef bgcad gfac gcb cdgabef |"
        "cg cg fdcagb cbg                                            "
        "fbegcd cbd adcefb dageb afcb bc aefdc ecdab fgdeca fcdbega |"
        "efabcd cedba gadfec cb                                      "
        "aecbfdg fbg gf bafeg dbefa fcge gcbea fcaegb dgceab fcbdga |"
        "gecf egdcabf bgf bfgea                                      "
        "fgeab ca afcebg bdacfeg cfaedg gcfdb baec bfadeg bafgc acf |"
        "gebdcfa ecba ca fadegcb                                     "
        "dbcfg fgd bdegcaf fgec aegbdf ecdfab fbedc dacgb gdcebf gf | "
        "cefg dcbef fcge gbcadfe                                     "
        "bdfegc cbegaf gecbf dfcage bdacg ed bedf ced adcbefg gebcd |"
        "ed bcgafe cdgba cbgef                                       "
        "egadfb cdbfeg cegd fecab cgb gbdefca cg fgcdab egfdb bfceg | "
        "gbdfcae bgc cg cgb                                          "
        "gcafb gcf dcaebfg ecagb gf abcdeg gaef cafbge fdbac fegbdc |"
        "fgae cfgab fg bagce";


    inline int main()
    {
        auto input = digits_from_string(data);
        auto number_count = count_numbers(input);
        if (number_count != 26)
        {
            std::format_to(std::ostream_iterator<char>(std::cerr),
                "Expected 26, had {}", number_count);
            return EXIT_FAILURE;
        }
        auto sum = std::transform_reduce(begin(input), end(input), 0, std::plus(), [](auto const& d)
            {
                return sum_numbers(d);
            });
        if (sum != 61229)
        {
            std::format_to(std::ostream_iterator<char>(std::cerr),
                "Expected 61229, had {}", sum);
            return EXIT_FAILURE;
        }


        return EXIT_SUCCESS;
    }
}


int main()
{
    return dooc::main();
}
