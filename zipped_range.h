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

template<typename ...Iterables>
class zipped_range_impl {
public:
	using iterator = zipped_iterator<typename std::remove_reference_t<Iterables>::iterator...>;
	using value_type = typename iterator::value_type;
	using pointer = typename iterator::pointer;
	using reference = typename iterator::reference;

	zipped_range_impl (
		std::conditional_t<std::is_lvalue_reference_v<Iterables>, Iterables&, Iterables&&>... iterables
	) : t(std::forward<Iterables>(iterables)...) {
	}
	iterator begin() {
		return _begin(std::index_sequence_for<Iterables...>());
	}
	iterator end() {
		return _end(std::index_sequence_for<Iterables...>());
	}
private:
	template<std::size_t ...I>
	iterator _begin(std::index_sequence<I...>) {
		return iterator(std::begin(std::get<I>(t))...);
	}
	template<std::size_t ...I>
	iterator _end(std::index_sequence<I...>) {
		return iterator(std::end(std::get<I>(t))...);
	}

	// store references/copies of the iterables
	// to make sure they stay valid during the lifetime
	// of this object
	std::tuple<Iterables...> t;
};

// need universal reference to determine whether to store
// copies (in case arguments are rvalues) or references
// (in case arguments are lvalues)
template<typename ...Iterables>
auto zipped_range(Iterables&&... iterables) {
	return zipped_range_impl<Iterables...>(std::forward<Iterables>(iterables)...);
}

#endif

