#ifndef _ITERUTILS_CHAINED_RANGE_H_
#define _ITERUTILS_CHAINED_RANGE_H_

#include <array>
#include <variant>
#include <tuple>
#include "detail.h"

namespace iterutils {

template<typename ...Iterators>
class chained_iterator {
public:
	using value_type = std::common_type_t<typename Iterators::value_type...>;
	using pointer = value_type*;
	using reference = value_type&;

	// need to use in_place_index_t to disambiguate the variant's index in case multiple types are the same
	template<std::size_t ...I>
	chained_iterator(std::index_sequence<I...>, Iterators... begins, Iterators... ends, std::size_t index)
		: _begins{element_type(std::in_place_index_t<I>(), begins)...},
		  _ends{element_type(std::in_place_index_t<I>(), ends)...},
		  _index(index) {
	}
	chained_iterator(Iterators... begins, Iterators... ends, std::size_t index)
		: chained_iterator(std::index_sequence_for<Iterators...>(), begins..., ends..., index) {
	}
	reference operator*() const {
		return *std::visit([](auto& it) { return it; }, _begins[_index]);
	}
	chained_iterator& operator++() {
		std::visit([](auto& it) { ++it; }, _begins[_index]);
		if (_index < sizeof...(Iterators) and _begins[_index] == _ends[_index])
			++_index;
		return *this;
	}
	chained_iterator operator++(int) {
		chained_iterator copy = *this;
		++(*this);
		return copy;
	}
	bool operator!=(const chained_iterator& other) const {
		if (_index == other._index) {
			// both iterators are in the same subrange
			if (_index < sizeof...(Iterators)) {
				return _begins[_index] != other._begins[other._index];
			}
			// both iterators are past the last subrange. essentially, they are equal
			else {
				return false;
			}
		}
		// the iterators are in different subranges. they are not equal
		return true;
	}
private:
	using element_type = std::variant<Iterators...>;
	std::array<element_type, sizeof...(Iterators)> _begins;
	std::array<element_type, sizeof...(Iterators)> _ends;
	std::size_t _index;
};

// template specialization for only two subranges
// should be more efficient
template<typename Iterator1, typename Iterator2>
class chained_iterator<Iterator1, Iterator2> {
private:
	Iterator1 it1;
	Iterator1 it1_end;
	Iterator2 it2;
public:
	using value_type = std::common_type_t<typename Iterator1::value_type, typename Iterator2::value_type>;
	using pointer = value_type*;
	using reference = value_type&;

	chained_iterator(Iterator1 pos1, Iterator1 middle, Iterator2 pos2) : it1(pos1), it1_end(middle), it2(pos2) {
	}
	reference operator*() const {
		if (it1 != it1_end)
			return *it1;
		else
			return *it2;
	}
	chained_iterator& operator++() {
		if (it1 != it1_end)
			++it1;
		else
			++it2;
		return *this;
	}
	chained_iterator operator++(int) {
		chained_iterator copy = *this;
		++(*this);
		return copy;
	}
	bool operator!=(const chained_iterator& other) const {
		if (it1 != it1_end and other.it1 != other.it1_end)
			return it1 != other.it1;
		else if (it1 == it1_end and other.it1 == other.it1_end)
			return it2 != other.it2;
		return true;
	}
};

template<typename ...Iterables>
class chained_range_impl {
public:
	using iterator = chained_iterator<typename std::remove_reference_t<Iterables>::iterator...>;
	using value_type = typename iterator::value_type;
	using reference = typename iterator::reference;
	using pointer = typename iterator::pointer;
	chained_range_impl (detail::arg_from_uref_t<Iterables>... iterables)
		: t(std::forward<Iterables>(iterables)...) {
	}
	iterator begin() { return _begin(std::index_sequence_for<Iterables...>()); }
	iterator end() { return _end(std::index_sequence_for<Iterables...>()); }
	std::size_t size() const { return _size(std::index_sequence_for<Iterables...>()); }
private:
	template<std::size_t ...I>
	iterator _begin(std::index_sequence<I...>) {
		return iterator(std::begin(std::get<I>(t))..., std::end(std::get<I>(t))..., 0);
	}
	template<std::size_t ...I>
	iterator _end(std::index_sequence<I...>) {
		return iterator(std::end(std::get<I>(t))..., std::end(std::get<I>(t))..., sizeof...(Iterables));
	}
	template<std::size_t ...I>
	std::size_t _size(std::index_sequence<I...>) const { return (std::get<I>(t) + ...); }
	std::tuple<Iterables...> t;
};

template<typename Iterable1, typename Iterable2>
class chained_range_impl<Iterable1, Iterable2> {
public:
	using iterator = chained_iterator<typename std::remove_reference_t<Iterable1>::iterator, typename std::remove_reference_t<Iterable2>::iterator>;
	using value_type = typename iterator::value_type;
	using reference = typename iterator::reference;
	using pointer = typename iterator::pointer;

	chained_range_impl(detail::arg_from_uref_t<Iterable1> iterable1, detail::arg_from_uref_t<Iterable2> iterable2)
		: iter1(std::forward<Iterable1>(iterable1)), iter2(std::forward<Iterable2>(iterable2)) {
	}
	iterator begin() { return iterator(std::begin(iter1), std::end(iter1), std::begin(iter2)); }
	iterator end() { return iterator(std::end(iter1), std::end(iter1), std::end(iter2)); }
	std::size_t size() const { return iter1.size() + iter2.size(); }
private:
	Iterable1 iter1;
	Iterable2 iter2;
};

// need universal reference to determine whether to store
// copies (in case arguments are rvalues) or references
// (in case arguments are lvalues)
template<typename ...Iterables>
auto chained_range(Iterables&&... iterables) {
	return chained_range_impl<Iterables...>(std::forward<Iterables>(iterables)...);
}

}

#endif
