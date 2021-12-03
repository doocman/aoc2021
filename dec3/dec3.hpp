
#pragma once

#include <algorithm>
#include <vector>
#include <numeric>

#include <dooc/views.hpp>

#include <cassert>

namespace dooc
{
    struct gamma_eps
    {
        int gamma;
        int eps;
    };

    struct oxy_co2
    {
        int oxygen_gen;
        int co2_scrub;
    };

    constexpr int bits_per_entry = 5;

    struct bit_counter
    {
        std::vector<int> bit_counts;
    };

    bit_counter& operator+=(bit_counter& lhs, bit_counter const& rhs)
    {
        auto& lb = lhs.bit_counts;
        auto const& rb = rhs.bit_counts;
        auto s = std::max(size(lb), size(rb));
        lb.resize(s);
        std::transform(begin(rb), end(rb), begin(lb), begin(lb), std::plus());
        return lhs;
    }

    bit_counter operator+(bit_counter lhs, bit_counter const& rhs)
    {
        return lhs += rhs;
    }


    template<std::ranges::view TV>
    inline bit_counter step_bit_counter(TV data, bit_counter prev = {})
    {
        using std::ranges::begin, std::ranges::end, std::size;
        prev.bit_counts.resize(size(data));
        std::transform(begin(data), end(data), begin(prev.bit_counts), begin(prev.bit_counts), [](char c, int count) { return count + (c - '0'); });
        return prev;
    }

    inline int bit_view_2_int(std::ranges::range auto&& v)
    {
        using std::ranges::begin, std::ranges::rend, std::ranges::rbegin;
        auto indexing = std::views::iota(0) | std::views::transform([](int i) {return 1 << i; });
        return std::transform_reduce(rbegin(v), rend(v), begin(indexing), 0, std::plus(), [](int bit_index, int value) {
            return value * bit_index; });
    }

    inline int bit_counter_2_int(bit_counter data)
    {
        return bit_view_2_int(data.bit_counts);
    }

    inline bit_counter count_bits(std::ranges::range auto&& v)
    {
        using std::ranges::begin, std::ranges::end;
        return std::transform_reduce(begin(v), v.end(), bit_counter{}, std::plus(), [](auto data) { return step_bit_counter(data); });
    }

    gamma_eps create_gamma_eps(std::string_view data, int bit_width)
    {
        using std::ranges::begin, std::ranges::end;
        auto v = data | bunch_view(bit_width) | stride_view(bit_width);
        auto tot_counts = count_bits(v);
        const int v_size = static_cast<int>(std::ssize(v));
        bit_counter gamma;
        gamma.bit_counts.reserve(size(tot_counts.bit_counts));
        std::transform(begin(tot_counts.bit_counts), end(tot_counts.bit_counts), std::back_inserter(gamma.bit_counts), [v_size](int v)
            { return (v * 2) / v_size; });
        bit_counter eps;
        eps.bit_counts.reserve(size(tot_counts.bit_counts));
        std::transform(begin(gamma.bit_counts), end(gamma.bit_counts), std::back_inserter(eps.bit_counts), [](int i) { return !i; });
        return { bit_counter_2_int(gamma), bit_counter_2_int(eps) };
    }

    oxy_co2 create_oxy_co2(std::string_view data, int bit_width)
    {
        using std::ranges::begin, std::ranges::end;
        using vector_t = std::vector<std::span<char const>>;
        vector_t buf_1;
        vector_t buf_0;
        buf_1.reserve(data.size());
        buf_0.reserve(data.size());
        {
            auto v = data | bunch_view(bit_width) | stride_view(bit_width);
            std::partition_copy(begin(v), end(v), std::back_inserter(buf_1), std::back_inserter(buf_0), [](std::span<char const> str) { return '0' == str[0]; });
        }
        if (std::ssize(buf_1) < std::ssize(buf_0))
        {
            std::swap(buf_1, buf_0);
        }
        int bit_index = 1;
        while (ssize(buf_1) > 1)
        {
            auto part_div = std::partition(begin(buf_1), end(buf_1), [bit_index](std::span<char const> str) { return str[bit_index] == '0'; });
            if (std::distance(begin(buf_1), part_div) * 2 > ssize(buf_1))
            {
                buf_1.erase(part_div, end(buf_1));
            }
            else
            {
                buf_1.erase(begin(buf_1), part_div);
            }
            ++bit_index;
        }
        bit_index = 1;
        while (ssize(buf_0) > 1)
        {
            auto part_div = std::partition(begin(buf_0), end(buf_0), [bit_index](std::span<char const> str) { return str[bit_index] == '0'; });
            if (std::distance(begin(buf_0), part_div) * 2 > ssize(buf_0))
            {
                buf_0.erase(begin(buf_0), part_div);
            }
            else
            {
                buf_0.erase(part_div, end(buf_0));
            }
            ++bit_index;
        }


        return {
            bit_view_2_int(buf_1[0] | std::ranges::views::transform([](char c) { return c - '0'; })),
            bit_view_2_int(buf_0[0] | std::ranges::views::transform([](char c) { return c - '0'; }))
        };
    }
}
