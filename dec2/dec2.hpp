
#pragma once

#include <algorithm>
#include <array>
#include <optional>
#include <iosfwd>
#include <numeric>
#include <variant>

namespace dooc
{
    struct pos
    {
        int horizontal;
        int vertical;
        int aim;
    };

    template <typename TDer>
    struct command_base
    {
        constexpr pos operator()(pos current, int distance, bool use_aim) const noexcept
        {
            static_cast<TDer const*>(this)->mod_pos(current, distance, use_aim);
            return current;
        }
    };

    struct move_forward : command_base<move_forward>
    {
        constexpr void mod_pos(pos& to_mod, int distance, bool use_aim) const noexcept
        {
            to_mod.horizontal += distance;
            if (use_aim)
            {
                to_mod.vertical += to_mod.aim * distance;
            }
        }
    };
    struct move_up : command_base<move_up>
    {
        constexpr void mod_pos(pos& to_mod, int distance, bool use_aim) const noexcept
        {
            if(!use_aim) to_mod.vertical -= distance;
            to_mod.aim -= distance;
        }
    };
    struct move_down : command_base<move_down>
    {
        constexpr void mod_pos(pos& to_mod, int distance, bool use_aim) const noexcept
        {
            if (!use_aim) to_mod.vertical += distance;
            to_mod.aim += distance;
        }
    };

    using command_t = std::variant<move_down, move_forward, move_up>;

    inline constexpr pos issue_command(command_t cmd, pos cur_pos, int dist_in, bool use_aim)
    {
        return std::visit([cur_pos, dist_in, use_aim](auto cmd) { return cmd(cur_pos, dist_in, use_aim); }, cmd);
    }

    template<typename... Ts>
    constexpr std::size_t variants_in_variant(std::variant<Ts...> const&)
    {
        return sizeof...(Ts);
    }

    constexpr std::size_t command_count = variants_in_variant(command_t{});

    constexpr std::array<std::pair<std::string_view, command_t>, command_count> string_2_command_table()
    {
        std::array<std::pair<std::string_view, command_t>, command_count> res =
        { 
            {
                {"forward", command_t(move_forward())},
            {"up", command_t(move_up())},
            {"down", command_t(move_down())}
            } };
        std::sort(begin(res), end(res), [](auto const& lhs, auto const& rhs) { return lhs.first < rhs.first; });
        return res;
    }

    constexpr std::optional<command_t> str_2_cmd(std::string_view str)
    {
        constexpr auto table = string_2_command_table();
        auto res = std::find_if(begin(table), end(table), [str](auto const& p) { return str == p.first; });
        if (res != end(table))
        {
            return res->second;
        }
        return std::nullopt;
    }

    inline std::pair<std::string_view, std::string_view> next_word(std::string_view input)
    {
        using namespace std::literals::string_view_literals;
        constexpr auto delims = " \n"sv;
        auto w = input.substr(0, input.find_first_of(delims));
        auto tot = input.substr(std::ssize(w));
        tot = tot.substr(std::min(tot.find_first_not_of(delims), tot.size()));
        return { w, tot };
    }

    inline pos process_movement(std::string_view str, pos start, bool use_aim)
    {
        using namespace std::literals::string_view_literals;
        while (!empty(str))
        {
            auto cmd_str = next_word(str);
            if (empty(cmd_str.second))
            {
                break;
            }
            auto dist_str = next_word(cmd_str.second);
            str = dist_str.second;
            if (auto opt_cmd = str_2_cmd(cmd_str.first))
            {
                int dist;
                auto conv_res = std::from_chars(dist_str.first.data(), std::next(dist_str.first.data(), dist_str.first.size()), dist);
                if (conv_res.ec != std::errc())
                {
                    std::cerr << "Failed to read distance from '" << dist_str.first << "'\n";
                }
                else
                {
                    start = issue_command(*opt_cmd, start, dist, use_aim);
                }
            }
            else
            {
                std::cerr << "Failed to read command '" << cmd_str.first << "'\n";
            }
        }
        return start;
    }
}
