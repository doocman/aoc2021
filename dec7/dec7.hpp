
#pragma once

#include <algorithm>
#include <numbers>
#include <vector>
#include <string_view>
#include <ranges>

#include <dooc/views.hpp>

namespace dooc
{

    inline std::vector<int> crabs_from_string(std::string_view in)
    {
        std::vector<int> res;
        res.reserve(std::ssize(in) / 2);
        auto beg = int_from_string_it<int>(in);
        auto end_it = int_from_string_it<int>(in, end(in));
        std::copy(beg, end_it, std::back_inserter(res));
        return res;
    }

    template <typename T>
    constexpr double sign_man(T in, T at_0 = T())
    {
        return in == T() ? at_0 : (in < T() ? -1 : 1);
    }

    inline std::pair<int, int> move_crabs(std::vector<int> in)
    {
        std::ranges::sort(in);
        auto median = in[std::ssize(in) / 2];
        auto init_mov = std::transform_reduce(begin(in), end(in), int(), std::plus(), [median](int c) { return std::abs(c - median); });
        double deri = 0., dmedian = median;
        do
        {
            dmedian -= deri;
            deri = std::transform_reduce(begin(in), end(in), 0., std::plus(),
                [dmedian](int i) {
                    double di = i - dmedian;
                    auto sign_i = sign_man(di);
                    auto to_return = -di - sign_i;
                    return to_return;
                }) / std::ssize(in);
        } while (std::lround(dmedian - deri) != std::lround(dmedian + deri));
        median = std::lround(dmedian);
        auto mov_part2 = std::transform_reduce(begin(in), end(in), int(), std::plus(), [median](int c)
            {
                c = std::abs(c - median);
                return c * (c + 1) / 2;
            });
        return { init_mov, mov_part2 };
    }
}
