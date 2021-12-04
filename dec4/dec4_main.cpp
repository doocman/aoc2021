
#include <array>
#include <vector>

#include <dec4.hpp>
#include <dec4_data.hpp>

#include <iostream>

namespace dooc
{
    int main_dec4()
    {
        boards_t boards(std::move(aoc2021::dec4::raw_boards));
        auto result = announce_bingo_numbers(boards, aoc2021::dec4::numbers);
        auto win_prod = dooc::retrieve_winning_product(result.winning_boards.front().first.front(), result.winning_boards.front().second);
        std::cout << "First Win product: " << win_prod << ".\n";

        std::cout << "Last win count : " << std::ssize(result.winning_boards.back().first) << '\n';

        auto last_win_prod = dooc::retrieve_winning_product(result.winning_boards.back().first.front(), result.winning_boards.back().second);
        std::cout << "Last Win product: " << last_win_prod << ".\n";
        return EXIT_SUCCESS;
    }
}

int main()
{
    return dooc::main_dec4();
}

