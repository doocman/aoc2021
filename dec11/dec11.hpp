
#pragma once

#include <string_view>
#include <vector>
#include <span>
#include <utility>
#include <ranges>
#include <string_view>

#include <dooc/views.hpp>

#include <cassert>

namespace dooc
{
    struct octopus_map_t
    {
        using value_t = signed char;
        std::vector<value_t> all_;
        std::vector<std::span<value_t>> map_;
        std::ptrdiff_t h_size_{};
        std::ptrdiff_t flashes_{};

        explicit octopus_map_t(std::string_view raw)
        {
            all_.reserve(size(raw));
            constexpr value_t signed_zero = static_cast<value_t>('0');
            constexpr value_t delim = static_cast<value_t>('\n') - signed_zero;
            h_size_ = raw.find('\n');
            using namespace std::views;
            auto raw_signed = all(raw) | transform([signed_zero](char c) { return static_cast<value_t>(c) - signed_zero; });
            std::ranges::copy_if(raw_signed, std::back_inserter(all_), [delim](value_t c) { return c != delim; });
            setup_map();
        }

        void setup_map()
        {
            map_.reserve(size(all_) / h_size_);
            std::ranges::copy(all_ | bunch_view(h_size_) | stride_view(h_size_), std::back_inserter(map_));
        }

        constexpr octopus_map_t(octopus_map_t&&) noexcept = default;
        constexpr octopus_map_t& operator=(octopus_map_t&&) noexcept = default;

        octopus_map_t(octopus_map_t const& rhs)
            : all_(rhs.all_)
            , flashes_(rhs.flashes_)
            , h_size_(rhs.h_size_)
        {
            setup_map();
        }
        octopus_map_t& operator=(octopus_map_t const& rhs)
        {
            all_ = rhs.all_;
            flashes_ = rhs.flashes_;
            h_size_ = rhs.h_size_;
            setup_map();
            return *this;
        }
    };

    constexpr std::pair<std::ptrdiff_t, std::ptrdiff_t> index_2_coord(std::ptrdiff_t index, std::ptrdiff_t h_size)
    {
        return { index / h_size, index % h_size };
    }

    constexpr octopus_map_t::value_t flash_v = 10;

    constexpr int step_single_oct(auto& collateral_oct, auto& to_flash)
    {
        if (collateral_oct > 0)
        {
            ++collateral_oct;
            if (collateral_oct == flash_v)
            {
                assert(std::ranges::count(to_flash, std::addressof(collateral_oct)) == 0);
                to_flash.push_back(std::addressof(collateral_oct));
                return 1;
            }
        }
        return 0;
    }

    octopus_map_t step_octopuses(octopus_map_t input)
    {
        using value_t = octopus_map_t::value_t;
        std::vector<value_t*> to_flash;
        to_flash.reserve(size(input.all_));
        {
            using namespace std::views;
            std::ranges::transform(input.all_, begin(input.all_), [](value_t v) { return v + 1; });
            std::ranges::copy_if(all(input.all_) | transform([](value_t& oct) { return &oct; }), std::back_inserter(to_flash), [](value_t* oct) { return *oct == flash_v; });
        }
        input.flashes_ += ssize(to_flash);
        while (!empty(to_flash))
        {
            value_t* cur_f = to_flash.back();
            to_flash.pop_back();
            auto index = std::distance(std::addressof(input.all_.front()), cur_f);
            auto [x, y] = index_2_coord(index, input.h_size_);
            if (0 < x)
            {
                auto const dx = x - 1;
                input.flashes_ += step_single_oct(input.map_[dx][y], to_flash);
                if(0 < y) input.flashes_ += step_single_oct(input.map_[dx][y - 1], to_flash);
                if (y + 1 < std::ssize(input.map_)) input.flashes_ += step_single_oct(input.map_[dx][y + 1], to_flash);
            }
            if (x + 1 < input.h_size_)
            {
                auto const dx = x + 1;
                input.flashes_ += step_single_oct(input.map_[dx][y], to_flash);
                if (0 < y) input.flashes_ += step_single_oct(input.map_[dx][y - 1], to_flash);
                if (y + 1 < std::ssize(input.map_)) input.flashes_ += step_single_oct(input.map_[dx][y + 1], to_flash);
            }
            if (0 < y) input.flashes_ += step_single_oct(input.map_[x][y - 1], to_flash);
            if (y + 1 < ssize(input.map_)) input.flashes_ += step_single_oct(input.map_[x][y + 1], to_flash);
            *cur_f = 0;
        }
        return input;
    }

    octopus_map_t step_octopuses(octopus_map_t input, std::ptrdiff_t n)
    {
        for (auto _: std::views::iota(0, n))
        {
            input = step_octopuses(std::move(input));
        }
        return input;
    }

    int find_first_sync(octopus_map_t input)
    {
        int steps{};
        while (input.flashes_ != std::ssize(input.all_))
        {
            input.flashes_ = 0;
            input = step_octopuses(std::move(input));
            ++steps;
        }
        return steps;
    }
}
