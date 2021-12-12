
#pragma once

#include <algorithm>
#include <string_view>
#include <vector>
#include <ranges>
#include <string_view>
#include <variant>
#include <unordered_map>
#include <optional>

namespace dooc
{

    template <typename TDer>
    struct cave
    {
        std::string_view name;
        constexpr explicit cave(std::string_view name_in)
            : name(name_in)
        {}
    };

    struct small_cave : cave<small_cave>
    {
        explicit constexpr small_cave(std::string_view name)
            : cave(name)
        {}
    };

    constexpr int explore(small_cave const& c, auto const& map, auto& small_caves_visited, std::optional<std::string_view> tw_vis, auto&& continue_f)
    {
        if (std::ranges::any_of(small_caves_visited, [&c](std::string_view str) { return str == c.name; }))
        {
            if (tw_vis && empty(*tw_vis))
            {
                *tw_vis = c.name;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            small_caves_visited.push_back(c.name);
        }
        return continue_f(map, small_caves_visited, tw_vis);
    }

    struct large_cave : cave<large_cave>
    {
        explicit constexpr large_cave(std::string_view name)
            : cave(name)
        {}
    };

    constexpr int explore(large_cave const&, auto const& map, auto& small_caves_visited, std::optional<std::string_view> tw_vis, auto&& continue_f)
    {
        return continue_f(map, small_caves_visited, tw_vis);
    }

    struct start_cave : cave<start_cave>
    {
        constexpr start_cave()
            : cave("start")
        {}
    };

    constexpr int explore(start_cave const&, auto const& map, auto& small_caves_visited, std::optional<std::string_view> tw_vis, auto&& continue_f)
    {
        return continue_f(map, small_caves_visited, tw_vis);
    }

    struct end_cave : cave<end_cave>
    {
        constexpr end_cave()
            : cave("end")
        {}
    };

    constexpr int explore(end_cave const&, auto const&, auto const&, std::optional<std::string_view>, auto&&)
    {
        return 1;
    }

    using cave_variant = std::variant<start_cave, end_cave, small_cave, large_cave>;

    struct cave_map_entry;
    using cave_map = std::unordered_map<std::string_view, cave_map_entry>;

    struct cave_map_entry
    {
        cave_variant this_cave;
        std::vector<std::string_view> connections;

        constexpr explicit cave_map_entry(cave_variant cur)
            : this_cave(cur)
        {}

        constexpr int count_part1(cave_map const& map, std::vector<std::string_view>& small_cave_visited, std::optional<std::string_view> tw_vis) const
        {
            return std::visit(
                [&small_cave_visited, this, &map, &tw_vis] <typename T> (T const& c)
            {
                return explore(c, map, small_cave_visited, tw_vis, 
                    [this](cave_map const& map, std::vector<std::string_view>& c_vis, std::optional<std::string_view> tw_vis)
                    {
                        auto cur_vis = size(c_vis);
                        return std::transform_reduce(begin(connections), end(connections), 0, std::plus(),
                            [&c_vis, cur_vis, &map, tw_vis](std::string_view e)
                            {
                                auto to_return = map.at(e).count_part1(map, c_vis, tw_vis);
                                c_vis.resize(cur_vis);
                                return to_return;
                            });
                    });
            }, this_cave
            );
            

        }
    };

    constexpr cave_variant cave_from_str(std::string_view str)
    {
        using namespace std::string_view_literals;
        if (str == "start"sv) return start_cave();
        if (str == "end"sv) return end_cave();
        if (std::isupper(str.front())) return large_cave(str);
        return small_cave(str);
    }

    inline auto parse_caves(std::string_view data)
    {
        std::unordered_map<std::string_view, cave_map_entry> caves;
        while (!data.empty())
        {
            using namespace std::string_view_literals;
            constexpr std::string_view delims = " \n-";
            auto lhs = data.substr(0, data.find_first_of(delims));
            data = data.substr(data.find_first_not_of(delims, ssize(lhs)));
            auto rhs = data.substr(0, data.find_first_of(delims));
            data = data.substr(std::min(data.find_first_not_of(delims, ssize(rhs)), size(data)));
            auto lhs_it = caves.find(lhs);
            if (lhs_it == end(caves)) lhs_it = caves.emplace_hint(lhs_it, lhs, cave_from_str(lhs) );
            auto rhs_it = caves.find(rhs);
            if (rhs_it == end(caves)) rhs_it = caves.emplace_hint(rhs_it, rhs, cave_from_str(rhs) );
            if (rhs != "start"sv) lhs_it->second.connections.push_back(rhs);
            if (lhs != "start"sv) rhs_it->second.connections.push_back(lhs);
            auto next_data_start = ssize(lhs) + ssize(rhs) + 1;
        }
        return caves;
    }

    inline auto part_1(cave_map const& map, std::optional<std::string_view> part_2 = std::nullopt)
    {
        using namespace std::string_view_literals;
        auto start_it = map.find("start"sv);
        using namespace std::views;
        std::vector<std::string_view> small_caves_visited;
        small_caves_visited.reserve(size(map));
        
        auto tr = all(start_it->second.connections) | transform([&small_caves_visited, &map, part_2](auto name_cave)
            { 
                auto to_return = map.at(name_cave).count_part1(map, small_caves_visited, part_2);
                small_caves_visited.clear();
                return to_return;
            });
        return std::reduce(begin(tr), end(tr));
    }

    inline auto part_2(cave_map const& map)
    {
        return part_1(map, std::string_view());
    }
}
