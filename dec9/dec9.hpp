
#pragma once

#include <algorithm>
#include <numeric>
#include <numbers>
#include <vector>
#include <string_view>
#include <unordered_set>
#include <ranges>
#include <cassert>
#include <map>

#include <dooc/views.hpp>

namespace dooc
{

    struct height_map_from_str
    {

        constexpr std::pair<std::string_view, std::optional<std::string_view>> operator()(std::string_view to_extract)
        {
            if (std::empty(to_extract)) return { {}, std::nullopt };
            auto to_return = to_extract.substr(0, to_extract.find('\n'));
            to_extract.remove_prefix(std::min(size(to_return) + 1, size(to_extract)));
            return { to_extract, to_return };
        }
    };

    inline std::vector<std::string_view> map_height(std::string_view in)
    {
        std::vector<std::string_view> res;
        res.reserve(std::count(begin(in), end(in), '\n') + 1);
        auto beg = from_string_base_it(in, height_map_from_str());
        auto end_it = from_string_base_it(in, end(in), height_map_from_str());
        std::copy(beg, end_it, std::back_inserter(res));
        return res;
    }

    template <typename TVIt, typename THIt>
    inline std::optional<std::pair<TVIt, THIt>> step_pos(TVIt v_beg, TVIt v_end, TVIt v_cur, THIt h_cur)
    {
        auto low_point = std::abs(*h_cur);
        std::optional<std::pair<TVIt, THIt>> res = std::nullopt;
        if (h_cur != begin(*v_cur))
        {
            auto next_h = prev(h_cur);
            auto pot_low = std::abs(*next_h);
            if (pot_low < low_point || (pot_low == low_point && *next_h < 0))
            {
                low_point = pot_low;
                res = std::pair{ v_cur, next_h };
            }
        }
        if (next(h_cur) != end(*v_cur))
        {
            auto next_h = next(h_cur);
            auto pot_low = std::abs(*next_h);
            if (pot_low < low_point || (pot_low == low_point && *next_h < 0))
            {
                low_point = pot_low;
                res = std::pair{ v_cur, next_h };
            }
        }
        if (v_cur != v_beg)
        {
            auto next_v = prev(v_cur);
            auto next_h = begin(*next_v) + std::distance(begin(*v_cur), h_cur);
            auto pot_low = std::abs(*next_h);
            if (pot_low < low_point || (pot_low == low_point && *next_h < 0))
            {
                low_point = pot_low;
                res = std::pair{ next_v, next_h };
            }
        }
        if (next(v_cur) != v_end)
        {
            auto next_v = next(v_cur);
            auto next_h = begin(*next_v) + std::distance(begin(*v_cur), h_cur);
            auto pot_low = std::abs(*next_h);
            if (pot_low < low_point || (pot_low == low_point && *next_h < 0))
            {
                low_point = pot_low;
                res = std::pair{ next_v, next_h };
            }
        }
        return res;
    }

    inline std::pair<int, int> risk_level(std::vector<std::string_view> const& init_h_map)
    {
        std::vector<std::vector<signed char>> h_map;
        std::map<signed char*, int> basin_map;
        std::array<signed char*, 3> largest_basins;
        h_map.reserve(size(init_h_map));
        std::transform(begin(init_h_map), end(init_h_map), std::back_inserter(h_map),
            [](std::string_view in_str)
            {
                std::vector<signed char> res;
                res.reserve(size(in_str));
                std::transform(begin(in_str), end(in_str), std::back_inserter(res), [](char c) { return static_cast<signed char>(c - '0') + 1; });
                return res;
            });
        auto v_beg = begin(h_map);
        auto const v_end = end(h_map);
        int low_points_sum{};
        while (v_beg != v_end)
        {
            auto const h_end = end(*v_beg);
            auto h_beg = std::find_if(begin(*v_beg), h_end, [](signed char c) { return c > 0; });
            while(h_beg != h_end)
            {
                auto cur_v_pos = v_beg;
                auto cur_h_pos = h_beg;
                bool do_break{};
                int basin_size = 0;
                while (!do_break)
                {
                    basin_size += (*cur_h_pos != 10);
                    auto cur_value = *cur_h_pos;
                    auto opt_step = step_pos(begin(h_map), v_end, cur_v_pos, cur_h_pos);
                    *cur_h_pos *= -1;
                    if (!opt_step)
                    {
                        low_points_sum += cur_value;
                        do_break = true;
                    }
                    else
                    {
                        auto [next_v, next_h] = *opt_step;
                        do_break = (*next_h < 0);
                        cur_v_pos = next_v;
                        cur_h_pos = next_h;
                    }
                }
                do_break = basin_size == 0;
                while (!do_break)
                {
                    auto opt_step = step_pos(begin(h_map), v_end, cur_v_pos, cur_h_pos);
                    if (!opt_step)
                    {
                        do_break = true;
                        signed char* key = std::addressof(*cur_h_pos);
                        auto& cur_basin = basin_map[key];
                        cur_basin += basin_size;
                        if (std::find(begin(largest_basins), end(largest_basins), key) == end(largest_basins))
                        {
                            auto bas_repl = std::find_if(begin(largest_basins), end(largest_basins), [&basin_map, cur_basin](auto key)
                                {
                                    return key == nullptr || basin_map[key] <= cur_basin;
                                }
                            );
                            if (bas_repl != end(largest_basins))
                            {
                                std::rotate(bas_repl, end(largest_basins), end(largest_basins));
                                *bas_repl = std::addressof(*cur_h_pos);
                            }
                        }
                    }
                    else
                    {
                        auto [next_v, next_h] = *opt_step;
                        cur_v_pos = next_v;
                        cur_h_pos = next_h;
                    }
                }
                h_beg = std::find_if(h_beg, h_end, [](signed char c) { return c > 0; });
            }
            std::advance(v_beg, 1);
        }
        std::cout << std::ssize(basin_map) << ".\n";
        return { low_points_sum, std::transform_reduce(begin(largest_basins), end(largest_basins), 1, std::multiplies(),
            [&basin_map](auto it) { return basin_map[it]; }) };
    }
}
