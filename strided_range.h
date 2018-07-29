#ifndef _STRIDED_RANGE_H_
#define _STRIDED_RANGE_H_

template<typename Iterator>
class strided_iterator : public Iterator {
public:
	explicit strided_iterator(Iterator _it, typename Iterator::difference_type _stride) : Iterator(_it), stride(_stride) {
	}
	strided_iterator& operator++() {
		Iterator::operator+=(stride);
		return *this;
	}
	strided_iterator operator++(int) {
		strided_iterator retval = *this;
		Iterator::operator+=(stride);
		return retval;
	}
	strided_iterator& operator--() {
		Iterator::operator-=(stride);
		return *this;
	}
	strided_iterator operator--(int) {
		strided_iterator retval = *this;
		Iterator::operator-=(stride);
		return retval;
	}
	strided_iterator& operator+=(typename Iterator::difference_type n) {
		Iterator::operator+=(stride*n);
		return *this;
	}
	strided_iterator operator+(typename Iterator::difference_type n) const {
		strided_iterator retval = *this;
		return retval += n;
	}
	strided_iterator& operator-=(typename Iterator::difference_type n) {
		Iterator::operator-=(stride*n);
		return *this;
	}
	strided_iterator operator-(typename Iterator::difference_type n) const {
		strided_iterator retval = *this;
		return retval -= n;
	}
	typename Iterator::difference_type operator-(const strided_iterator& other) const {
		return (static_cast<const Iterator&>(*this)-static_cast<const Iterator&>(other))/stride;
	}
	typename Iterator::reference operator[](typename Iterator::difference_type n) const {
		return Iterator::operator[](n*stride);
	}
private:
	typename Iterator::difference_type stride;
};

template<typename Iterable>
strided_iterator<typename Iterable::iterator> strided_begin(Iterable& x, typename Iterable::size_type offset, typename Iterable::size_type stride) {
	return strided_iterator<typename Iterable::iterator>(x.begin() + offset, stride);
}
template<typename Iterable>
strided_iterator<typename Iterable::const_iterator> strided_cbegin(const Iterable& x, typename Iterable::size_type offset, typename Iterable::size_type stride) {
	return strided_iterator<typename Iterable::const_iterator>(x.cbegin() + offset, stride);
}
// strided_(c)end iterators need to calculate an offset from the true end of
// the iterable. This should be the first past-the-end position that the
// corresponding strided_(c)begin iterator will find through successive increments
template<typename Iterable>
strided_iterator<typename Iterable::iterator> strided_end(Iterable& x, typename Iterable::size_type offset, typename Iterable::size_type stride) {
	return strided_begin(x, offset, stride) += (x.size() - offset + stride - 1)/stride;
}
template<typename Iterable>
strided_iterator<typename Iterable::const_iterator> strided_cend(const Iterable& x, typename Iterable::size_type offset, typename Iterable::size_type stride) {
	return strided_cbegin(x, offset, stride) += (x.size() - offset + stride - 1)/stride;
}

template<typename Iterable>
class strided_range {
public:
	using value_type = typename Iterable::value_type;
	using pointer = typename Iterable::pointer;
	using reference = typename Iterable::reference;
	using difference_type = typename Iterable::difference_type;
	using iterator = strided_iterator<typename Iterable::iterator>;
	using size_type = typename Iterable::size_type;

	strided_range(Iterable& i, size_type offset, size_type stride)
		: _begin(strided_begin(i, offset, stride)), _end(strided_end(i, offset, stride)) {
	}
	iterator begin() { return _begin; }
	iterator end() { return _end; }
private:
	iterator _begin;
	iterator _end;

};

#endif

