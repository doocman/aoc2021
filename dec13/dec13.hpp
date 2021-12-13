
#pragma once

#include <algorithm>
#include <deque>
#include <variant>
#include <vector>
#include <utility>
#include <string_view>
#include <unordered_set>
#include <iterator>
#include <ranges>

#include <dooc/strconv.hpp>

namespace dooc
{
    struct fold_along_y
    {
        int y;
    };
    struct fold_along_x
    {
        int x;
    };
    using coord_t = std::pair<int, int>;

    constexpr int x_of(coord_t const& c) noexcept { return c.first; }
    constexpr int y_of(coord_t const& c) noexcept { return c.second; }

    struct coord_hash
    {
        std::size_t operator()(coord_t const& c) const noexcept
        {
            return std::hash<long long>()(x_of(c) | (y_of(c) << sizeof(y_of(c))));
        }
        
    };
    using coord_set = std::unordered_set<coord_t, coord_hash>;
    using fold_t = std::variant<fold_along_x, fold_along_y>;

    inline constexpr coord_t fold_transform(coord_t const& c, fold_along_x f)
    {
        return x_of(c) < f.x ? c : coord_t{2 * f.x - x_of(c), y_of(c)};
    }

    inline constexpr auto fold_transform(coord_t const& c, fold_along_y f)
    {
        return y_of(c) < f.y ? c : coord_t{x_of(c), 2 * f.y - y_of(c)};
    }

    struct dot_map
    {
        coord_set dots;
        std::deque<fold_t> folds;
    };

    template<typename TFold>
    inline coord_set apply_fold(coord_set coords, TFold f) noexcept
    {
        coord_set to_return;
        to_return.reserve(size(coords));
        std::transform(begin(coords), end(coords), std::inserter(to_return, begin(to_return)), [f](coord_t const& c) { return fold_transform(c, f); });
        return to_return;
    }

    inline dot_map parse_dot_map(std::string_view str)
    {
        using namespace std::string_view_literals;
        dot_map result;
        auto lines = std::ranges::count(str, '\n');
        result.dots.reserve(lines);
        while (!str.starts_with("\n"sv))
        {
            auto& dots = result.dots;
            auto coords = str.substr(0, str.find('\n'));
            str = str.substr(ssize(coords) + 1);
            auto x = coords.substr(0, coords.find(','));
            auto y = coords.substr(ssize(x) + 1);
            dots.emplace(*from_string<int>(x), *from_string<int>(y));
        }
        str.remove_prefix(1);
        while (!empty(str))
        {
            auto& folds = result.folds;
            auto fold_str = str.substr(0, str.find('\n'));
            str.remove_prefix(size(fold_str));
            if (!empty(str)) str.remove_prefix(1);
            constexpr auto fold_x_str = "fold along x="sv;
            constexpr auto fold_y_str = "fold along y="sv;
            if (fold_str.starts_with(fold_x_str))
            {
                fold_str.remove_prefix(size(fold_x_str));
                folds.emplace_back(fold_along_x{ *from_string<int>(fold_str) });
            }
            else if(fold_str.starts_with(fold_y_str))
            {
                fold_str.remove_prefix(size(fold_y_str));
                folds.emplace_back(fold_along_y{ *from_string<int>(fold_str) });
            }
        }

        return result;
    }

    inline coord_set apply_fold_var(coord_set set, fold_t const& f_var)
    {
        return std::visit([dots = std::move(set), &f_var](auto f) { return apply_fold(std::move(dots), f); }, f_var);
    }

    inline dot_map pop_fold(dot_map input)
    {
        auto f_var = input.folds.front();
        input.folds.pop_front();
        input.dots = apply_fold_var(std::move(input.dots), f_var);
        return input;
    }

    inline dot_map fold_all(dot_map input)
    {
        std::for_each(begin(input.folds), end(input.folds), [&input](auto const& f_var)
            {
                input.dots = apply_fold_var(std::move(input.dots), f_var);
            });
        input.folds.clear();
        return input;
    }

    inline void format_to(coord_set const& s, auto& stream)
    {
        auto max_x = x_of(*std::max_element(begin(s), end(s), [](coord_t const& lhs, coord_t const& rhs) { return x_of(lhs) < x_of(rhs); }));
        auto max_y = y_of(*std::max_element(begin(s), end(s), [](coord_t const& lhs, coord_t const& rhs) { return y_of(lhs) < y_of(rhs); }));
        for (auto y : std::views::iota(0, max_y + 1))
        {
            for (auto x : std::views::iota(0, max_x + 1))
            {
                coord_t c{ x, y };
                if (s.count(c) == 0) stream << ' ';
                else stream << '#';
            }
            stream << '\n';
        }
    }
}
