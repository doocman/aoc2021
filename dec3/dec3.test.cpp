
#include <format>
#include <iostream>
#include <cstdlib>
#include <string_view>

#include <dec3.hpp>

constexpr std::string_view data =
"00100"
"11110"
"10110"
"10111"
"10101"
"01111"
"00111"
"11100"
"10000"
"11001"
"00010"
"01010";

int main()
{
    dooc::gamma_eps res = dooc::create_gamma_eps(data, 5);

    if (res.gamma != 22 || res.eps != 9)
    {
        std::format_to(std::ostream_iterator<char>(std::cerr),
            "Expected [{}, {}] but received [{}, {}]", 22, 9, res.gamma, res.eps);
        return EXIT_FAILURE;
    }

    dooc::oxy_co2 res2 = dooc::create_oxy_co2(std::string(data), 5);

    if (res2.oxygen_gen != 23 || res2.co2_scrub != 10)
    {
        std::format_to(std::ostream_iterator<char>(std::cerr),
            "Expected [{}, {}] but received [{}, {}]", 23, 10, res2.oxygen_gen, res2.co2_scrub);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
