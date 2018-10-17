#ifndef _ITERUTILS_CYCLE_RANGE_H_
#define _ITERUTILS_CYCLE_RANGE_H_

#include <iterator>
#include "util.h"

namespace iterutils {

template<typename Tag, typename Iterator>
class cycle_iterator {};

template<typename Tag, typename Iterator>
cycle_iterator<Tag, Iterator>&
operator++(cycle_iterator<Tag, Iterator>& it) {
	if (++it.it_ == it.end_)
		it.it_ = it.begin_;
	return it;
}
template<typename Tag, typename Iterator>
cycle_iterator<Tag, Iterator>
operator++(cycle_iterator<Tag, Iterator>& it, int) {
	auto copy = it;
	++it;
	return copy;
}
template<typename Tag, typename Iterator>
cycle_iterator<Tag, Iterator>&
operator--(cycle_iterator<Tag, Iterator>& it) {
	if (it.it_ == it.begin_)
		it.it_ = it.end_;
	--it.it_;
	return it;
}
template<typename Tag, typename Iterator>
cycle_iterator<Tag, Iterator>
operator--(cycle_iterator<Tag, Iterator>& it, int) {
	auto copy = it;
	--it;
	return copy;
}
template<typename Tag, typename Iterator>
cycle_iterator<Tag, Iterator>&
operator+=(cycle_iterator<Tag, Iterator>& it, typename cycle_iterator<Tag, Iterator>::difference_type n) {
	it.it_ = it.begin_ + (n + (it.it_ - it.begin_)) % (it.end_ - it.begin_);
	return it;
}
template<typename Tag, typename Iterator>
cycle_iterator<Tag, Iterator>
operator+(const cycle_iterator<Tag, Iterator>& it, typename cycle_iterator<Tag, Iterator>::difference_type n) {
	auto copy = it;
	return copy += n;
}
template<typename Tag, typename Iterator>
cycle_iterator<Tag, Iterator>&
operator-=(cycle_iterator<Tag, Iterator>& it, typename cycle_iterator<Tag, Iterator>::difference_type n) {
	it.it_ = it.end_ - 1 -  ((it.end_ - it.it_) + n) % (it.end_ - it.begin_);
	return it;
}
template<typename Tag, typename Iterator>
cycle_iterator<Tag, Iterator>
operator-(const cycle_iterator<Tag, Iterator>& it, typename cycle_iterator<Tag, Iterator>::difference_type n) {
	auto copy = it;
	return copy -= n;
}

// cannot have a cycle iterator for input iterators
// we require multipass behaviour to cycle at least once!

template<typename Iterator>
class cycle_iterator<std::forward_iterator_tag, Iterator> {
public:
	using iterator_category = std::forward_iterator_tag;
	using value_type = typename Iterator::value_type;
	using reference = typename Iterator::reference;
	using pointer = typename Iterator::pointer;
	using difference_type = typename Iterator::difference_type;

	cycle_iterator(const cycle_iterator& other) = default;
	cycle_iterator() = default;
	cycle_iterator(Iterator begin, Iterator end) : begin_(begin), end_(end), it_(begin) {}
	cycle_iterator(Iterator begin, Iterator end, Iterator it) : begin_(begin), end_(end), it_(it) {}

	reference operator*() const { return *it_; }
	bool operator==(const cycle_iterator& other) const { return it_ == other.it_; }
	bool operator!=(const cycle_iterator& other) const { return it_ != other.it_; }

	friend cycle_iterator& operator++<iterator_category, Iterator>(cycle_iterator&);
	friend cycle_iterator operator++<iterator_category, Iterator>(cycle_iterator&, int);
protected:
	Iterator begin_;
	Iterator end_;
	Iterator it_;
};
template<typename Iterator>
class cycle_iterator<std::bidirectional_iterator_tag, Iterator> : public cycle_iterator<std::forward_iterator_tag, Iterator> {
public:
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = typename Iterator::value_type;
	using reference = typename Iterator::reference;
	using pointer = typename Iterator::pointer;
	using difference_type = typename Iterator::difference_type;

	using cycle_iterator<std::forward_iterator_tag, Iterator>::cycle_iterator;

	friend cycle_iterator& operator++<iterator_category, Iterator>(cycle_iterator&);
	friend cycle_iterator  operator++<iterator_category, Iterator>(cycle_iterator&, int);
	friend cycle_iterator& operator--<iterator_category, Iterator>(cycle_iterator&);
	friend cycle_iterator  operator--<iterator_category, Iterator>(cycle_iterator&, int);
};
template<typename Iterator>
class cycle_iterator<std::random_access_iterator_tag, Iterator> : public cycle_iterator<std::bidirectional_iterator_tag, Iterator> {
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = typename Iterator::value_type;
	using reference = typename Iterator::reference;
	using pointer = typename Iterator::pointer;
	using difference_type = typename Iterator::difference_type;

	using cycle_iterator<std::bidirectional_iterator_tag, Iterator>::cycle_iterator;

	friend cycle_iterator& operator++<iterator_category, Iterator>(cycle_iterator&);
	friend cycle_iterator  operator++<iterator_category, Iterator>(cycle_iterator&,       int);
	friend cycle_iterator& operator--<iterator_category, Iterator>(cycle_iterator&);
	friend cycle_iterator  operator--<iterator_category, Iterator>(cycle_iterator&,       int);
	friend cycle_iterator& operator+=<iterator_category, Iterator>(cycle_iterator&,       difference_type);
	friend cycle_iterator  operator+ <iterator_category, Iterator>(const cycle_iterator&, difference_type);
	friend cycle_iterator& operator-=<iterator_category, Iterator>(cycle_iterator&,       difference_type);
	friend cycle_iterator  operator- <iterator_category, Iterator>(const cycle_iterator&, difference_type);

	bool operator< (const cycle_iterator& other) const { return this->it_ <  other.it_; }
	bool operator<=(const cycle_iterator& other) const { return this->it_ <= other.it_; }
	bool operator> (const cycle_iterator& other) const { return this->it_ >  other.it_; }
	bool operator>=(const cycle_iterator& other) const { return this->it_ >= other.it_; }
};

template<typename Iterator>
cycle_iterator(Iterator, Iterator, Iterator) -> cycle_iterator<typename Iterator::iterator_category, Iterator>;

template<typename Iterable>
detail::specialize_iterator_from_iterables<cycle_iterator, Iterable> cycle_begin(Iterable& iterable) {
	return {std::begin(iterable), std::end(iterable)};
}
template<typename Iterable>
detail::specialize_iterator_from_iterables<cycle_iterator, Iterable> cycle_end(Iterable& iterable) {
	return {std::begin(iterable), std::end(iterable), std::end(iterable)};
}

template<typename Iterable>
class cycle_range {
public:
	using iterator = detail::specialize_iterator_from_iterables<cycle_iterator, Iterable>;
	using value_type = typename iterator::value_type;
	using pointer = typename iterator::pointer;
	using reference = typename iterator::reference;

	cycle_range(Iterable&& iterable) : iterable_(std::forward<Iterable>(iterable)) {
	}
	iterator begin() { return cycle_begin(iterable_); }
	iterator end() { return cycle_end(iterable_); }
private:
	Iterable iterable_;
};
template<typename Iterable> cycle_range(Iterable&&) -> cycle_range<Iterable>;

template<typename ...T>
struct is_infinite<cycle_range<T...>> : public std::true_type {};

} // namespace iterutils

#endif
