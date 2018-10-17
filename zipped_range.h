#ifndef _ITERUTILS_ZIPPED_RANGE_H_
#define _ITERUTILS_ZIPPED_RANGE_H_

#include <tuple>
#include "util.h"

namespace iterutils {

template<typename Tag, typename ...Iterators>
class zipped_iterator {};

template<typename Tag, typename ...Iterators>
zipped_iterator<Tag, Iterators...>& operator++(zipped_iterator<Tag, Iterators...>& it) {
	it._prefix_inc(std::index_sequence_for<Iterators...>());
	return it;
}
template<typename Tag, typename ...Iterators>
zipped_iterator<Tag, Iterators...> operator++(zipped_iterator<Tag, Iterators...>& it, int) {
	auto copy = it;
	it._suffix_inc(std::index_sequence_for<Iterators...>());
	return copy;
}
template<typename Tag, typename ...Iterators>
zipped_iterator<Tag, Iterators...>& operator--(zipped_iterator<Tag, Iterators...>& it) {
	it._prefix_dec(std::index_sequence_for<Iterators...>());
	return it;
}
template<typename Tag, typename ...Iterators>
zipped_iterator<Tag, Iterators...> operator--(zipped_iterator<Tag, Iterators...>& it, int) {
	auto copy = it;
	it._suffix_dec(std::index_sequence_for<Iterators...>());
	return copy;
}
template<typename Tag, typename ...Iterators>
zipped_iterator<Tag, Iterators...>&
operator+=(zipped_iterator<Tag, Iterators...>& it, typename zipped_iterator<Tag, Iterators...>::difference_type n) {
	it._inplace_add(n, std::index_sequence_for<Iterators...>());
	return it;
}
template<typename Tag, typename ...Iterators>
zipped_iterator<Tag, Iterators...>
operator+(const zipped_iterator<Tag, Iterators...>& it, typename zipped_iterator<Tag, Iterators...>::difference_type n) {
	auto copy = it;
	copy._inplace_add(n, std::index_sequence_for<Iterators...>());
	return copy;
}
template<typename Tag, typename ...Iterators>
zipped_iterator<Tag, Iterators...>&
operator-=(zipped_iterator<Tag, Iterators...>& it, typename zipped_iterator<Tag, Iterators...>::difference_type n) {
	it._inplace_sub(n, std::index_sequence_for<Iterators...>());
	return it;
}
template<typename Tag, typename ...Iterators>
zipped_iterator<Tag, Iterators...>
operator-(const zipped_iterator<Tag, Iterators...>& it, typename zipped_iterator<Tag, Iterators...>::difference_type n) {
	zipped_iterator<Tag, Iterators...> copy = it;
	copy._inplace_sub(n, std::index_sequence_for<Iterators...>());
	return copy;
}

template<typename ...Iterators>
class zipped_iterator<std::input_iterator_tag, Iterators...> {
public:
	using value_type = std::tuple<typename Iterators::value_type...>;
	using pointer = std::tuple<typename Iterators::pointer...>;
	using reference = std::tuple<typename Iterators::reference...>;
	using difference_type = std::common_type_t<typename Iterators::difference_type...>;
	using iterator_category = std::input_iterator_tag;

	zipped_iterator(Iterators... args) : t(args...) {
	}
	// some iterators are non-copyable so the zipped iterator is not copyable either
	zipped_iterator(const zipped_iterator& other) = delete;
	// some iterators are not default constructible so the zipped iterator is not default constructible either
	zipped_iterator() = delete;

	// input iterator operators
	friend zipped_iterator& operator++<iterator_category, Iterators...>(zipped_iterator&);

	reference operator*() const {
		return _deref(std::index_sequence_for<Iterators...>());
	}
	bool operator!=(const zipped_iterator& other) const {
		return _logical_neq(other, std::index_sequence_for<Iterators...>());
	}
	bool operator==(const zipped_iterator& other) const {
		return !_logical_neq(other, std::index_sequence_for<Iterators...>());
	}

	template<std::size_t I>
	auto& get() const { return std::get<I>(t); }
protected:
	std::tuple<Iterators...> t;

	// use a fold expression to increment all the inner iterators
	template<std::size_t ...I>
	void _prefix_inc(std::index_sequence<I...>) {
		(... , ++std::get<I>(t));
	}

	// return a tuple of item references when dereferencing the iterator
	template<std::size_t ...I>
	reference _deref(std::index_sequence<I...>) const {
		return reference(*std::get<I>(t)...);
	}

	// return true if all the iterators in t are different from their counterparts in other
	// this ensures that iteration will stop on the shortest range
	template<std::size_t ...I>
	bool _logical_neq(const zipped_iterator& other, std::index_sequence<I...>) const {
		return (... && (std::get<I>(t)!=std::get<I>(other.t)));
	}
};

template<typename ...Iterators>
class zipped_iterator<std::forward_iterator_tag, Iterators...> : public zipped_iterator<std::input_iterator_tag, Iterators...> {
public:
	using value_type = typename zipped_iterator<std::input_iterator_tag, Iterators...>::value_type;
	using reference = typename zipped_iterator<std::input_iterator_tag, Iterators...>::reference;
	using pointer = typename zipped_iterator<std::input_iterator_tag, Iterators...>::pointer;
	using difference_type = typename zipped_iterator<std::input_iterator_tag, Iterators...>::difference_type;
	using iterator_category = std::forward_iterator_tag;

	zipped_iterator(Iterators... args) : zipped_iterator<std::input_iterator_tag, Iterators...>(args...) {
	}
	zipped_iterator(const zipped_iterator& other) : zipped_iterator<std::input_iterator_tag, Iterators...>(other.t) {
	}
	zipped_iterator() {
	}
	// methods copied over from base iterators
	friend zipped_iterator& operator++<iterator_category, Iterators...>(zipped_iterator&);
	// forward iterator operators
	friend zipped_iterator operator++<iterator_category, Iterators...>(zipped_iterator&, int);
protected:
	template<std::size_t ...I>
	void _suffix_inc(std::index_sequence<I...>) {
		(... , (std::get<I>(this->t)++));
	}
};

template<typename ...Iterators>
class zipped_iterator<std::bidirectional_iterator_tag, Iterators...> : public zipped_iterator<std::forward_iterator_tag, Iterators...> {
public:
	using value_type = typename zipped_iterator<std::input_iterator_tag, Iterators...>::value_type;
	using reference = typename zipped_iterator<std::input_iterator_tag, Iterators...>::reference;
	using pointer = typename zipped_iterator<std::input_iterator_tag, Iterators...>::pointer;
	using difference_type = typename zipped_iterator<std::input_iterator_tag, Iterators...>::difference_type;
	using iterator_category = std::bidirectional_iterator_tag;

	using zipped_iterator<std::forward_iterator_tag, Iterators...>::zipped_iterator;
	// methods copied over from base iterators
	friend zipped_iterator& operator++<iterator_category, Iterators...>(zipped_iterator&);
	friend zipped_iterator operator++<iterator_category, Iterators...>(zipped_iterator&, int);
	// bidirectional iterator operators
	friend zipped_iterator& operator--<iterator_category, Iterators...>(zipped_iterator&);
	friend zipped_iterator operator--<iterator_category, Iterators...>(zipped_iterator&, int);
protected:
	// use a fold expression to decrement all the inner iterators
	template<std::size_t ...I>
	void _prefix_dec(std::index_sequence<I...>) {
		(... , --std::get<I>(this->t));
	}
	template<std::size_t ...I>
	void _suffix_dec(std::index_sequence<I...>) {
		(... , (std::get<I>(this->t)--));
	}
};

template<typename ...Iterators>
class zipped_iterator<std::random_access_iterator_tag, Iterators...> : public zipped_iterator<std::bidirectional_iterator_tag, Iterators...> {
public:
	using value_type = typename zipped_iterator<std::input_iterator_tag, Iterators...>::value_type;
	using reference = typename zipped_iterator<std::input_iterator_tag, Iterators...>::reference;
	using pointer = typename zipped_iterator<std::input_iterator_tag, Iterators...>::pointer;
	using difference_type = typename zipped_iterator<std::input_iterator_tag, Iterators...>::difference_type;
	using iterator_category = std::random_access_iterator_tag;

	using zipped_iterator<std::bidirectional_iterator_tag, Iterators...>::zipped_iterator;
	// methods copied over from base iterators
	friend zipped_iterator& operator++<iterator_category, Iterators...>(zipped_iterator&);
	friend zipped_iterator operator++<iterator_category, Iterators...>(zipped_iterator&, int);
	friend zipped_iterator& operator--<iterator_category, Iterators...>(zipped_iterator&);
	friend zipped_iterator operator--<iterator_category, Iterators...>(zipped_iterator&, int);
	// random access iterator operators
	friend zipped_iterator& operator+=<iterator_category, Iterators...>(zipped_iterator&, difference_type);
	friend zipped_iterator operator+<iterator_category, Iterators...>(const zipped_iterator&, difference_type);
	friend zipped_iterator& operator-=<iterator_category, Iterators...>(zipped_iterator&, difference_type);
	friend zipped_iterator operator-<iterator_category, Iterators...>(const zipped_iterator&, difference_type);

	bool operator<(const zipped_iterator& other) const {
		return this->t < other.t;
	}
	bool operator<=(const zipped_iterator& other) const {
		return this->t <= other.t;
	}
	bool operator>(const zipped_iterator& other) const {
		return this->t > other.t;
	}
	bool operator>=(const zipped_iterator& other) const {
		return this->t >= other.t;
	}
protected:
	template<std::size_t ...I>
	void _inplace_add(difference_type n, std::index_sequence<I...>) {
		(... , (std::get<I>(this->t)+=n));
	}
	template<std::size_t ...I>
	void _inplace_sub(difference_type n, std::index_sequence<I...>) {
		(... , (std::get<I>(this->t)-=n));
	}
};
template<typename ...Iterators>
zipped_iterator(Iterators...) -> zipped_iterator<std::common_type_t<typename Iterators::iterator_category...>, Iterators...>;

template<typename ...Iterables>
detail::specialize_iterator_from_iterables<zipped_iterator, Iterables...>
zipped_begin(Iterables&... args) {
	return {std::begin(args)...};
}

template<typename ...Iterables>
detail::specialize_const_iterator_from_iterables<zipped_iterator, Iterables...>
zipped_cbegin(const Iterables&... args) {
	return {std::cbegin(args)...};
}

template<typename ...Iterables>
detail::specialize_iterator_from_iterables<zipped_iterator, Iterables...>
zipped_end(Iterables&... args) {
	return {std::end(args)...};
}

template<typename ...Iterables>
detail::specialize_const_iterator_from_iterables<zipped_iterator, Iterables...>
zipped_cend(const Iterables&... args) {
	return {std::cend(args)...};
}

template<typename ...Iterables>
class zipped_range {
public:
	using iterator = detail::specialize_iterator_from_iterables<zipped_iterator, Iterables...>;
	using value_type = typename iterator::value_type;
	using pointer = typename iterator::pointer;
	using reference = typename iterator::reference;

	zipped_range(Iterables&&... iterables)
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
	std::tuple<Iterables...> t;
};
template<typename ...Iterables>
zipped_range(Iterables&&...) -> zipped_range<Iterables...>;


} // namespace iterutils

#endif

