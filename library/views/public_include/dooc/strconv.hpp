
#pragma once

#include <charconv>
#include <optional>

namespace dooc
{
    template <typename T>
    constexpr std::optional<T> from_string(std::string_view str, int base = 10)
    {
        T to_return;
        auto conv_res = std::from_chars(str.data(), str.data() + ssize(str), to_return, base);
        if (conv_res.ec == std::errc())
        {
            return to_return;
        }
        return std::nullopt;
    }

}

