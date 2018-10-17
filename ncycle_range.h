#ifndef _ITERUTILS_NCYCLE_RANGE_H_
#define _ITERUTILS_NCYCLE_RANGE_H_

#include <iterator>
#include <limits>
#include "util.h"

namespace iterutils {

template<typename Tag, typename Iterator>
class ncycle_iterator {};

template<typename Tag, typename Iterator>
ncycle_iterator<Tag, Iterator>&
operator++(ncycle_iterator<Tag, Iterator>& it) {
	if (++it.it_ == it.end_ and it.n_ > 0) {
		it.it_ = it.begin_;
		--it.n_;
	}
	return it;
}
template<typename Tag, typename Iterator>
ncycle_iterator<Tag, Iterator>
operator++(ncycle_iterator<Tag, Iterator>& it, int) {
	auto copy = it;
	++it;
	return copy;
}
template<typename Tag, typename Iterator>
ncycle_iterator<Tag, Iterator>&
operator--(ncycle_iterator<Tag, Iterator>& it) {
	if (it.it_ == it.begin_) {
		it.it_ = it.end_;
		++it.n_;
	}
	--it.it_;
	return it;
}
template<typename Tag, typename Iterator>
ncycle_iterator<Tag, Iterator>
operator--(ncycle_iterator<Tag, Iterator>& it, int) {
	auto copy = it;
	--it;
	return copy;
}
template<typename Tag, typename Iterator>
ncycle_iterator<Tag, Iterator>&
operator+=(ncycle_iterator<Tag, Iterator>& it, typename ncycle_iterator<Tag, Iterator>::difference_type n) {
	it.n_ -= (n + (it.it_ - it.begin_)) / (it.end_ - it.begin_);
	it.it_ = it.begin_ + (n + (it.it_ - it.begin_)) % (it.end_ - it.begin_);
	return it;
}
template<typename Tag, typename Iterator>
ncycle_iterator<Tag, Iterator>
operator+(const ncycle_iterator<Tag, Iterator>& it, typename ncycle_iterator<Tag, Iterator>::difference_type n) {
	auto copy = it;
	return copy += n;
}
template<typename Tag, typename Iterator>
ncycle_iterator<Tag, Iterator>&
operator-=(ncycle_iterator<Tag, Iterator>& it, typename ncycle_iterator<Tag, Iterator>::difference_type n) {
	it.n_ += ((it.end_ - it.it_) + n) / (it.end_ - it.begin_);
	it.it_ = it.end_ - 1 -  ((it.end_ - it.it_) + n) % (it.end_ - it.begin_);
	return it;
}
template<typename Tag, typename Iterator>
ncycle_iterator<Tag, Iterator>
operator-(const ncycle_iterator<Tag, Iterator>& it, typename ncycle_iterator<Tag, Iterator>::difference_type n) {
	auto copy = it;
	return copy -= n;
}

// cannot have a cycle iterator for input iterators
// we require multipass behaviour to cycle at least once!

template<typename Iterator>
class ncycle_iterator<std::forward_iterator_tag, Iterator> {
public:
	using iterator_category = std::forward_iterator_tag;
	using value_type = typename Iterator::value_type;
	using reference = typename Iterator::reference;
	using pointer = typename Iterator::pointer;
	using difference_type = typename Iterator::difference_type;

	ncycle_iterator(const ncycle_iterator& other) = default;
	ncycle_iterator() = default;
	ncycle_iterator(Iterator begin, Iterator end, Iterator it, std::size_t n) : begin_(begin), end_(end), it_(it), n_(n) {}

	reference operator*() const { return *it_; }
	bool operator==(const ncycle_iterator& other) const { return n_ == other.n_ and it_ == other.it_; }
	bool operator!=(const ncycle_iterator& other) const { return n_ != other.n_ or it_ != other.it_; }

	friend ncycle_iterator& operator++<std::forward_iterator_tag, Iterator>(ncycle_iterator&);
	friend ncycle_iterator operator++<std::forward_iterator_tag, Iterator>(ncycle_iterator&, int);
protected:
	Iterator begin_;
	Iterator end_;
	Iterator it_;
	std::size_t n_;
};
template<typename Iterator>
class ncycle_iterator<std::bidirectional_iterator_tag, Iterator> : public ncycle_iterator<std::forward_iterator_tag, Iterator> {
public:
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = typename Iterator::value_type;
	using reference = typename Iterator::reference;
	using pointer = typename Iterator::pointer;
	using difference_type = typename Iterator::difference_type;

	using ncycle_iterator<std::forward_iterator_tag, Iterator>::ncycle_iterator;

	friend ncycle_iterator& operator++<iterator_category, Iterator>(ncycle_iterator&);
	friend ncycle_iterator  operator++<iterator_category, Iterator>(ncycle_iterator&, int);
	friend ncycle_iterator& operator--<iterator_category, Iterator>(ncycle_iterator&);
	friend ncycle_iterator  operator--<iterator_category, Iterator>(ncycle_iterator&, int);
};
template<typename Iterator>
class ncycle_iterator<std::random_access_iterator_tag, Iterator> : public ncycle_iterator<std::bidirectional_iterator_tag, Iterator> {
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = typename Iterator::value_type;
	using reference = typename Iterator::reference;
	using pointer = typename Iterator::pointer;
	using difference_type = typename Iterator::difference_type;

	using ncycle_iterator<std::bidirectional_iterator_tag, Iterator>::ncycle_iterator;

	friend ncycle_iterator& operator++<iterator_category, Iterator>(ncycle_iterator&);
	friend ncycle_iterator  operator++<iterator_category, Iterator>(ncycle_iterator&, int);
	friend ncycle_iterator& operator--<iterator_category, Iterator>(ncycle_iterator&);
	friend ncycle_iterator  operator--<iterator_category, Iterator>(ncycle_iterator&, int);
	friend ncycle_iterator& operator+=<iterator_category, Iterator>(ncycle_iterator&,       difference_type);
	friend ncycle_iterator  operator+ <iterator_category, Iterator>(const ncycle_iterator&, difference_type);
	friend ncycle_iterator& operator-=<iterator_category, Iterator>(ncycle_iterator&,       difference_type);
	friend ncycle_iterator  operator- <iterator_category, Iterator>(const ncycle_iterator&, difference_type);

	bool operator< (const ncycle_iterator& other) const {
		return (this->n_ > other.n_) or (this->n_ == other.n_ and this->it_ < other.it_);
	}
	bool operator<=(const ncycle_iterator& other) const {
		return (this->n_ > other.n_) or (this->n_ == other.n_ and this->it_ <= other.it_);
	}
	bool operator>(const ncycle_iterator& other) const {
		return (this->n_ < other.n_) or (this->n_ == other.n_ and this->it_ > other.it_);
	}
	bool operator>=(const ncycle_iterator& other) const {
		return (this->n_ < other.n_) or (this->n_ == other.n_ and this->it_ >= other.it_);
	}
};
template<typename Iterator>
ncycle_iterator(Iterator, Iterator, Iterator) -> ncycle_iterator<typename Iterator::iterator_category, Iterator>;

template<typename Iterable>
detail::specialize_iterator_from_iterables<ncycle_iterator, Iterable> ncycle_begin(Iterable& iterable, std::size_t n) {
	return {std::begin(iterable), std::end(iterable), std::begin(iterable), n};
}
template<typename Iterable>
detail::specialize_iterator_from_iterables<ncycle_iterator, Iterable> ncycle_end(Iterable& iterable) {
	return {std::begin(iterable), std::end(iterable), std::end(iterable), 0};
}

template<typename Iterable>
class ncycle_range {
public:
	using iterator = detail::specialize_iterator_from_iterables<ncycle_iterator, Iterable>;
	using value_type = typename iterator::value_type;
	using pointer = typename iterator::pointer;
	using reference = typename iterator::reference;

	ncycle_range(Iterable&& iterable, std::size_t n) : iterable_(std::forward<Iterable>(iterable)), n_(n) {
	}
	iterator begin() { return ncycle_begin(iterable_, n_); }
	iterator end() { return ncycle_end(iterable_); }
	std::size_t size() const { return iterable_.size()*n_; }
private:
	Iterable iterable_;
	std::size_t n_;
};
template<typename Iterable> ncycle_range(Iterable&& iterable, std::size_t) -> ncycle_range<Iterable>;

} // namespace iterutils

#endif
