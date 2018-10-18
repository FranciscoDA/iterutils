#ifndef _ITERUTILS_ALTERNATED_RANGE_H_
#define _ITERUTILS_ALTERNATED_RANGE_H_

#include <variant>
#include <utility>
#include <array>
#include <iterator>
#include "util.h"

namespace iterutils {

template<typename Tag, typename ...Iterators>
class alternated_iterator {};

template<typename Tag, typename ...Iterators>
alternated_iterator<Tag, Iterators...>& operator++(alternated_iterator<Tag, Iterators...>& it) {
	std::visit([](auto&& item){ ++item; }, it._its[it._index]);
	it._index = (it._index+1) % sizeof...(Iterators);
	return it;
}
template<typename Tag, typename ...Iterators>
alternated_iterator<Tag, Iterators...> operator++(alternated_iterator<Tag, Iterators...>& it, int) {
	auto copy = it;
	++copy;
	return it;
}
template<typename Tag, typename ...Iterators>
alternated_iterator<Tag, Iterators...>& operator--(alternated_iterator<Tag, Iterators...>& it) {
	// decreasing a 0 index would wrap to SIZE_MAX so we wrap to sizeof(Iterators) instead
	if (it._index == 0)
		it._index = sizeof...(Iterators)-1;
	else
		--it._index;
	std::visit([](auto&& item){ --item; }, it._its[it._index]);
	return it;
}
template<typename Tag, typename ...Iterators>
alternated_iterator<Tag, Iterators...> operator--(alternated_iterator<Tag, Iterators...>& it, int) {
	auto copy = it;
	--copy;
	return it;
}
template<typename Tag, typename ...Iterators>
alternated_iterator<Tag, Iterators...>&
operator+=(alternated_iterator<Tag, Iterators...>& it, typename alternated_iterator<Tag, Iterators...>::difference_type n) {
	while (n) {
		auto x = (n + sizeof...(Iterators)-1) / sizeof...(Iterators);
		std::visit([x](auto&& item){ item += x; }, it._its[it._index]);
		n -= x;
		it._index = (it._index+1) % sizeof...(Iterators);
	}
	return it;
}
template<typename Tag, typename ...Iterators>
alternated_iterator<Tag, Iterators...>
operator+(const alternated_iterator<Tag, Iterators...>& it, typename alternated_iterator<Tag, Iterators...>::difference_type n) {
	auto copy = it;
	return copy += it;
}
template<typename Tag, typename ...Iterators>
alternated_iterator<Tag, Iterators...>&
operator-=(alternated_iterator<Tag, Iterators...>& it, typename alternated_iterator<Tag, Iterators...>::difference_type n) {
	while (n) {
		auto x = (n + sizeof...(Iterators)-1) / sizeof...(Iterators);
		std::visit([x](auto&& item){ item -= x; }, it._its[it._index]);
		n -= x;
		it._index = (it._index+1) % sizeof...(Iterators);
	}
	return it;
}
template<typename Tag, typename ...Iterators>
alternated_iterator<Tag, Iterators...>
operator-(const alternated_iterator<Tag, Iterators...>& it, typename alternated_iterator<Tag, Iterators...>::difference_type n) {
	auto copy = it;
	return copy -= n;
}

template<typename ...Iterators>
class alternated_iterator<std::input_iterator_tag, Iterators...> {
public:
	using value_type        = std::common_type_t<typename std::iterator_traits<Iterators>::value_type...>;
	using reference         = std::add_lvalue_reference_t<value_type>;
	using pointer           = std::add_pointer_t<value_type>;
	using difference_type   = std::common_type_t<typename std::iterator_traits<Iterators>::difference_type...>;
	using iterator_category = std::input_iterator_tag;

	alternated_iterator(Iterators... iterators, std::size_t index=0)
	: alternated_iterator(iterators..., std::index_sequence_for<Iterators...>(), index) {
	}
	alternated_iterator() = delete;
	alternated_iterator(const alternated_iterator& other) = delete;

	friend alternated_iterator& operator++<iterator_category, Iterators...>(alternated_iterator&);

	reference operator*() {
		return std::visit([](auto&& it) -> reference { return *it; }, _its[_index]);
	}
	bool operator!=(const alternated_iterator& other) const {
		return _index != other._index or _its[_index] != other._its[other._index];
	}
	bool operator==(const alternated_iterator& other) const {
		return _logical_eqeq(other, std::index_sequence_for<Iterators...>());
	}

protected:
	template<std::size_t ...I>
	alternated_iterator(Iterators... iterators, std::index_sequence<I...>, std::size_t index=0)
	: _its{std::variant<Iterators...>(std::in_place_index_t<I>(), iterators)...},
	_index(index) {
	}
	template<std::size_t ...I>
	bool _logical_eqeq(const alternated_iterator& other, std::index_sequence<I...>) const {
		return (... && (std::get<I>(_its) == std::get<I>(other._its)));
	}

	std::array<std::variant<Iterators...>, sizeof...(Iterators)> _its;
	std::size_t _index;
};

template<typename ...Iterators>
class alternated_iterator<std::forward_iterator_tag, Iterators...>
: public alternated_iterator<std::input_iterator_tag, Iterators...> {
public:
	using value_type        = std::common_type_t<typename std::iterator_traits<Iterators>::value_type...>;
	using reference         = std::add_lvalue_reference_t<value_type>;
	using pointer           = std::add_pointer_t<value_type>;
	using difference_type   = std::common_type_t<typename std::iterator_traits<Iterators>::difference_type...>;
	using iterator_category = std::forward_iterator_tag;

	using alternated_iterator<std::input_iterator_tag, Iterators...>::alternated_iterator;

	alternated_iterator(const alternated_iterator&) = default;
	alternated_iterator() = default;

	friend alternated_iterator& operator++<iterator_category, Iterators...>(alternated_iterator&);
	friend alternated_iterator  operator++<iterator_category, Iterators...>(alternated_iterator&, int);

protected:
};

template<typename ...Iterators>
class alternated_iterator<std::bidirectional_iterator_tag, Iterators...>
: public alternated_iterator<std::forward_iterator_tag, Iterators...> {
public:
	using value_type        = std::common_type_t<typename std::iterator_traits<Iterators>::value_type...>;
	using reference         = std::add_lvalue_reference_t<value_type>;
	using pointer           = std::add_pointer_t<value_type>;
	using difference_type   = std::common_type_t<typename std::iterator_traits<Iterators>::difference_type...>;
	using iterator_category = std::bidirectional_iterator_tag;

	using alternated_iterator<std::forward_iterator_tag, Iterators...>::alternated_iterator;

	friend alternated_iterator& operator++<iterator_category, Iterators...>(alternated_iterator&);
	friend alternated_iterator  operator++<iterator_category, Iterators...>(alternated_iterator&, int);
	friend alternated_iterator& operator--<iterator_category, Iterators...>(alternated_iterator&);
	friend alternated_iterator  operator--<iterator_category, Iterators...>(alternated_iterator&, int);

protected:
};

template<typename ...Iterators>
class alternated_iterator<std::random_access_iterator_tag, Iterators...>
: public alternated_iterator<std::bidirectional_iterator_tag, Iterators...> {
public:
	using value_type        = std::common_type_t<typename std::iterator_traits<Iterators>::value_type...>;
	using reference         = std::add_lvalue_reference_t<value_type>;
	using pointer           = std::add_pointer_t<value_type>;
	using difference_type   = std::common_type_t<typename std::iterator_traits<Iterators>::difference_type...>;
	using iterator_category = std::random_access_iterator_tag;

	using alternated_iterator<std::bidirectional_iterator_tag, Iterators...>::alternated_iterator;

	friend alternated_iterator& operator++<iterator_category, Iterators...>(alternated_iterator&);
	friend alternated_iterator  operator++<iterator_category, Iterators...>(alternated_iterator&, int);
	friend alternated_iterator& operator--<iterator_category, Iterators...>(alternated_iterator&);
	friend alternated_iterator  operator--<iterator_category, Iterators...>(alternated_iterator&, int);
	friend alternated_iterator& operator+=<iterator_category, Iterators...>(alternated_iterator&, difference_type);
	friend alternated_iterator  operator+ <iterator_category, Iterators...>(const alternated_iterator&, difference_type);
	friend alternated_iterator& operator-=<iterator_category, Iterators...>(alternated_iterator&, difference_type);
	friend alternated_iterator  operator- <iterator_category, Iterators...>(const alternated_iterator&, difference_type);

	// TODO: add relational operators <, <=, >, >=
protected:
};

template<typename ...Iterators>
alternated_iterator(Iterators...) -> alternated_iterator<
	std::common_type_t<typename Iterators::iterator_category...>,
	Iterators...
>;

template<typename ...Iterables>
detail::specialize_iterator_from_iterables<alternated_iterator, Iterables...> alternated_begin(Iterables&... iterables) {
	return {std::begin(iterables)...};
}

template<typename ...Iterables>
detail::specialize_iterator_from_iterables<alternated_iterator, Iterables...> alternated_end(Iterables&... iterables) {
	auto shortest = shortest_iterable(iterables...);
	return {std::end(iterables)..., shortest.second};
}

template<typename ...Iterables>
class alternated_range {
public:
	using iterator   = detail::specialize_iterator_from_iterables<alternated_iterator, Iterables...>;
	using value_type = std::common_type_t<typename std::remove_reference_t<Iterables>::value_type...>;
	using reference  = std::common_type_t<typename std::remove_reference_t<Iterables>::reference...>;
	using pointer    = std::common_type_t<typename std::remove_reference_t<Iterables>::pointer...>;

	alternated_range(Iterables&&... iterables) : t(std::forward<Iterables>(iterables)...) {
	}
	iterator begin() { return std::apply(alternated_begin<std::remove_reference_t<Iterables>...>, t); }
	iterator end() { return std::apply(alternated_end<std::remove_reference_t<Iterables>...>, t); }

	std::enable_if_t<!is_infinite<alternated_range>::value,	size_t>
	size() const {
		auto shortest = std::apply(shortest_iterable<Iterables...>, t);
		return shortest.first * sizeof...(Iterables) + shortest.second;
	}
private:
	std::tuple<Iterables...> t;
};
template<typename ...Iterables>
alternated_range(Iterables&&...) -> alternated_range<Iterables...>;

template<typename ...T>
struct is_infinite<alternated_range<T...> > : public std::conjunction<is_infinite<T>...> {};

} // namespace iterutils

#endif

