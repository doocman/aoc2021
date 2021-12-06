
#pragma once

#include <algorithm>
#include <vector>
#include <string_view>

#include <dooc/views.hpp>

namespace dooc
{
    using fish_count_t = std::int64_t;

    inline std::vector<fish_count_t> fish_from_string(std::string_view in)
    {
        std::vector<fish_count_t> res(9);
        auto beg = int_from_string_it<fish_count_t>(in);
        auto end_it = int_from_string_it<fish_count_t>(in, end(in));
        std::for_each(beg, end_it, [&res](fish_count_t c) { ++res[c]; });
        return res;
    }

    inline std::vector<fish_count_t> pass_fish_time(std::vector<fish_count_t> in, int days)
    {
        for (auto i : std::views::iota(0, days))
        {
            std::rotate(begin(in), next(begin(in)), end(in));
            in[6] += in.back();
        }
        return in;
    }
}
