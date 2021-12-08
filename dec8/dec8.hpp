
#pragma once

#include <algorithm>
#include <numbers>
#include <vector>
#include <string_view>
#include <unordered_set>
#include <ranges>
#include <cassert>
#include <unordered_map>

#include <dooc/views.hpp>

namespace dooc
{
    struct seven_segments_display
    {
        std::array<std::string_view, 10> input;
        std::array<std::string_view, 4> output;
    };

    struct seven_segments_display_from_str
    {
        using delims_t = default_str_delims<char>;
        delims_t delims_;

        constexpr std::pair<std::string_view, std::optional<seven_segments_display>> operator()(std::string_view to_extract)
        {
            seven_segments_display to_return;
            if (std::empty(to_extract)) return { {}, std::nullopt };
            for (auto i_it = begin(to_return.input); auto i : std::views::iota(0, std::ssize(to_return.input)))
            {
                auto [cur_i, next_extr] = delims_(to_extract);
                *i_it = cur_i;
                to_extract = next_extr;
                ++i_it;
            }
            to_extract.remove_prefix(to_extract.find_first_not_of("| "));
            for (auto o_it = begin(to_return.output); auto i : std::views::iota(0, std::ssize(to_return.output)))
            {
                auto [cur_o, next_extr] = delims_(to_extract);
                *o_it = cur_o;
                to_extract = next_extr;
                ++o_it;
            }
            return { to_extract, to_return };
        }
    };

    inline std::vector<seven_segments_display> digits_from_string(std::string_view in)
    {
        std::vector<seven_segments_display> res;
        res.reserve(std::ssize(in) / 14);
        auto beg = from_string_base_it(in, seven_segments_display_from_str());
        auto end_it = from_string_base_it(in, end(in), seven_segments_display_from_str());
        std::copy(beg, end_it, std::back_inserter(res));
        return res;
    }

    inline int count_numbers(std::span<const seven_segments_display> in)
    {
        return std::transform_reduce(begin(in), end(in), 0, std::plus(), [](seven_segments_display const & in)
            {
                return std::transform_reduce(begin(in.output), end(in.output), 0, std::plus(), []
                (std::string_view in)
                    {
                        constexpr std::array easy_sizes = { 2,3,4,7 };
                        return std::any_of(begin(easy_sizes), end(easy_sizes), [in](int s) { return std::ssize(in) == s ? 1 : 0; });
                    });
            });
    }

    inline auto extract_disp(auto& display, auto&& func)
    {
        auto it = std::partition(begin(display), end(display), [&func](auto const& i) {return !func(i); });
        auto res = *it;
        display = std::span(begin(display), it);
        return res;
    }

    inline auto extract_num(auto& lines, auto& to_convert)
    {
        auto to_return = std::distance(begin(lines), std::find_if(begin(lines), end(lines), [&to_convert](auto& l_v)
            { return std::ssize(l_v) == std::ssize(to_convert) && std::is_permutation(begin(l_v), end(l_v), begin(to_convert)); }));
        assert(to_return < 10);
        return to_return;
    }

    inline int sum_numbers(seven_segments_display display)
    {
        std::array<std::string_view, 10> lines;
        std::span<std::string_view, std::dynamic_extent> disp_in = display.input;
        lines[1] = extract_disp(disp_in, [](auto const& v) { return std::ssize(v) == 2; });
        lines[4] = extract_disp(disp_in, [](auto const& v) { return std::ssize(v) == 4; });
        lines[7] = extract_disp(disp_in, [](auto const& v) { return std::ssize(v) == 3; });
        lines[8] = extract_disp(disp_in, [](auto const& v) { return std::ssize(v) == 7; });
        lines[9] = extract_disp(disp_in, [&lines](auto const& v)
            {
                if (std::ssize(v) == 6)
                {
                    return std::all_of(begin(lines[4]), end(lines[4]), [&v](char c) { return std::ranges::find(v, c) != end(v); });
                }
                return false;
            });
        lines[0] = extract_disp(disp_in, [&lines](auto const& v)
            {
                if (std::ssize(v) == 6)
                {
                    return std::all_of(begin(lines[1]), end(lines[1]), [&v](char c) { return std::ranges::find(v, c) != end(v); });
                }
                return false;
            });
        lines[6] = extract_disp(disp_in, [&lines](auto const& v)
            {
                return (std::ssize(v) == 6);
            });
        lines[5] = extract_disp(disp_in, [&lines](auto const& v)
            {
                if (std::ssize(v) == 5)
                {
                    return std::all_of(begin(v), end(v), [&lines](char c) { return std::ranges::find(lines[6], c) != end(lines[6]); });
                }
                return false;
            });
        lines[3] = extract_disp(disp_in, [&lines](auto const& v)
            {
                if (std::ssize(v) == 5)
                {
                    return std::all_of(begin(v), end(v), [&lines](char c) { return std::ranges::find(lines[9], c) != end(lines[9]); });
                }
                return false;
            });
        lines[2] = disp_in.front();


        return extract_num(lines, display.output[0]) * 1000
            + extract_num(lines, display.output[1]) * 100
            + extract_num(lines, display.output[2]) * 10
            + extract_num(lines, display.output[3]);
    }
}
