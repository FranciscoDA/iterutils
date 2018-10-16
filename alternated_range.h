#ifndef _ITERUTILS_ALTERNATED_RANGE_H_
#define _ITERUTILS_ALTERNATED_RANGE_H_

#include <variant>
#include <utility>
#include <array>

namespace iterutils {

namespace detail {
	template<typename Tag, typename ...Iterators>
	class alternated_iterator_impl {};

	template<typename Tag, typename ...Iterators>
	alternated_iterator_impl<Tag, Iterators...>& operator++(alternated_iterator_impl<Tag, Iterators...>& it) {
		std::visit([](auto&& item){ ++item; }, it._its[it._index]);
		it._index = (it._index+1) % sizeof...(Iterators);
		return it;
	}
	template<typename Tag, typename ...Iterators>
	alternated_iterator_impl<Tag, Iterators...> operator++(alternated_iterator_impl<Tag, Iterators...>& it, int) {
		auto copy = it;
		++copy;
		return it;
	}
	template<typename Tag, typename ...Iterators>
	alternated_iterator_impl<Tag, Iterators...>& operator--(alternated_iterator_impl<Tag, Iterators...>& it) {
		// decreasing a 0 index would wrap to SIZE_MAX so we wrap to sizeof(Iterators) instead
		if (it._index == 0)
			it._index = sizeof...(Iterators)-1;
		else
			--it._index;
		std::visit([](auto&& item){ --item; }, it._its[it._index]);
		return it;
	}
	template<typename Tag, typename ...Iterators>
	alternated_iterator_impl<Tag, Iterators...> operator--(alternated_iterator_impl<Tag, Iterators...>& it, int) {
		auto copy = it;
		--copy;
		return it;
	}
	template<typename Tag, typename ...Iterators>
	alternated_iterator_impl<Tag, Iterators...>&
	operator+=(alternated_iterator_impl<Tag, Iterators...>& it, typename alternated_iterator_impl<Tag, Iterators...>::difference_type n) {
		while (n) {
			auto x = (n + sizeof...(Iterators)-1) / sizeof...(Iterators);
			std::visit([x](auto&& item){ item += x; }, it._its[it._index]);
			n -= x;
			it._index = (it._index+1) % sizeof...(Iterators);
		}
		return it;
	}
	template<typename Tag, typename ...Iterators>
	alternated_iterator_impl<Tag, Iterators...>
	operator+(const alternated_iterator_impl<Tag, Iterators...>& it, typename alternated_iterator_impl<Tag, Iterators...>::difference_type n) {
		auto copy = it;
		return copy += it;
	}
	template<typename Tag, typename ...Iterators>
	alternated_iterator_impl<Tag, Iterators...>&
	operator-=(alternated_iterator_impl<Tag, Iterators...>& it, typename alternated_iterator_impl<Tag, Iterators...>::difference_type n) {
		while (n) {
			auto x = (n + sizeof...(Iterators)-1) / sizeof...(Iterators);
			std::visit([x](auto&& item){ item -= x; }, it._its[it._index]);
			n -= x;
			it._index = (it._index+1) % sizeof...(Iterators);
		}
		return it;
	}
	template<typename Tag, typename ...Iterators>
	alternated_iterator_impl<Tag, Iterators...>
	operator-(const alternated_iterator_impl<Tag, Iterators...>& it, typename alternated_iterator_impl<Tag, Iterators...>::difference_type n) {
		auto copy = it;
		return copy -= n;
	}

	template<typename ...Iterators>
	class alternated_iterator_impl<std::input_iterator_tag, Iterators...> {
	public:
		using value_type = std::common_type_t<typename Iterators::value_type...>;
		using reference = std::add_lvalue_reference_t<value_type>;
		using pointer = std::add_pointer_t<value_type>;
		using difference_type = std::common_type_t<typename Iterators::difference_type...>;
		using iterator_category = std::input_iterator_tag;

		alternated_iterator_impl(Iterators... iterators, std::size_t index=0)
		: alternated_iterator_impl(iterators..., std::index_sequence_for<Iterators...>(), index) {
		}
		alternated_iterator_impl() = delete;
		alternated_iterator_impl(const alternated_iterator_impl& other) = delete;

		friend alternated_iterator_impl& operator++<iterator_category, Iterators...>(alternated_iterator_impl&);

		reference operator*() const {
			return std::visit(
				[](auto&& it) -> reference { return *it; },
				_its[_index % sizeof...(Iterators)]
			);
		}
		bool operator!=(const alternated_iterator_impl& other) const {
			return _logical_neq(other, std::index_sequence_for<Iterators...>());
		}
		bool operator==(const alternated_iterator_impl& other) const {
			return _logical_eqeq(other, std::index_sequence_for<Iterators...>());
		}

	protected:
		template<std::size_t ...I>
		alternated_iterator_impl(Iterators... iterators, std::index_sequence<I...>, std::size_t index=0)
		: _its{std::variant<Iterators...>(std::in_place_index_t<I>(), iterators)...},
		_index(index) {
		}

		template<std::size_t ...I>
		bool _logical_neq(const alternated_iterator_impl& other, std::index_sequence<I...>) const {
			return (... && (std::get<I>(_its) != std::get<I>(other._its)));
		}

		template<std::size_t ...I>
		bool _logical_eqeq(const alternated_iterator_impl& other, std::index_sequence<I...>) const {
			return (... && (std::get<I>(_its) == std::get<I>(other._its)));
		}

		std::array<std::variant<Iterators...>, sizeof...(Iterators)> _its;
		std::size_t _index;
	};

	template<typename ...Iterators>
	class alternated_iterator_impl<std::forward_iterator_tag, Iterators...> : public alternated_iterator_impl<std::input_iterator_tag, Iterators...> {
	public:
		using value_type = std::common_type_t<typename Iterators::value_type...>;
		using reference = std::add_lvalue_reference_t<value_type>;
		using pointer = std::add_pointer_t<value_type>;
		using difference_type = std::common_type_t<typename Iterators::difference_type...>;
		using iterator_category = std::forward_iterator_tag;

		using alternated_iterator_impl<std::input_iterator_tag, Iterators...>::alternated_iterator_impl;

		alternated_iterator_impl(const alternated_iterator_impl&) = default;
		alternated_iterator_impl() = default;

		friend alternated_iterator_impl& operator++<iterator_category, Iterators...>(alternated_iterator_impl&);
		friend alternated_iterator_impl operator++<iterator_category, Iterators...>(alternated_iterator_impl&, int);

	protected:
	};
	template<typename ...Iterators>
	class alternated_iterator_impl<std::bidirectional_iterator_tag, Iterators...> : public alternated_iterator_impl<std::forward_iterator_tag, Iterators...> {
	public:
		using value_type = std::common_type_t<typename Iterators::value_type...>;
		using reference = std::add_lvalue_reference_t<value_type>;
		using pointer = std::add_pointer_t<value_type>;
		using difference_type = std::common_type_t<typename Iterators::difference_type...>;
		using iterator_category = std::bidirectional_iterator_tag;

		using alternated_iterator_impl<std::forward_iterator_tag, Iterators...>::alternated_iterator_impl;

		friend alternated_iterator_impl& operator++<iterator_category, Iterators...>(alternated_iterator_impl&);
		friend alternated_iterator_impl operator++<iterator_category, Iterators...>(alternated_iterator_impl&, int);
		friend alternated_iterator_impl& operator--<iterator_category, Iterators...>(alternated_iterator_impl&);
		friend alternated_iterator_impl operator--<iterator_category, Iterators...>(alternated_iterator_impl&, int);

	protected:
	};
	template<typename ...Iterators>
	class alternated_iterator_impl<std::random_access_iterator_tag, Iterators...> : public alternated_iterator_impl<std::bidirectional_iterator_tag, Iterators...> {
	public:
		using value_type = std::common_type_t<typename Iterators::value_type...>;
		using reference = std::add_lvalue_reference_t<value_type>;
		using pointer = std::add_pointer_t<value_type>;
		using difference_type = std::common_type_t<typename Iterators::difference_type...>;
		using iterator_category = std::random_access_iterator_tag;

		using alternated_iterator_impl<std::bidirectional_iterator_tag, Iterators...>::alternated_iterator_impl;

		friend alternated_iterator_impl& operator++<iterator_category, Iterators...>(alternated_iterator_impl&);
		friend alternated_iterator_impl operator++<iterator_category, Iterators...>(alternated_iterator_impl&, int);
		friend alternated_iterator_impl& operator--<iterator_category, Iterators...>(alternated_iterator_impl&);
		friend alternated_iterator_impl operator--<iterator_category, Iterators...>(alternated_iterator_impl&, int);
		friend alternated_iterator_impl& operator+=<iterator_category, Iterators...>(alternated_iterator_impl&, difference_type);
		friend alternated_iterator_impl operator+<iterator_category, Iterators...>(const alternated_iterator_impl&, difference_type);
		friend alternated_iterator_impl& operator-=<iterator_category, Iterators...>(alternated_iterator_impl&, difference_type);
		friend alternated_iterator_impl operator-<iterator_category, Iterators...>(const alternated_iterator_impl&, difference_type);

		// TODO: add relational operators <, <=, >, >=
	protected:
	};

} // namespace detail

template<typename ...Iterators>
using alternated_iterator = detail::alternated_iterator_impl<
	std::common_type_t<typename Iterators::iterator_category...>,
	Iterators...
>;

template<typename ...Iterables>
alternated_iterator<typename Iterables::iterator...> alternated_begin(Iterables&... iterables) {
	return alternated_iterator<typename Iterables::iterator...>(std::begin(iterables)...);
}

template<typename ...Iterables>
alternated_iterator<typename Iterables::iterator...> alternated_end(Iterables&... iterables) {
	return alternated_iterator<typename Iterables::iterator...>(std::end(iterables)...);
}

template<typename ...Iterables>
class alternated_range {
public:
	using iterator = alternated_iterator<typename std::remove_reference_t<Iterables>::iterator...>;
	using value_type = typename iterator::value_type;
	using reference = typename iterator::reference;
	using pointer = typename iterator::pointer;

	alternated_range(Iterables&&... iterables) : t(std::forward<Iterables>(iterables)...) {
	}
	iterator begin() { return std::apply(alternated_begin<std::remove_reference_t<Iterables>...>, t); }
	iterator end() { return std::apply(alternated_end<std::remove_reference_t<Iterables>...>, t); }
	std::size_t size() const { return _size(); }
private:
	template<std::size_t I>
	std::size_t _size() const {
		if constexpr (I < sizeof...(Iterables)-1) {
			return std::min(_element_size<I>(), _element_size<I+1>());
		}
		return _element_size<I>();
	}
	template<std::size_t I>
	std::size_t _element_size() const {
		// the size of a cycle_range_impl would overflow due to multiplication or addition
		// add a special case to handle cycle_range_impls
		using T = std::tuple_element_t<I, decltype(t)>;
		if constexpr(std::is_same_v<std::remove_reference_t<T>, cycle_range<T>>) {
			return std::numeric_limits<std::size_t>::max();
		}
		return std::get<I>(t).size() * sizeof...(Iterables) + I;
	}
	std::tuple<Iterables...> t;
};
template<typename ...Iterables>
alternated_range(Iterables&&...) -> alternated_range<Iterables...>;

} // namespace iterutils

#endif

