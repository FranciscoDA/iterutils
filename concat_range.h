#ifndef _CONCAT_RANGE_H_
#define _CONCAT_RANGE_H_

#include <array>
#include <variant>
#include <tuple>

template<typename Iterator, typename ...Iterators>
class concat_iterator {
public:
	using value_type = typename Iterator::value_type;
	using pointer = typename Iterator::pointer;
	using reference = typename Iterator::reference;

	// need to use in_place_index_t to disambiguate the tuple's index in case multiple types are the same
	template<std::size_t ...I>
	concat_iterator(std::index_sequence<I...>, Iterator begin1, Iterators... begins, Iterator end1, Iterators... ends, std::size_t index)
		: _begins{element_type(std::in_place_index_t<0>(), begin1), element_type(std::in_place_index_t<I+1>(), begins)...},
		  _ends{element_type(std::in_place_index_t<0>(), end1), element_type(std::in_place_index_t<I+1>(), ends)...},
		  _index(index) {
	}
	concat_iterator(Iterator begin1, Iterators... begins, Iterator end1, Iterators... ends, std::size_t index)
		: concat_iterator(std::make_index_sequence<sizeof...(Iterators)>(), begin1, begins..., end1, ends..., index) {
	}
	reference operator*() const {
		return *std::visit([](auto& it) { return it; }, _begins[_index]);
	}
	concat_iterator& operator++() {
		std::visit([](auto& it) { ++it; }, _begins[_index]);
		if (_index < sizeof...(Iterators)+1 and _begins[_index] == _ends[_index])
			++_index;
		return *this;
	}
	bool operator!=(const concat_iterator& other) const {
		if (_index == other._index) {
			// both iterators are in the same subrange
			if (_index < sizeof...(Iterators)+1) {
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
	using element_type = std::variant<Iterator, Iterators...>;
	std::array<element_type, sizeof...(Iterators)+1> _begins;
	std::array<element_type, sizeof...(Iterators)+1> _ends;
	std::size_t _index;
};

// template specialization for only two subranges
// should be more efficient
template<typename Iterator1, typename Iterator2>
class concat_iterator<Iterator1, Iterator2> {
private:
	Iterator1 it1;
	Iterator1 it1_end;
	Iterator2 it2;
public:
	using value_type = std::enable_if_t<std::is_same_v<typename Iterator1::value_type, typename Iterator2::value_type>, typename Iterator1::value_type>;
	using pointer = std::enable_if_t<std::is_same_v<typename Iterator1::pointer, typename Iterator2::pointer>, typename Iterator1::pointer>;
	using reference = std::enable_if_t<std::is_same_v<typename Iterator1::reference, typename Iterator2::reference>, typename Iterator1::reference>;

	concat_iterator(Iterator1 pos1, Iterator1 middle, Iterator2 pos2) : it1(pos1), it1_end(middle), it2(pos2) {
	}
	reference operator*() const {
		if (it1 != it1_end)
			return *it1;
		else
			return *it2;
	}
	concat_iterator& operator++() {
		if (it1 != it1_end)
			++it1;
		else
			++it2;
		return *this;
	}
	bool operator!=(const concat_iterator& other) const {
		if (it1 != it1_end and other.it1 != other.it1_end)
			return it1 != other.it1;
		else if (it1 == it1_end and other.it1 == other.it1_end)
			return it2 != other.it2;
		return true;
	}
};

template<typename ...Iterables>
class concat_range {
public:
	using iterator = concat_iterator<typename Iterables::iterator...>;
	concat_range(Iterables&... iterables)
		: _begin(std::begin(iterables)..., std::end(iterables)..., 0),
		  _end(std::end(iterables)..., std::end(iterables)..., sizeof...(Iterables)) {
	}
	iterator begin() { return _begin; }
	iterator end() { return _end; }
private:
	iterator _begin;
	iterator _end;
};

template<typename Iterable1, typename Iterable2>
class concat_range<Iterable1, Iterable2> {
public:
	using iterator = concat_iterator<typename Iterable1::iterator, typename Iterable2::iterator>;
	using value_type = typename Iterable1::value_type;
	using reference = typename Iterable1::reference;
	using pointer = typename Iterable1::pointer;

	concat_range(Iterable1& arg1, Iterable2& arg2)
		: _begin(std::begin(arg1), std::end(arg1), std::begin(arg2)), _end(std::end(arg1), std::end(arg1), std::end(arg2)) {
	}
	iterator begin() { return _begin; }
	iterator end() { return _end; }
private:
	iterator _begin;
	iterator _end;
};

#endif
