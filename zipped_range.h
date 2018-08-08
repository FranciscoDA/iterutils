#ifndef _ITERUTILS_ZIPPED_RANGE_H_
#define _ITERUTILS_ZIPPED_RANGE_H_

#include <tuple>

namespace iterutils {

namespace detail {

template<typename Tag, typename ...Iterators>
class zipped_iterator_impl {};

template<typename ...Iterators>
class zipped_iterator_impl<std::input_iterator_tag, Iterators...> {
public:
	using value_type = std::tuple<typename Iterators::value_type...>;
	using pointer = std::tuple<typename Iterators::pointer...>;
	using reference = std::tuple<typename Iterators::reference...>;
	using difference_type = std::common_type_t<typename Iterators::difference_type...>;
	using iterator_category = std::input_iterator_tag;

	zipped_iterator_impl(Iterators... args) : t(args...) {
	}
	zipped_iterator_impl() = delete;

	auto& operator++() {
		_prefix_inc(std::index_sequence_for<Iterators...>());
		return *this;
	}
	reference operator*() const {
		return _deref(std::index_sequence_for<Iterators...>());
	}
	bool operator!=(const zipped_iterator_impl& other) const {
		return _logical_neq(other, std::index_sequence_for<Iterators...>());
	}

	template<std::size_t I>
	auto& get() const { return std::get<I>(t); }
protected:
	zipped_iterator_impl(const std::tuple<Iterators...>& _t) : t(_t) {
	}
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

	// use a fold expression that returns true only when all corresponding iterators are different
	// this causes the iteration to stop at the shortest range
	template<std::size_t ...I>
	bool _logical_neq(const zipped_iterator_impl& other, std::index_sequence<I...>) const {
		return (... && (std::get<I>(t)!=std::get<I>(other.t)));
	}
};

template<typename ...Iterators>
class zipped_iterator_impl<std::forward_iterator_tag, Iterators...> : public zipped_iterator_impl<std::input_iterator_tag, Iterators...> {
public:
	using value_type = typename zipped_iterator_impl<std::input_iterator_tag, Iterators...>::value_type;
	using reference = typename zipped_iterator_impl<std::input_iterator_tag, Iterators...>::reference;
	using pointer = typename zipped_iterator_impl<std::input_iterator_tag, Iterators...>::pointer;
	using difference_type = typename zipped_iterator_impl<std::input_iterator_tag, Iterators...>::difference_type;
	using iterator_category = std::forward_iterator_tag;

	zipped_iterator_impl(Iterators... args) : zipped_iterator_impl<std::input_iterator_tag, Iterators...>(args...) {
	}
	zipped_iterator_impl(const zipped_iterator_impl& other) : zipped_iterator_impl<std::input_iterator_tag, Iterators...>(other.t) {
	}
	zipped_iterator_impl() {
	}
	// methods copied over from base iterators
	auto& operator++() {
		_prefix_inc(std::index_sequence_for<Iterators...>());
		return *this;
	}
	// forward iterator operators
	auto operator++(int) {
		auto copy = *this;
		this->_prefix_inc(std::index_sequence_for<Iterators...>());
		return copy;
	}
};

template<typename ...Iterators>
class zipped_iterator_impl<std::bidirectional_iterator_tag, Iterators...> : public zipped_iterator_impl<std::forward_iterator_tag, Iterators...> {
public:
	using value_type = typename zipped_iterator_impl<std::input_iterator_tag, Iterators...>::value_type;
	using reference = typename zipped_iterator_impl<std::input_iterator_tag, Iterators...>::reference;
	using pointer = typename zipped_iterator_impl<std::input_iterator_tag, Iterators...>::pointer;
	using difference_type = typename zipped_iterator_impl<std::input_iterator_tag, Iterators...>::difference_type;
	using iterator_category = std::bidirectional_iterator_tag;

	zipped_iterator_impl(Iterators... args) : zipped_iterator_impl<std::forward_iterator_tag, Iterators...>(args...) {
	}
	zipped_iterator_impl(const zipped_iterator_impl& other) : zipped_iterator_impl<std::forward_iterator_tag, Iterators...>(other) {
	}
	zipped_iterator_impl() {
	}
	// methods copied over from base iterators
	auto& operator++() {
		_prefix_inc(std::index_sequence_for<Iterators...>());
		return *this;
	}
	auto operator++(int) {
		auto copy = *this;
		this->_prefix_inc(std::index_sequence_for<Iterators...>());
		return copy;
	}
	// bidirectional iterator operators
	auto& operator--() {
		_prefix_dec(std::index_sequence_for<Iterators...>());
		return *this;
	}
	auto operator--(int) {
		auto copy = *this;
		_prefix_dec(std::index_sequence_for<Iterators...>());
		return copy;
	}
protected:
	// use a fold expression to decrement all the inner iterators
	template<std::size_t ...I>
	void _prefix_dec(std::index_sequence<I...>) {
		(... , --std::get<I>(this->t));
	}
};

template<typename ...Iterators>
class zipped_iterator_impl<std::random_access_iterator_tag, Iterators...> : public zipped_iterator_impl<std::bidirectional_iterator_tag, Iterators...> {
public:
	using value_type = typename zipped_iterator_impl<std::input_iterator_tag, Iterators...>::value_type;
	using reference = typename zipped_iterator_impl<std::input_iterator_tag, Iterators...>::reference;
	using pointer = typename zipped_iterator_impl<std::input_iterator_tag, Iterators...>::pointer;
	using difference_type = typename zipped_iterator_impl<std::input_iterator_tag, Iterators...>::difference_type;
	using iterator_category = std::random_access_iterator_tag;

	zipped_iterator_impl(Iterators... args) : zipped_iterator_impl<std::bidirectional_iterator_tag, Iterators...>(args...) {
	}
	zipped_iterator_impl(const zipped_iterator_impl& other) : zipped_iterator_impl<std::bidirectional_iterator_tag, Iterators...>(other) {
	}
	zipped_iterator_impl() {
	}
	// methods copied over from base iterators
	auto& operator++() {
		this->_prefix_inc(std::index_sequence_for<Iterators...>());
		return *this;
	}
	auto operator++(int) {
		auto copy = *this;
		this->_prefix_inc(std::index_sequence_for<Iterators...>());
		return copy;
	}
	auto& operator--() {
		this->_prefix_dec(std::index_sequence_for<Iterators...>());
		return *this;
	}
	auto operator--(int) {
		auto copy = *this;
		_prefix_dec(std::index_sequence_for<Iterators...>());
		return copy;
	}
	// random access iterator operators
	auto& operator+=(difference_type n) {
		_inplace_add(n, std::index_sequence_for<Iterators...>());
		return *this;
	}
	auto operator+(difference_type n) {
		auto copy = *this;
		copy._inplace_add(n, std::index_sequence_for<Iterators...>());
		return copy;
	}
	auto& operator-=(difference_type n) {
		_inplace_sub(n, std::index_sequence_for<Iterators...>());
		return *this;
	}
	auto operator-(difference_type n) {
		auto copy = *this;
		copy._inplace_sub(n, std::index_sequence_for<Iterators...>());
		return copy;
	}
	bool operator<(const zipped_iterator_impl& other) const {
		return this->t < other.t;
	}
	bool operator<=(const zipped_iterator_impl& other) const {
		return this->t <= other.t;
	}
	bool operator>(const zipped_iterator_impl& other) const {
		return this->t > other.t;
	}
	bool operator>=(const zipped_iterator_impl& other) const {
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

} // namespace detail


template<typename ...Iterators>
using zipped_iterator = detail::zipped_iterator_impl<
	std::common_type_t<typename Iterators::iterator_category...>, // resolves to the worst iterator tag supported
	Iterators...
>;
 
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

