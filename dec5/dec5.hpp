
#pragma once

#include <optional>
#include <dooc/views.hpp>

namespace dooc
{
    struct coord
    {
        int x;
        int y;
    };

    constexpr auto coord_by_x(coord const& lhs, coord const& rhs) noexcept
    {
        return std::pair(lhs.x, lhs.y) <=> std::pair(rhs.x, rhs.y);
    }

    constexpr auto coord_by_y(coord const& lhs, coord const& rhs) noexcept
    {
        return std::pair(lhs.y, lhs.x) <=> std::pair(rhs.y, rhs.x);
    }

    constexpr bool operator==(coord const& l, coord const& r) noexcept
    {
        return l.x == r.x && l.y == r.y;
    }

    struct vent_line
    {
        coord begin, end;
    };

    //inline bool operator==(vent_line const&, vent_line const&) = default;

    struct vent_line_str_convert
    {
        using str_view = std::string_view;
    private:
        default_str_convert<int, char> int_conv_;

        static constexpr auto delims() noexcept
        {
            return default_str_convert<int, char>::delims();
        }

    public:

        constexpr std::pair<str_view, std::optional<vent_line>> operator()(str_view to_extract)
        {
            using namespace std::string_view_literals;
            vent_line result;
            auto cres = int_conv_(to_extract);
            if (!cres.second) return { cres.first, std::nullopt };
            result.begin.x = *cres.second;
            cres = int_conv_(cres.first);
            if (!cres.second) return { cres.first, std::nullopt };
            result.begin.y = *cres.second;
            if(!cres.first.starts_with("->"sv)) return { cres.first, std::nullopt };
            cres.first.remove_prefix(cres.first.find_first_not_of(delims(), 2));
            cres = int_conv_(cres.first);
            if (!cres.second) return { cres.first, std::nullopt };
            result.end.x = *cres.second;
            cres = int_conv_(cres.first);
            if (!cres.second) return { cres.first, std::nullopt };
            result.end.y = *cres.second;

            return { cres.first, result };
        }
    };

    inline auto counted_single_view(auto v, int count)
    {
        using namespace std::views;
        return iota(0, count) | transform([v = std::move(v)] (auto const&){ return v; });
    }


    template<typename TOutIt>
    constexpr TOutIt convert_to_vents(std::string_view in, TOutIt out)
    {
        vent_line_str_convert conv{};
        return std::copy(from_string_base_it(in, conv), from_string_base_it(in, std::cend(in), conv), out);
    }

    inline constexpr auto coord_insert_or_increase(auto& cont)
    {
        using coord_count = std::pair<coord, int>;
        return sorted_insert_or_increase_it(cont,
            [](coord_count& v_count) { ++v_count.second; },
            [](coord_count const& lhs, coord_count const& rhs) { return coord_by_x(lhs.first, rhs.first) < 0; }
        );
    }

    constexpr std::pair<std::ptrdiff_t, std::ptrdiff_t> count_v_h_intersections(std::ranges::range auto vent_lines)
    {
        auto hor_end = std::partition(begin(vent_lines), end(vent_lines), [](vent_line const& v) { return v.begin.y == v.end.y; });
        auto vert_end = std::partition(hor_end, end(vent_lines), [](vent_line const& v) { return v.begin.x == v.end.x; });
        using coord_count = std::pair<coord, int>;
        std::vector<coord_count> vent_map;
        std::span horizontal(begin(vent_lines), hor_end);
        std::span vertical(hor_end, vert_end);
        std::span diagonal(vert_end, end(vent_lines));
        for (auto&& [x_coords, y_coords] : std::ranges::views::transform(horizontal, [](vent_line const& v)
            {
                std::pair<int, int> x_pair = std::minmax(v.begin.x, v.end.x);
                x_pair.second += 1;
                auto& [bx, ex] = x_pair;
                return std::pair(std::views::iota(bx, ex), counted_single_view(v.begin.y, ex - bx));
            }))
        {
            std::transform(std::ranges::begin(x_coords), std::ranges::end(x_coords), std::ranges::begin(y_coords), coord_insert_or_increase(vent_map),
                [](int x, int y) { return coord_count({ x, y }, 1); });
        }
        for (auto&& [x_coords, y_coords] : std::ranges::views::transform(vertical, [](vent_line const& v)
            {
                std::pair<int, int> y_pair = std::minmax(v.begin.y, v.end.y);
                y_pair.second += 1;
                auto& [by, ey] = y_pair;
                return std::pair(counted_single_view(v.begin.x, ey - by), std::views::iota(by, ey));
            }))
        {
            std::transform(std::ranges::begin(x_coords), std::ranges::end(x_coords), std::ranges::begin(y_coords), coord_insert_or_increase(vent_map),
                [](int x, int y) { return coord_count({ x, y }, 1); });
        }
        auto vert_hor = std::ranges::count_if(vent_map, [](coord_count const& c) {return c.second > 1; });

        for (auto&& [x_coords, y_coords] : std::ranges::views::transform(diagonal, [](vent_line const& v)
            {
                std::pair<int, int> x_pair = std::minmax(v.begin.x, v.end.x);
                x_pair.second += 1;
                auto& [bx, ex] = x_pair;
                std::pair<int, int> y_pair = std::minmax(v.begin.y, v.end.y);
                y_pair.second += 1;
                auto& [by, ey] = y_pair;
                bool dl_ur = (bx == v.begin.x && by == v.begin.y) || (bx == v.end.x && by == v.end.y);
                int direction = dl_ur ? 1 : -1;
                int offset = dl_ur ? 0 : by + ey - 1;
                auto dir_trans = std::views::transform([direction, offset](int i) { return i * direction + offset; });
                return std::pair(std::views::iota(bx, ex), std::views::iota(by, ey) | dir_trans);
            }))
        {
            std::transform(std::ranges::begin(x_coords), std::ranges::end(x_coords), std::ranges::begin(y_coords), coord_insert_or_increase(vent_map),
                [](int x, int y) { return coord_count({ x, y }, 1); });
        }
            return { vert_hor, std::ranges::count_if(vent_map, [](coord_count const& c) {return c.second > 1; }) };
    }
}
