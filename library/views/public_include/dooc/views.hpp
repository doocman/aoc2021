
#pragma once

#include <algorithm>
#include <concepts>
#include <iterator>
#include <span>
#include <ranges>

#include <dooc/strconv.hpp>

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
        using reference = value_type;
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

        TIterator underlying_{};
        difference_type end_dist_{};
        difference_type stride_{};

        base_stride_view_iterator() = default;
        base_stride_view_iterator(TIterator beg, TIterator end, difference_type stride)
            : underlying_(beg)
            , end_dist_(std::distance(beg, end))
            , stride_(stride)
        {}

        constexpr reference operator*() const noexcept
        {
            return *underlying_;
        }

        constexpr base_stride_view_iterator& operator+=(difference_type d) noexcept
        {
            using std::advance;
            auto to_advance = std::min(stride_ * d, end_dist_);
            advance(underlying_, to_advance);
            end_dist_ -= to_advance;
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
        using reference = TContainer::reference;
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
                return { next(std::ranges::begin(to_view_), offset_), std::ranges::end(to_view_), stride_ };
            }
            return { std::ranges::end(to_view_), std::ranges::end(to_view_), stride_ };
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
                return { std::ranges::end(to_view_), std::ranges::end(to_view_), stride_ };
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

    template <typename TValue, typename TChar, typename TTraits = std::char_traits<TChar>>
    struct default_str_convert
    {
        using str_view = std::basic_string_view<TChar, TTraits>;

        static constexpr str_view delims() noexcept
        {
            using namespace std::string_view_literals;
            return " \n,"sv;
        }

    public:

        constexpr std::pair<str_view, std::optional<TValue>> operator()(str_view to_extract)
        {
            str_view to_convert = to_extract.substr(0, to_extract.find_first_of(delims()));
            to_extract.remove_prefix(std::min(to_extract.find_first_not_of(delims(), size(to_convert)), size(to_extract)));
            auto opt_conv = from_string<TValue>(to_convert);
            return { to_extract, opt_conv };
        }
    };

    template <std::move_constructible TStrConv, typename TFromChar, typename TCharTraits = std::char_traits<TFromChar>>
    requires requires(TStrConv t) {
        { t(std::basic_string_view<TFromChar, TCharTraits>()).first } -> std::convertible_to<std::basic_string_view<TFromChar, TCharTraits>>;
        //{ *(t(std::basic_string_view<TFromChar, TCharTraits>()).second) } -> std::movable;
    }
    struct from_string_base_it
    {
        using str_view = std::basic_string_view<TFromChar, TCharTraits>;
        using value_type = std::remove_reference_t<decltype(std::declval<TStrConv>()(str_view()).second.operator *())>;
        using reference = value_type;
        using difference_type = str_view::difference_type;
        using iterator_category = std::input_iterator_tag;

        value_type current_value_{};
        str_view remaining_str_{};
        typename str_view::const_pointer current_pos_{};
        TStrConv convert_;

        constexpr from_string_base_it() = default;
        from_string_base_it(str_view str, TStrConv str_conv)
            : from_string_base_it(str, begin(str), str_conv)
        {
        }
        from_string_base_it(str_view str, typename str_view::const_iterator start_pos, TStrConv str_conv)
            : remaining_str_(str.substr(std::distance(cbegin(str), start_pos)))
            , current_pos_(remaining_str_.data())
            , convert_(std::move(str_conv))
        {
            update_current();
        }

        value_type const& operator*() const noexcept
        {
            return current_value_;
        }

        value_type& operator*() noexcept
        {
            return current_value_;
        }

        from_string_base_it& operator++() noexcept
        {
            update_current();
            return *this;
        }

        from_string_base_it operator++(int) noexcept
        {
            auto res = *this;
            ++(*this);
            return res;
        }

        bool operator==(from_string_base_it const& rhs) const noexcept
        {
            return current_pos_ == rhs.current_pos_;
        }

    private:
        void update_current()
        {
            current_pos_ = remaining_str_.data();
            auto [new_str, opt_conv] = convert_(remaining_str_);
            remaining_str_ = new_str;
            if (opt_conv) current_value_ = std::move(*opt_conv);
        }
    };

    template <std::move_constructible TStrConv, typename TFromChar, typename TCharTraits = std::char_traits<TFromChar>>
    from_string_base_it(std::basic_string_view<TFromChar, TCharTraits>, TStrConv)->from_string_base_it<TStrConv, TFromChar, TCharTraits>;

    template <std::move_constructible TStrConv, typename TFromChar, typename TCharTraits = std::char_traits<TFromChar>>
    from_string_base_it(std::basic_string_view<TFromChar, TCharTraits>, typename std::basic_string_view<TFromChar, TCharTraits>::const_iterator, TStrConv)->from_string_base_it<TStrConv, TFromChar, TCharTraits>;

    template <std::integral TValue, std::integral TChar, typename TTraits = std::char_traits<TChar>>
    constexpr auto int_from_string_it(std::basic_string_view<TChar, TTraits> str, typename std::string_view::const_iterator pos)
    {
        using convert_t = default_str_convert<TValue, TChar, TTraits>;
        return from_string_base_it<convert_t, TChar, TTraits>(str, pos, default_str_convert<TValue, TChar, TTraits>());
    }

    template <std::integral TValue, std::integral TChar, typename TTraits = std::char_traits<TChar>>
    constexpr auto int_from_string_it(std::basic_string_view<TChar, TTraits> str, typename std::string_view::difference_type pos = 0)
    {
        return int_from_string_it<TValue>(str, begin(str) + pos);
    }

    template<typename TPredicate, typename TIterator>
    concept iterator_predicate = std::semiregular<TPredicate> && requires(TPredicate p) { { p(std::declval<TIterator>()) } -> std::convertible_to<bool>; };


    template<std::input_or_output_iterator TIterator, iterator_predicate<TIterator> TPredicate>
    class generic_sentinel
    {
        TPredicate end_predicate_;
    public:
        explicit generic_sentinel(TPredicate p, TIterator const& = TIterator())
            : end_predicate_(std::move(p))
        {}

        generic_sentinel() = default;

        template<std::input_or_output_iterator TIterator2>
        //requires std::is_convertible_v<TIterator2, TIterator>
            constexpr bool operator()(TIterator2 rhs) const
        {
            return end_predicate_(rhs);
        }
    };

    template<std::input_or_output_iterator TIterator1, iterator_predicate<TIterator1> TPredicate, std::input_or_output_iterator TIterator2>
    //requires std::is_convertible_v< TIterator2, TIterator1>
        constexpr bool operator==(generic_sentinel<TIterator1, TPredicate> s, TIterator2 it)
    {
        return s(it);
    }

    template<std::input_or_output_iterator TIterator1, iterator_predicate<TIterator1> TPredicate, std::input_or_output_iterator TIterator2>
    //requires std::is_convertible_v< TIterator2, TIterator1>
        constexpr bool operator==(TIterator2 it, generic_sentinel<TIterator1, TPredicate> s)
    {
        return s == it;
    }

    template <typename T = void>
    struct pre_increment
    {
        template<typename T2>
            constexpr void operator()(T2& v) const noexcept(noexcept(++v)) requires std::is_same_v<T, void>
        {
            ++v;
        }

            constexpr void operator()(T& v) const noexcept(noexcept(++v)) requires !std::is_same_v<T, void>
        {
            ++v;
        }
    };

    template <typename TCont, typename TIncOp = pre_increment<>, typename TOrderCmp = std::less<>>
    class sorted_insert_or_increase_it
    {
        TCont* cont_;
        TIncOp inc_;
        TOrderCmp cmp_;

        template<typename T>
        constexpr bool is_eq(T const& l, T const& r) const
        {
            return !cmp_(l, r) && !cmp_(r, l);
        }
    public:

        using value_type = TCont::value_type;
        using iterator_category = std::output_iterator_tag;

        struct ref_proxy
        {
            sorted_insert_or_increase_it* main_;
            ref_proxy(sorted_insert_or_increase_it& main)
                : main_(&main)
            {
            }

            template <typename T>
            //requires std::convertible_to<value_type>
                ref_proxy& operator=(T&& v)
            {
                auto pos = std::find_if(begin(*main_->cont_), end(*main_->cont_), [&v, this](value_type const& rhs) -> bool { return !main_->cmp_(rhs, v); });
                if (pos != std::ranges::end(*main_->cont_) && main_->is_eq(v, *pos))
                {
                    main_->inc_(*pos);
                }
                else
                {
                    main_->cont_->insert(pos, std::move(v));
                }
                return *this;
            }
        };

        using reference = ref_proxy;

        explicit sorted_insert_or_increase_it(TCont& cont, TIncOp inc_op = TIncOp{}, TOrderCmp cmp = TOrderCmp{})
            : cont_(&cont)
            , inc_(std::move(inc_op))
            , cmp_(std::move(cmp))
        {}

        reference operator*()
        {
            return { *this };
        }

        constexpr sorted_insert_or_increase_it& operator++() noexcept
        {
            // no op.
            return *this;
        }

        constexpr sorted_insert_or_increase_it operator++(int) noexcept
        {
            // no op.
            return *this;
        }
    };
}
