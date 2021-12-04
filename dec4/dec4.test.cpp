
#include <array>
#include <format>
#include <iostream>
#include <cstdlib>
#include <string_view>
#include <sstream>

#include <dec4.hpp>

#include <cassert>

namespace dooc
{

    constexpr std::array numbers{ 7, 4, 9, 5, 11, 17, 23, 2, 0, 14, 21, 24, 10, 16, 13, 6, 15, 25, 12, 22, 18, 20, 8, 19, 3, 26, 1 };
constexpr std::string_view board_str =
"22 13 17 11  0 "
"8  2 23  4 24  "
"21  9 14 16  7 "
"6 10  3 18  5  "
"1 12 20 15 19  "

"3 15  0  2 22  "
"9 18 13 17  5  "
"19  8  7 25 23 "
"20 11 10 24  4 "
"14 21 16 12  6 "

"14 21 17 24  4 "
"10 16 15  9 19 "
"18  8 23 26 20 "
"22 11 13  6  5 "
"2  0 12  3  7 ";

int main_dec4()
{
    using std::ssize;
    std::vector<int> data;
    data.reserve(size(board_str) / 2); // Arbitrary, but it should be safe to assume atleast a factor of 2 smaller.
    {
        from_string_base_it it(board_str, int());
        from_string_base_it end_it(board_str, end(board_str), int());
        std::copy(it, end_it, std::back_inserter(data));
    }

    boards_t boards(std::move(data));



    auto result = dooc::announce_bingo_numbers(boards, numbers);

    assert(!empty(result.winning_boards));
    auto& first_win = result.winning_boards.front();
    assert(!empty(first_win.first));
    auto win_prod = dooc::retrieve_winning_product(first_win.first.front(), first_win.second);

    if (win_prod != 4512)
    {
            std::format_to(std::ostream_iterator<char>(std::cerr),
                "Expected win_prod = 4512 but was {}", win_prod);
            return EXIT_FAILURE;
    }

    auto& last_win = result.winning_boards.back();
    assert(!empty(last_win.first));
    auto last_win_prod = dooc::retrieve_winning_product(last_win.first.front(), last_win.second);

    if (last_win_prod != 1924)
    {
        std::format_to(std::ostream_iterator<char>(std::cerr),
            "Expected win_prod = 1924 but was {}", last_win_prod);
        return EXIT_FAILURE;
    }

    std::array test_column =
    {
        0,0,0,0,-1,
        0,0,0,0,-1,
        0,0,0,0,-1,
        0,0,0,0,-1,
        0,0,0,0,-1
    };

    if (!is_board_finished<int>(std::span(test_column)))
    {

        return EXIT_FAILURE;
    }


    //if (boards.size() != 3)
    //{
    //    std::format_to(std::ostream_iterator<char>(std::cerr),
    //        "Expected size = 5 but was {}", ssize(boards));
    //    return EXIT_FAILURE;
    //}
    return EXIT_SUCCESS;
}
}

int main()
{
    return dooc::main_dec4();
}
