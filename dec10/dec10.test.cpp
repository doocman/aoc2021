
#include <format>
#include <iostream>
#include <cstdlib>
#include <numeric>
#include <string_view>
#include <vector>

#include <dec10.hpp>

#include <cassert>

namespace dooc
{

    constexpr std::string_view data =
        "[({(<(())[]>[[{[]{<()<>>\n"
        "[(()[<>])]({[<{<<[]>>(\n"
        "{([(<{}[<>[]}>{[]{[(<()>\n"
        "(((({<>}<{<{<>}{[]{[]{}\n"
        "[[<[([]))<([[{}[[()]]]\n"
        "[{[{({}]{}}([{[{{{}}([]\n"
        "{<[[]]>}<{[{[{[]{()[[[]\n"
        "[<(<(<(<{}))><([]([]()\n"
        "<{([([[(<>()){}]>(<<{{\n"
        "<{([{{}}[<[[[<>{}]]]>[]]";


    inline int main()
    {
        auto points = calculate_all_chunks(data);
        if (points.first != 26397)
        {
            std::format_to(std::ostream_iterator<char>(std::cerr),
                "Expected 26397, had {}", points.first);
            return EXIT_FAILURE;
        }

        std::string_view incomplete_line1 = "[({(<(())[]>[[{[]{<()<>>";
        auto [unus1, compl_scr1] = calculate_chunk_error_points(incomplete_line1);
        assert(288957 == compl_scr1);

        std::string_view incomplete_line2 = "[(()[<>])]({[<{<<[]>>(";
        auto [unus2, compl_scr2] = calculate_chunk_error_points(incomplete_line2);
        assert(5566 == compl_scr2);

        if (points.second != 288957)
        {
            std::format_to(std::ostream_iterator<char>(std::cerr),
                "Expected 288957, had {}", points.second);
            return EXIT_FAILURE;
        }



        return EXIT_SUCCESS;
    }
}


int main()
{
    return dooc::main();
}
