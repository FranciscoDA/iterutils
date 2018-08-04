#ifndef _ITERUTILS_CYCLE_RANGE_H_
#define _ITERUTILS_CYCLE_RANGE_H_

#include <iterator>

template<typename Iterator>
class cycle_iterator {
public:
	using iterator_tag = std::forward_iterator_tag;
	using value_type = typename Iterator::value_type;
	using reference = typename Iterator::reference;
	using pointer = typename Iterator::pointer;

	cycle_iterator(Iterator begin, Iterator end)
		: _begin(begin), _end(end), _it(begin) {
		
	}
	cycle_iterator& operator++() {
		_it++;
		if (_it == _end) {
			_it = _begin;
		}
		return *this;
	}
	cycle_iterator operator++(int) {
		cycle_iterator copy = *this;
		++copy;
		return copy;
	}
	bool operator!=(const cycle_iterator& other) const {
		return _it == other._it;
	}
private:
	Iterator _begin;
	Iterator _end;
	Iterator _it;
};

template<typename Iterator>
class cycle_iterator_n {
public:
	using iterator_tag = std::forward_iterator_tag;
	using value_type = typename Iterator::value_type;
	using reference = typename Iterator::reference;
	using pointer = typename Iterator::pointer;

	cycle_iterator_n(Iterator begin, Iterator end, std::size_t maxcycles=0)
		: _begin(begin), _end(end), _maxcycles(maxcycles), _it(begin) {
		
	}
	reference operator*() {
		return *_it;
	}
	cycle_iterator_n& operator++() {
		_it++;
		if (_it == _end) {
			if (_maxcycles > 1) {
				--_maxcycles;
				_it = _begin;
			}
			else if (_maxcycles == 1) {
				// don't reset _it if no more cycles left
				_maxcycles = 0;
			}
		}
		return *this;
	}
	cycle_iterator_n operator++(int) {
		cycle_iterator_n copy = *this;
		++copy;
		return copy;
	}
	bool operator!=(const cycle_iterator_n& other) const {
		return _it != other._it or _maxcycles != other._maxcycles;
	}
private:
	std::size_t _maxcycles;
	Iterator _begin;
	Iterator _end;
	Iterator _it;
};

template<typename Iterable>
class cycle_range_n_impl {
public:
	using iterator = cycle_iterator_n<typename std::remove_reference_t<Iterable>::iterator>;
	using value_type = typename iterator::value_type;
	using pointer = typename iterator::pointer;
	using reference = typename iterator::reference;
	
	cycle_range_n_impl(
		std::conditional_t<std::is_lvalue_reference_v<Iterable>, Iterable, Iterable&&> iterable,
		std::size_t maxcycles
	) : _iterable(iterable), _maxcycles(maxcycles) {
	}
	iterator begin() {
		return iterator(std::begin(_iterable), std::end(_iterable), _maxcycles);
	}
	iterator end() {
		return iterator(std::end(_iterable), std::end(_iterable), 0);
	}
private:
	Iterable _iterable;
	std::size_t _maxcycles;
};

template<typename Iterable>
class cycle_range_impl {
public:
	using iterator = cycle_iterator<typename std::remove_reference_t<Iterable>::iterator>;
	using value_type = typename iterator::value_type;
	using pointer = typename iterator::pointer;
	using reference = typename iterator::reference;
	
	cycle_range_impl(
		std::conditional_t<std::is_lvalue_reference_v<Iterable>, Iterable, Iterable&&> iterable
	) : _iterable(iterable) {
	}
	iterator begin() {
		return iterator(std::begin(_iterable), std::end(_iterable));
	}
	iterator end() {
		return iterator(std::end(_iterable), std::end(_iterable));
	}
private:
	Iterable _iterable;
};

template<typename Iterable>
auto cycle_range(Iterable&& iterable) {
	return cycle_range_impl<Iterable>(std::forward<Iterable>(iterable));
}

template<typename Iterable>
auto cycle_range(Iterable&& iterable, std::size_t maxcycles) {
	return cycle_range_n_impl<Iterable>(std::forward<Iterable>(iterable), maxcycles);
}

#endif
