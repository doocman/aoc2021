
#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <numeric>
#include <optional>
#include <span>
#include <vector>
#include <ranges>
#include <type_traits>

#include <dooc/views.hpp>

#include <iostream>

#include <cassert>

namespace dooc
{

    template <std::ranges::range TCont>
    struct boards_t
    {
        using container_t = std::remove_cvref_t<TCont>;
        using board_view = decltype(std::declval<container_t&>() | bunch_view(int()) | stride_view(int()));

        container_t raw_data_;

        template<typename TCont2>
        explicit boards_t(TCont2&& raw)
            : raw_data_(std::forward<TCont2>(raw))
        {}

        board_view boards()
        {
            return raw_data_ | bunch_view(25) | stride_view(25);
        }
    };
    template<typename TCont2>
    boards_t(TCont2&&)->boards_t<std::remove_cvref_t<TCont2>>;

    template <std::ranges::range TCont>
    struct bingo_result
    {
        using value_type = typename TCont::value_type;
        boards_t<TCont> boards;
        std::vector<std::pair<std::vector<std::span<value_type>>, value_type>> winning_boards;
        value_type winning_number;
    };

    template<typename T>
    inline constexpr bool is_row_finished(std::span<T> b, int row_index)
    {
        auto start_it = next(begin(b), row_index * 5);
        auto end_it = next(start_it, 5);
        return std::all_of(start_it, end_it, [](T const& v) { return v == -1; });
    }

    template <typename T>
    inline constexpr bool is_column_finished(std::span<T> b, int column_index)
    {
        auto v = b | stride_view(5, column_index);
        return std::all_of(begin(v), end(v), [](T const& v) { return v == -1; });
    }

    template <typename T>
    inline constexpr bool is_ul_dr_diag_finished(std::span<T> b)
    {
        auto v = b | stride_view(6);
        return std::all_of(begin(v), std::next(begin(v), 5), [](T const& v) { return v == -1; });
    }

    template <typename T>
    inline constexpr bool is_ur_dl_diag_finished(std::span<T> b)
    {
        auto v = b | stride_view(4, 4);
        return std::all_of(begin(v), std::next(begin(v), 5), [](T const& v) { return v == -1; });
    }

    template <typename T>
    inline constexpr bool is_board_finished(std::span<T> b)
    {
        using std::ranges::begin, std::ranges::end;
        std::ranges::iota_view iota{ 0, 5 };
        return std::any_of(begin(iota), end(iota), [b](int i) { return is_row_finished(b, i) || is_column_finished(b, i); });// || is_ul_dr_diag_finished(b) || is_ur_dl_diag_finished(b);
    }

    template <typename TNumIt>
    struct bingo_is_finished_sentinel
    {
        TNumIt last_number{};
        bool* is_done_ptr{};


        bool operator()(TNumIt const& rhs) const noexcept
        {
            return rhs == last_number || (is_done_ptr != nullptr && *is_done_ptr);
        }

    };

    template <std::ranges::range TCont, std::ranges::range TNumbers>
    inline bingo_result<TCont> announce_bingo_numbers(boards_t<TCont> boards_in, TNumbers const& numbers)
    {
        using value_t = typename TCont::value_type;
        using std::begin, std::end;
        bool any_board_finished{};
        bingo_result<TCont> res{ std::move(boards_in) };
        auto& boards = res.boards;
        std::vector<std::span<value_t>> boards_not_won;
        boards_not_won.reserve(boards.boards().size());

        {
            auto b_view = boards.boards();
            std::copy_n(begin(b_view), b_view.size(), std::back_inserter(boards_not_won));
        }

        for (int i = 0; auto n : numbers)
        {
            for (auto b : boards_not_won)
            {
                std::ranges::replace(b, n, -1);
            }

            auto won_boards = std::partition(begin(boards_not_won), end(boards_not_won), [](std::span<value_t> board_span)
                {
                    return !is_board_finished(board_span);
                }
            );



            if (won_boards != end(boards_not_won))
            {
                std::cout << "New victory on " << i << "\n";
                res.winning_boards.emplace_back(std::vector<std::span<value_t>>{ won_boards, end(boards_not_won) }, n);
                boards_not_won.resize(std::distance(begin(boards_not_won), won_boards));

                //std::for_each(won_boards, std::ranges::end(boards_not_won), [&res](std::span<value_t> b)
                //    {
                //        //std::transform(begin(b), end(b), begin(b), [](value_t v) { return (v == -1) ? -2 : v; });

                //    });
                //std::transform(begin(winning_boards.front()), end(winning_boards.front()), begin(winning_boards.front()), [](value_t v) { return (v == -1) ? -2 : v; });
                //res.winning_boards.emplace_back(winning_boards.front(), n);
            }
            ++i;
            if (i >= 86)
            {
                std::cout << "";
            }
        }

        return res;
    }

    template <std::ranges::range TBoard, typename T>
    inline auto retrieve_winning_product(TBoard b, T win_nr)
    {
        return std::transform_reduce(begin(b), end(b), 0, std::plus(), [](T v) { return std::max(0, v); }) * win_nr;
    }

}
