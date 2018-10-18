#ifndef _ITERUTILS_CHAINED_RANGE_H_
#define _ITERUTILS_CHAINED_RANGE_H_

#include <array>
#include <variant>
#include <tuple>
#include "util.h"

namespace iterutils {

template<typename Tag, typename ...Iterators>
class chained_iterator {};

template<typename Tag, typename ...Iterators>
chained_iterator<Tag, Iterators...>& operator++(chained_iterator<Tag, Iterators...>& it) {
	std::visit([](auto& it) { ++it; }, it.pos_[it.index_]);
	if (it.index_ < sizeof...(Iterators) and it.pos_[it.index_] == it.ends_[it.index_])
		++it.index_;
	return it;
}
template<typename Tag, typename ...Iterators>
chained_iterator<Tag, Iterators...> operator++(chained_iterator<Tag, Iterators...>& it, int) {
	chained_iterator<Tag, Iterators...> copy = it;
	++it;
	return copy;
}
template<typename Tag, typename ...Iterators>
chained_iterator<Tag, Iterators...>& operator--(chained_iterator<Tag, Iterators...>& it) {
	if (it.pos_[it.index_] == it.begins_[it.index_])
		--it.index_;
	std::visit([](auto& it) { --it; }, it.pos_[it.index_]);
	return it;
}
template<typename Tag, typename ...Iterators>
chained_iterator<Tag, Iterators...> operator--(chained_iterator<Tag, Iterators...>& it, int) {
	chained_iterator<Tag, Iterators...> copy = it;
	--it;
	return copy;
}
template<typename Tag, typename ...Iterators>
chained_iterator<Tag, Iterators...>&
operator+=(chained_iterator<Tag, Iterators...>& it, typename chained_iterator<Tag, Iterators...>::difference_type n) {
	while (n > it.ends_[it.index_] - it.pos_[it.index_]) {
		n -= it.ends_[it.index_] - it.pos_[it.index_];
		it.pos_[it.index_] = it.ends_[it.index_];
		++it.index_;
	}
	it.pos_[it.index_] += n;
}
template<typename Tag, typename ...Iterators>
chained_iterator<Tag, Iterators...>
operator+(const chained_iterator<Tag, Iterators...>& it, typename chained_iterator<Tag, Iterators...>::difference_type n) {
	chained_iterator<Tag, Iterators...> copy = it;
	return (copy += n);
}
template<typename Tag, typename ...Iterators>
chained_iterator<Tag, Iterators...>&
operator-=(chained_iterator<Tag, Iterators...>& it, typename chained_iterator<Tag, Iterators...>::difference_type n) {
	while (n > it.pos_[it.index_] - it.begins_[it.index_]) {
		n -= it.pos_[it.index_] - it.begins_[it.index_];
		it.pos_[it.index_] = it.begins_[it.index_];
		--it.index_;
	}
	it.pos_[it.index_] -= n;
}
template<typename Tag, typename ...Iterators>
chained_iterator<Tag, Iterators...>
operator-(const chained_iterator<Tag, Iterators...>& it, typename chained_iterator<Tag, Iterators...>::difference_type n) {
	chained_iterator<Tag, Iterators...> copy = it;
	return (copy -= n);
}

template<typename ...Iterators>
class chained_iterator<std::input_iterator_tag, Iterators...> {
public:
	using value_type        = std::common_type_t<typename std::iterator_traits<Iterators>::value_type...>;
	using reference         = std::add_lvalue_reference_t<value_type>;
	using pointer           = std::add_pointer_t<value_type>;
	using difference_type   = std::common_type_t<typename std::iterator_traits<Iterators>::difference_type...>;
	using iterator_category = std::input_iterator_tag;

	template<std::size_t ...I>
	chained_iterator(std::index_sequence<I...>, Iterators... pos, Iterators... ends, std::size_t index)
		: pos_{element_type(std::in_place_index_t<I>(), pos)...},
		ends_{element_type(std::in_place_index_t<I>(), ends)...},
		index_(index) {
	}
	chained_iterator(Iterators... pos, Iterators... ends, std::size_t index=0)
		: chained_iterator(std::index_sequence_for<Iterators...>(), pos..., ends..., index) {
	}
	chained_iterator(const chained_iterator& other) = delete;
	chained_iterator() = delete;

	friend chained_iterator& operator++<iterator_category, Iterators...>(chained_iterator&);

	reference operator*() const {
		return *std::visit([](auto&& it) { return it; }, pos_[index_]);
	}
	bool operator!=(const chained_iterator& other) const {
		return pos_[index_] != other.pos_[other.index_];
	}
	bool operator==(const chained_iterator& other) const {
		return pos_[index_] == other.pos_[other.index_];
	}

protected:
	using element_type = std::variant<Iterators...>;
	std::array<element_type, sizeof...(Iterators)> pos_;
	std::array<element_type, sizeof...(Iterators)> ends_;
	std::size_t index_;
};

template<typename ...Iterators>
class chained_iterator<std::forward_iterator_tag, Iterators...>
: public chained_iterator<std::input_iterator_tag, Iterators...> {
public:
	using iterator_category = std::forward_iterator_tag;

	chained_iterator(Iterators... pos, Iterators... ends, std::size_t index=0)
		: chained_iterator<std::input_iterator_tag, Iterators...>(std::index_sequence_for<Iterators...>(), pos..., ends..., index) {
	}
	chained_iterator(const chained_iterator& other) = default;
	chained_iterator() = default;

	// inherited operators
	friend chained_iterator& operator++<iterator_category, Iterators...>(chained_iterator&);
	// forward iterator operators
	friend chained_iterator  operator++<iterator_category, Iterators...>(chained_iterator&, int);
};

template<typename ...Iterators>
class chained_iterator<std::bidirectional_iterator_tag, Iterators...>
: public chained_iterator<std::forward_iterator_tag, Iterators...> {
public:
	using iterator_category = std::bidirectional_iterator_tag;

	template<std::size_t ...I>
	chained_iterator(std::index_sequence<I...>, Iterators... pos, Iterators... ends, std::size_t index)
		: chained_iterator<std::forward_iterator_tag, Iterators...>(pos..., ends..., index),
		begins_{typename chained_iterator<std::forward_iterator_tag, Iterators...>::element_type(std::in_place_index_t<I>(), pos)...} {
	}
	chained_iterator(Iterators... pos, Iterators... ends, std::size_t index=0)
		: chained_iterator(std::index_sequence_for<Iterators...>(), pos..., ends..., index) {
	}
	chained_iterator(const chained_iterator& other) = default;
	chained_iterator() = default;

	// inherited operators
	friend chained_iterator& operator++<iterator_category, Iterators...>(chained_iterator&);
	friend chained_iterator  operator++<iterator_category, Iterators...>(chained_iterator&, int);
	// bidirectional iterator operators
	friend chained_iterator& operator--<iterator_category, Iterators...>(chained_iterator&);
	friend chained_iterator  operator--<iterator_category, Iterators...>(chained_iterator&, int);
protected:
	std::array<
		typename chained_iterator<std::forward_iterator_tag, Iterators...>::element_type,
		sizeof...(Iterators)
	> begins_;
};

template<typename ...Iterators>
class chained_iterator<std::random_access_iterator_tag, Iterators...>
: public chained_iterator<std::bidirectional_iterator_tag, Iterators...> {
public:
	using difference_type = typename chained_iterator<std::bidirectional_iterator_tag, Iterators...>::difference_type;
	using iterator_category = std::random_access_iterator_tag;

	using chained_iterator<std::bidirectional_iterator_tag, Iterators...>::chained_iterator;

	// inherited operators
	friend chained_iterator& operator++<iterator_category, Iterators...>(chained_iterator&);
	friend chained_iterator  operator++<iterator_category, Iterators...>(chained_iterator&,       int);
	friend chained_iterator& operator--<iterator_category, Iterators...>(chained_iterator&);
	friend chained_iterator  operator--<iterator_category, Iterators...>(chained_iterator&,       int);
	// random access iterator operators
	friend chained_iterator& operator+=<iterator_category, Iterators...>(chained_iterator&,       difference_type);
	friend chained_iterator  operator+ <iterator_category, Iterators...>(const chained_iterator&, difference_type);
	friend chained_iterator& operator-=<iterator_category, Iterators...>(chained_iterator&,       difference_type);
	friend chained_iterator  operator- <iterator_category, Iterators...>(const chained_iterator&, difference_type);

	bool operator<(const chained_iterator& other) const {
		return (this->index_ < other.index_) or (this->index_ == other.index_ and this->pos_[this->index_] < other.pos_[other.index_]);
	}
	bool operator<=(const chained_iterator& other) const {
		return (this->index_ < other.index_) or (this->index_ == other.index_ and this->pos_[this->index_] <= other.pos_[other.index_]);
	}
	bool operator>(const chained_iterator& other) const {
		return (this->index_ > other.index_) or (this->index_ == other.index_ and this->pos_[this->index_] > other.pos_[other.index_]);
	}
	bool operator>=(const chained_iterator& other) const {
		return (this->index_ > other.index_) or (this->index_ == other.index_ and this->pos_[this->index_] >= other.pos_[other.index_]);
	}
};
template<typename ...Iterators>
chained_iterator(Iterators..., Iterators...) -> chained_iterator<
	std::common_type_t<typename Iterators::iterator_category...>,
	Iterators...
>;

template<typename ...Iterables>
detail::specialize_iterator_from_iterables<chained_iterator, Iterables...> chained_begin(Iterables&... iterables) {
	return {std::begin(iterables)..., std::end(iterables)...};
}
template<typename ...Iterables>
detail::specialize_iterator_from_iterables<chained_iterator, Iterables...> chained_end(Iterables&... iterables) {
	return {std::end(iterables)..., std::end(iterables)..., sizeof...(Iterables)};
}

template<typename ...Iterables>
class chained_range {
public:
	using iterator = detail::specialize_iterator_from_iterables<chained_iterator, Iterables...>;
	using value_type = typename iterator::value_type;
	using reference = typename iterator::reference;
	using pointer = typename iterator::pointer;
	chained_range (Iterables&&... iterables) : t(std::forward<Iterables>(iterables)...) {
	}
	iterator begin() { return std::apply(chained_begin<std::remove_reference_t<Iterables>...>, t); }
	iterator end() { return std::apply(chained_end<std::remove_reference_t<Iterables>...>, t); }

	std::enable_if<!is_infinite<chained_range>::value, size_t>
	size() const { return _size(std::index_sequence_for<Iterables...>()); }
private:
	template<std::size_t ...I>
	std::size_t _size(std::index_sequence<I...>) const { return (std::size(std::get<I>(t)) + ...); }
	std::tuple<Iterables...> t;
};
template<typename ...Iterables>
chained_range(Iterables&&...) -> chained_range<Iterables...>;

template<typename ...T>
struct is_infinite<chained_range<T...>> : public std::disjunction<is_infinite<T>...> {};

} // namespace iterutils

#endif
