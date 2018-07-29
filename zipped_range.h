#ifndef _ZIPPED_RANGE_H_
#define _ZIPPED_RANGE_H_

#include <tuple>

template<typename Iterator, typename ...Iterators>
class zipped_iterator {
private:
	using Seq = std::make_index_sequence<sizeof...(Iterators)+1>;
public:
	using value_type = std::tuple<typename Iterator::value_type, typename Iterators::value_type...>;
	using pointer = std::tuple<typename Iterator::pointer, typename Iterators::pointer...>;
	using reference = std::tuple<typename Iterator::reference, typename Iterators::reference...>;
	using iterator_category = std::input_iterator_tag;
	using difference_type = typename Iterator::difference_type;

	// initialize the tuple
	zipped_iterator(Iterator arg1, Iterators... args) : t(arg1, args...) {
	}

	// these operators act as a proxy to the private methods
	zipped_iterator& operator++() { return _prefix_inc(Seq{}); }
	reference operator*() const { return _deref(Seq{}); }
	bool operator!=(const zipped_iterator& other) const { return _logical_neq(other, Seq{}); }

	template<std::size_t I>
	auto& get() const { return std::get<I>(t); }

private:
	std::tuple<Iterator, Iterators...> t;

	// use a fold expression to increment all the inner iterators
	template<std::size_t ...I>
	zipped_iterator& _prefix_inc(std::index_sequence<I...>) {
		(... , ++std::get<I>(t));
		return *this;
	}

	// return a tuple of item references when dereferencing the iterator
	template<std::size_t ...I>
	reference _deref(std::index_sequence<I...>) const {
		return reference(*std::get<I>(t)...);
	}

	// use a fold expression that returns true only when all corresponding iterators are different
	// this causes the iteration to stop at the shortest range
	template<std::size_t ...I>
	bool _logical_neq(const zipped_iterator& other, std::index_sequence<I...>) const {
		return (... && (std::get<I>(t)!=std::get<I>(other.t)));
	}
};

template<typename Iterable, typename ...Iterables>
zipped_iterator<typename Iterable::iterator, typename Iterables::iterator...>
zipped_begin(Iterable& arg1, Iterables&... args) {
	return zipped_iterator(std::begin(arg1), std::begin(args)...);
}
template<typename Iterable, typename ...Iterables>
zipped_iterator<typename Iterable::const_iterator, typename Iterables::const_iterator...>
zipped_cbegin(const Iterable& arg1, const Iterables&... args) {
	return zipped_iterator(std::cbegin(arg1), std::cbegin(args)...);
}
template<typename Iterable, typename ...Iterables>
zipped_iterator<typename Iterable::iterator, typename Iterables::iterator...>
zipped_end(Iterable& arg1, Iterables&... args) {
	return zipped_iterator(std::end(arg1), std::end(args)...);
}
template<typename Iterable, typename ...Iterables>
zipped_iterator<typename Iterable::const_iterator, typename Iterables::const_iterator...>
zipped_cend(const Iterable& arg1, const Iterables&... args) {
	return iterator(std::cend(arg1), std::cend(args)...);
}

// Range implementation class. Should not be instantiated explicitly
// Use the zipped_range() functions instead
template<typename Container, typename ...Args>
class zipped_range {
public:
	using iterator = zipped_iterator<typename Container::iterator, typename Args::iterator...>;
	using value_type = typename iterator::value_type;
	using reference = typename iterator::reference;
	using pointer = typename iterator::pointer;
	zipped_range(Container& arg1, Args&... args) : _begin(zipped_begin(arg1, args...)), _end(zipped_end(arg1, args...)) {
	}
	zipped_range(Container&& arg1, Args&&... args) : _begin(zipped_begin(arg1, args...)), _end(zipped_end(arg1, args...)) {
	}
	iterator begin() { return _begin; }
	iterator end() { return _end; }
private:
	iterator _begin;
	iterator _end;
};

#endif

