
#pragma once

#include <string_view>
#include <unordered_map>
#include <utility>
#include <ranges>

#include <dooc/views.hpp>

namespace dooc
{

    struct chunk_calc_result
    {
        std::string_view remaining_str;
        int points;
        long long completion_points;
    };

    inline std::optional<chunk_calc_result> calculate_chunk_error_points_helper(
        std::string_view data,
        std::ranges::range auto const& token_pairs,
        std::ranges::range auto const& token_points,
        std::ranges::range auto const& compl_points,
        long long completion_points)
    {
        if (empty(data)) return chunk_calc_result{};
        char tok = data.front();
        auto tok_it = std::find_if(begin(token_pairs), end(token_pairs), [tok](auto tok_pair) { return tok_pair.first == tok; });
        if (tok_it != end(token_pairs))
        {
            chunk_calc_result to_return{data};
            to_return.completion_points = completion_points;
            to_return.remaining_str.remove_prefix(1);
            {
                auto rec_res = calculate_chunk_error_points_helper(to_return.remaining_str, token_pairs, token_points, compl_points, completion_points);
                while (!empty(to_return.remaining_str) && rec_res)
                {
                    to_return.points += rec_res->points;
                    to_return.remaining_str = rec_res->remaining_str;
                    to_return.completion_points = rec_res->completion_points;
                    rec_res = calculate_chunk_error_points_helper(to_return.remaining_str, token_pairs, token_points, compl_points, completion_points);
                }
            }
            if (empty(to_return.remaining_str))
            {
                to_return.completion_points *= 5;
                to_return.completion_points += std::find_if(begin(compl_points), end(compl_points), [tok_it](auto const& tok_p)
                    {
                        return tok_p.first == tok_it->second;
                    })->second;
                return to_return;
            }
            tok = to_return.remaining_str.front();
            to_return.remaining_str.remove_prefix(1);
            if (tok != tok_it->second)
            {
                auto point_it = std::find_if(begin(token_points), end(token_points), [tok](auto const& tok_point)
                    {
                        return tok_point.first == tok;
                    });
                to_return.points = point_it->second;
            }
            return to_return;
        }
        return std::nullopt;
    }

    inline std::pair<int, long long> calculate_chunk_error_points(std::string_view line)
    {
        constexpr std::array token_point_map{
            std::pair{')', 3},
            std::pair{']', 57},
            std::pair{'}', 1197},
            std::pair{'>', 25137}
        };
        constexpr std::array compl_point_map{
            std::pair{')', 1},
            std::pair{']', 2},
            std::pair{'}', 3},
            std::pair{'>', 4}
        };
        constexpr std::array token_pair_map{
            std::pair{'(', ')'},
            std::pair{'[', ']'},
            std::pair{'{', '}'},
            std::pair{'<', '>'},
        };
        std::pair<int, long long> res{};
        while (!empty(line))
        {
            auto points = calculate_chunk_error_points_helper(line, token_pair_map, token_point_map, compl_point_map, res.second);
            res.first += points->points;
            res.second += points->completion_points;
            line = points->remaining_str;
        }
        return res;
    }

    inline std::pair<int, long long> calculate_all_chunks(std::string_view data)
    {
        int res = 0;
        std::vector<long long> compl_res{};
        while (!empty(data))
        {
            auto line = data.substr(0, data.find('\n'));
            data.remove_prefix(size(line));
            if (!empty(data)) data.remove_prefix(1);
            auto err_points = calculate_chunk_error_points(line);
            if(err_points.first > 0) res += err_points.first;
            else
            {
                compl_res.insert(std::find_if(begin(compl_res), end(compl_res), [&err_points](auto p) { return p < err_points.second; }), err_points.second);
            }
        }
        return { res, compl_res[ssize(compl_res) / 2] };
    }
}
