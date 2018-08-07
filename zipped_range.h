#ifndef _ITERUTILS_ZIPPED_RANGE_H_
#define _ITERUTILS_ZIPPED_RANGE_H_

#include <tuple>

namespace iterutils {

template<typename ...Iterators>
class zipped_iterator {
public:
	using value_type = std::tuple<typename Iterators::value_type...>;
	using pointer = std::tuple<typename Iterators::pointer...>;
	using reference = std::tuple<typename Iterators::reference...>;
	using iterator_category = std::input_iterator_tag;
	using difference_type = std::common_type_t<typename Iterators::difference_type...>;

	// initialize the tuple
	zipped_iterator(Iterators... args) : t(args...) {
	}

	// these operators act as a proxy to the private methods
	zipped_iterator& operator++() { return _prefix_inc(std::index_sequence_for<Iterators...>()); }
	zipped_iterator operator++(int) {
		zipped_iterator copy = *this;
		++(*this);
		return copy;
	}
	reference operator*() const { return _deref(std::index_sequence_for<Iterators...>()); }
	bool operator!=(const zipped_iterator& other) const { return _logical_neq(other, std::index_sequence_for<Iterators...>()); }

	template<std::size_t I>
	auto& get() const { return std::get<I>(t); }

private:
	std::tuple<Iterators...> t;

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

template<typename ...Iterables>
auto zipped_begin(Iterables&... args) {
	return zipped_iterator<typename Iterables::iterator...>(std::begin(args)...);
}

template<typename ...Iterables>
auto zipped_cbegin(const Iterables&... args) {
	return zipped_iterator<typename Iterables::const_iterator...>(std::cbegin(args)...);
}

template<typename ...Iterables>
auto zipped_end(Iterables&... args) {
	return zipped_iterator<typename Iterables::iterator...>(std::end(args)...);
}

template<typename ...Iterables>
auto zipped_cend(const Iterables&... args) {
	return zipped_iterator<typename Iterables::const_iterator...>(std::cend(args)...);
}

template<typename ...Iterables>
class zipped_range_impl {
public:
	using iterator = zipped_iterator<typename std::remove_reference_t<Iterables>::iterator...>;
	using value_type = typename iterator::value_type;
	using pointer = typename iterator::pointer;
	using reference = typename iterator::reference;

	zipped_range_impl (std::add_rvalue_reference_t<Iterables>... iterables)
		: t(std::forward<Iterables>(iterables)...) {
	}
	iterator begin() { return std::apply(zipped_begin<std::remove_reference_t<Iterables>...>, t); }
	iterator end() { return std::apply(zipped_end<std::remove_reference_t<Iterables>...>, t); }
	std::size_t size() const { return _size<0>(); }
private:
	template<std::size_t I>
	std::size_t _size() const {
		if constexpr (I < sizeof...(Iterables)-1) {
			return std::min(std::get<I>(t).size(), _size<I+1>());
		}
		return std::get<I>(t).size();
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

} // namespace iterutils

#endif

