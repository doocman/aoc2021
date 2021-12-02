
#pragma once

#include <iterator>
#include <span>

namespace dooc
{
    template <typename TValue, typename TIterator>
    struct bunch_base_iterator
    {
        using reference = std::span<TValue>;
        using value_type = reference;
        using difference_type = TIterator::difference_type;
        using iterator_category = std::random_access_iterator_tag;

        TIterator cur_pos_{};
        difference_type span_size_{};


        constexpr value_type operator*() const noexcept
        {
            return { cur_pos_, static_cast<typename reference::size_type>(span_size_) };
        }

        constexpr bunch_base_iterator& operator+=(difference_type offset) noexcept
        {
            cur_pos_ += offset;
            return *this;
        }

        constexpr bunch_base_iterator operator+(difference_type offset) const noexcept
        {
            auto res = *this;
            res += offset;
            return res;
        }

        constexpr bunch_base_iterator& operator++() noexcept
        {
            return (*this) += 1;
        }

        constexpr bunch_base_iterator operator++(int) noexcept
        {
            auto res = *this;
            ++(*this);
            return res;
        }

        constexpr bunch_base_iterator& operator-=(difference_type offset) noexcept
        {
            cur_pos_ -= offset;
        }

        constexpr bunch_base_iterator operator-(difference_type offset) const noexcept
        {
            auto res = *this;
            res -= offset;
            return res;
        }

        template <typename TIt2>
        constexpr difference_type operator-(bunch_base_iterator<TValue, TIt2> rhs) const noexcept
        {
            return cur_pos_ - rhs.cur_pos_;
        }

        constexpr bunch_base_iterator& operator--() noexcept
        {
            return (*this) -= 1;
        }

        constexpr bunch_base_iterator operator--(int) noexcept
        {
            auto res = *this;
            --(*this);
            return res;
        }
    };

    template <typename TIt>
    struct bunch_end_sentinel
    {
        using difference_type = TIt::difference_type;

        TIt cur_pos_{};
        difference_type span_size_{};
    };

    template <typename TContainer>
    struct bunch_view_t
    {
        using size_type = TContainer::size_type;
        using difference_type = TContainer::difference_type;
        using underlying_value_type = TContainer::value_type;
        using value_type = std::span<underlying_value_type>;
        using underlying_iterator = TContainer::iterator;
        using underlying_const_iterator = TContainer::iterator;

        TContainer* to_view_{};
        difference_type span_size_{};

        using iterator = bunch_base_iterator<underlying_value_type, underlying_iterator>;
        using const_iterator = bunch_base_iterator<underlying_value_type const, underlying_const_iterator>;
        using end_sentinel = bunch_end_sentinel<underlying_const_iterator>;

        constexpr bunch_view_t() noexcept = default;
        constexpr bunch_view_t(TContainer& to_view, difference_type span_size)
            : to_view_(&to_view)
            , span_size_(span_size)
        {}

        iterator begin() noexcept
        {
            using std::begin;
            if (to_view_ == nullptr)
            {
                return {};
            }
            return { begin(*to_view_), span_size_ };
        }

        const_iterator begin() const noexcept
        {
            return const_cast<bunch_view_t*>(this)->begin();
        }

        const_iterator cbegin() const noexcept
        {
            return begin();
        }

        iterator end() noexcept
        {
            using std::end;
            if (to_view_ == nullptr)
            {
                return {};
            }
            if (std::ssize(*to_view_) < span_size_)
            {
                return begin();
            }
            return { std::next(end(*to_view_), -span_size_ + 1), span_size_ };
        }

        const_iterator end() const noexcept
        {
            return const_cast<bunch_view_t*>(this)->end();
        }
        end_sentinel cend() const noexcept
        {
            return end();
        }

        value_type operator[](difference_type index) noexcept
        {
            return *(begin() + index);
        }

    };

    template <typename TValue, typename TIt1, typename TIt2>
    bool operator==(bunch_base_iterator<TValue, TIt1> lhs, bunch_base_iterator<TValue, TIt2> rhs)
    {
        return lhs.cur_pos_ == rhs.cur_pos_;
    }

    template <typename TValue, typename TIt1, typename TIt2>
    bool operator==(bunch_end_sentinel<TIt1> lhs, bunch_base_iterator<TValue, TIt2> rhs)
    {
        return std::distance(rhs.cur_pos_, lhs.cur_pos_) < lhs.span_size_;
    }

    template <typename TValue, typename TIt1, typename TIt2>
    bool operator==(bunch_base_iterator<TValue, TIt1> lhs, bunch_end_sentinel<TIt2> rhs)
    {
        return rhs == lhs;
    }

    struct bunch_view
    {
        using difference_type = std::ptrdiff_t;

        difference_type span_size_;

        explicit bunch_view(difference_type span_size) noexcept
            : span_size_(span_size)
        {
        }
    };

    template <typename TContainer>
    constexpr bunch_view_t<TContainer> operator|(TContainer& c, bunch_view v)
    {
        return { c, v.span_size_ };
    }
}

namespace std
{
    template <typename TValue, typename TIt>
    struct iterator_traits<dooc::bunch_base_iterator<TValue, TIt>> : std::input_iterator_tag
    {
        using the_it = dooc::bunch_base_iterator<TValue, TIt>;
        using difference_type = the_it::difference_type;
        using reference = the_it::reference;
        using value_type = the_it::value_type;
        using iterator_category = the_it::iterator_category;
    };
}
