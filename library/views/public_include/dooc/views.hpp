
#pragma once

#include <iterator>
#include <span>
#include <ranges>

namespace dooc
{
    template <typename TIterator>
    struct bunch_base_iterator
    {
        using e_value = std::remove_reference_t<decltype(*(std::declval<TIterator>()))>;
        using reference = std::span<e_value>;
        using value_type = reference;
        using difference_type = std::iterator_traits<TIterator>::difference_type;
        using iterator_category = std::random_access_iterator_tag;

        TIterator cur_pos_{};
        difference_type span_size_{};

        bunch_base_iterator() = default;

        template <typename TPtr>
        requires std::is_convertible_v<TPtr, TIterator>
        bunch_base_iterator(TPtr pos, difference_type span_size)
            : cur_pos_(pos)
            , span_size_(span_size)
        {}

        template <typename TPtr>
        requires std::is_convertible_v<TPtr, TIterator>
        bunch_base_iterator(bunch_base_iterator<TPtr> const& rhs)
            : cur_pos_(rhs.cur_pos_)
            , span_size_(rhs.span_size_)
        {}

        bunch_base_iterator(bunch_base_iterator const&) = default;
        bunch_base_iterator& operator=(bunch_base_iterator const&) = default;
        bunch_base_iterator(bunch_base_iterator &&) noexcept = default;
        bunch_base_iterator& operator=(bunch_base_iterator &&) noexcept = default;



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
            return *this;
        }

        constexpr bunch_base_iterator operator-(difference_type offset) const noexcept
        {
            auto res = *this;
            res -= offset;
            return res;
        }

        template <typename TIt2>
        constexpr difference_type operator-(bunch_base_iterator<TIt2> rhs) const noexcept
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

    template <std::ranges::random_access_range TContainer>
    struct bunch_view_t : std::ranges::view_interface<bunch_view_t<TContainer>>
    {
        using size_type = TContainer::size_type;
        using difference_type = TContainer::difference_type;
        using underlying_value_type = TContainer::value_type;
        using value_type = std::span<underlying_value_type>;
        using underlying_iterator = std::remove_reference_t<decltype(std::declval<TContainer>().data())>; //TContainer::pointer;
        using underlying_const_iterator = TContainer::const_pointer;

        TContainer* to_view_{};
        difference_type span_size_{};

        using iterator = bunch_base_iterator<underlying_iterator>;
        using const_iterator = bunch_base_iterator<underlying_const_iterator>;
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
            return { to_view_->data(), span_size_ };
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
            return { std::next(to_view_->data() + std::ssize(*to_view_), -span_size_ + 1), span_size_ };
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

    template <typename TIt1, typename TIt2>
    auto operator==(bunch_base_iterator<TIt1> lhs, bunch_base_iterator<TIt2> rhs)
    {
        return lhs.cur_pos_ == rhs.cur_pos_;
    }


    template <typename TIt1, typename TIt2>
    auto operator<=>(bunch_base_iterator<TIt1> lhs, bunch_base_iterator<TIt2> rhs)
    {
        return lhs.cur_pos_ <=> rhs.cur_pos_;
    }

    template <typename TIt1, typename TIt2>
    bool operator==(bunch_end_sentinel<TIt1> lhs, bunch_base_iterator<TIt2> rhs)
    {
        return std::distance(rhs.cur_pos_, lhs.cur_pos_) < lhs.span_size_;
    }

    template <typename TIt1, typename TIt2>
    bool operator==(bunch_base_iterator<TIt1> lhs, bunch_end_sentinel<TIt2> rhs)
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


    template <typename TIterator>
    struct base_stride_view_iterator
    {
        using reference = std::iterator_traits<TIterator>::reference;
        using value_type = std::iterator_traits<TIterator>::value_type;
        using pointer = std::iterator_traits<TIterator>::pointer;
        using difference_type = std::iterator_traits<TIterator>::difference_type;
        using iterator_category = std::iterator_traits<TIterator>::iterator_category;
        
        TIterator underlying_;
        difference_type stride_;

        constexpr reference operator*() const noexcept
        {
            return *underlying_;
        }

        constexpr base_stride_view_iterator& operator+=(difference_type d) noexcept
        {
            using std::advance;
            advance(underlying_, stride_ * d);
            return *this;
        }

        constexpr base_stride_view_iterator& operator-=(difference_type d) noexcept
        {
            return (*this) += -d;
        }

        constexpr base_stride_view_iterator& operator++() noexcept
        {
            return (*this) += 1;
        }

        constexpr base_stride_view_iterator& operator--() noexcept
        {
            return (*this) += -1;
        }

        constexpr base_stride_view_iterator operator++(int) noexcept
        {
            auto ret = *this;
            ++(*this);
            return ret;
        }

        constexpr base_stride_view_iterator operator--(int) noexcept
        {
            auto ret = *this;
            --(*this);
            return ret;
        }

        constexpr difference_type operator-(base_stride_view_iterator rhs)
        {
            return (underlying_ - rhs.underlying_) / stride_;
        }
    };

    template <typename TIterator1, typename TIterator2>
    auto operator==(base_stride_view_iterator<TIterator1> const& lhs, base_stride_view_iterator<TIterator2> const& rhs)
    {
        return lhs.underlying_ == rhs.underlying_;
    }

    template <std::ranges::view TContainer>
    struct stride_view_t : std::ranges::view_interface<stride_view_t<TContainer>>
    {
        using size_type = TContainer::size_type;
        using difference_type = TContainer::difference_type;
        using value_type = TContainer::value_type;
        using underlying_iterator = TContainer::iterator;
        using underlying_const_iterator = TContainer::iterator;

        using iterator = base_stride_view_iterator<underlying_iterator>;
        using const_iterator = base_stride_view_iterator<underlying_const_iterator>;

        TContainer to_view_{};
        difference_type stride_{1};
        difference_type offset_{};

        constexpr stride_view_t() = default;
        stride_view_t(TContainer to_view, difference_type stride, difference_type offset = 0)
            : to_view_(to_view)
            , stride_(stride)
            , offset_(offset)
        {}

        constexpr size_type size() const noexcept
        {
            using std::ssize;
            auto tot_size = ssize(to_view_) + stride_ - 1 - offset_;
            return std::max<std::ptrdiff_t>(0, (tot_size) / stride_);
        }

        constexpr iterator begin() noexcept
        {
            using std::ssize;
            using namespace std::ranges;
            if (offset_ < ssize(to_view_))
            {
                return { next(std::ranges::begin(to_view_), offset_), stride_ };
            }
            return { std::ranges::end(to_view_), stride_ };
        }

        constexpr const_iterator begin() const noexcept
        {
            return const_cast<stride_view_t*>(this)->begin();
        }

        constexpr const_iterator cbegin() const noexcept
        {
            return begin();
        }

        constexpr iterator end() noexcept
        {
            using std::ssize;
            using namespace std::ranges;
            auto s = size();
            if (s > 0)
            {
                return { next(std::ranges::begin(to_view_), offset_ + s * stride_), stride_ };
            }
            return begin();
        }

        constexpr const_iterator end() const noexcept
        {
            return const_cast<stride_view_t*>(this)->end();
        }

        constexpr const_iterator cend() const noexcept
        {
            return end();
        }
    };

    struct stride_view
    {
        using difference_type = std::ptrdiff_t;

        difference_type stride_;
        difference_type offset_;

        explicit stride_view(difference_type stride, difference_type offset = 0)
            : stride_(stride)
            , offset_(offset)
        {}
    };

    template <std::ranges::view TV>
    constexpr stride_view_t<TV> operator|(TV v, stride_view sr)
    {
        return { v, sr.stride_, sr.offset_ };
    }

    //template <std::ranges::viewable_range TV>
    //constexpr stride_view_t<TV> operator|(TV v, stride_view sr)
    //{
    //    return { std::ranges::views::all(v), sr.stride_, sr.offset_ };
    //}
}
