
#include <format>
#include <iostream>
#include <cstdlib>
#include <string_view>

#include <dec2.hpp>

constexpr std::string_view data =
"forward 5\n"
"down 5   \n"
"forward 8\n"
"up 3     \n"
"down 8   \n"
"forward 2\n";

int main()
{
    dooc::pos final_pos = dooc::process_movement(data, dooc::pos(), false);

    if (final_pos.horizontal != 15 && final_pos.vertical != 10)
    {

        std::format_to(std::ostream_iterator<char>(std::cerr),
            "Final [horizontal, vertical] was not [15, 20] but ['{}', '{}']", final_pos.horizontal, final_pos.vertical);
        return EXIT_FAILURE;
    }

    final_pos = dooc::process_movement(data, dooc::pos(), true);

    if (final_pos.horizontal != 15 && final_pos.vertical != 60)
    {

        std::format_to(std::ostream_iterator<char>(std::cerr),
            "Final [horizontal, vertical] with aim was not [15, 60] but ['{}', '{}']", final_pos.horizontal, final_pos.vertical);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
