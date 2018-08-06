#ifndef _ITERUTILS_STRIDED_RANGE_H_
#define _ITERUTILS_STRIDED_RANGE_H_

#include <type_traits>

namespace iterutils {

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
auto strided_begin(Iterable& x, typename Iterable::size_type offset, typename Iterable::size_type stride) {
	return strided_iterator<typename Iterable::iterator>(x.begin() + offset, stride);
}
template<typename Iterable>
auto strided_cbegin(const Iterable& x, typename Iterable::size_type offset, typename Iterable::size_type stride) {
	return strided_iterator<typename Iterable::const_iterator>(x.cbegin() + offset, stride);
}
// strided_(c)end iterators need to calculate an offset from the true end of
// the iterable. This should be the first past-the-end position that the
// corresponding strided_(c)begin iterator will find through successive increments
template<typename Iterable>
auto strided_end(Iterable& x, typename Iterable::size_type offset, typename Iterable::size_type stride) {
	return strided_begin(x, offset, stride) += (x.size() - offset + stride - 1)/stride;
}
template<typename Iterable>
auto strided_cend(const Iterable& x, typename Iterable::size_type offset, typename Iterable::size_type stride) {
	return strided_cbegin(x, offset, stride) += (x.size() - offset + stride - 1)/stride;
}

template<typename Iterable>
class strided_range_impl {
public:
	using iterator = strided_iterator<typename std::remove_reference_t<Iterable>::iterator>;
	using value_type = typename iterator::value_type;
	using pointer = typename iterator::pointer;
	using reference = typename iterator::reference;
	using difference_type = typename iterator::difference_type;
	using size_type = typename std::remove_reference_t<Iterable>::size_type;

	strided_range_impl (
		detail::arg_from_uref_t<Iterable> iter,
		size_type offset, size_type stride
	) : _iter(iter), _offset(offset), _stride(stride) {
	}
	iterator begin() { return strided_begin(_iter, _offset, _stride); }
	iterator end() { return strided_end(_iter, _offset, _stride); }
	std::size_t size() const { return (_iter.size()-_offset) / _stride; }
private:
	Iterable _iter;
	size_type _offset;
	size_type _stride;
};
template<typename Iterable>
auto strided_range(Iterable&& i, typename std::remove_reference_t<Iterable>::size_type offset, typename std::remove_reference_t<Iterable>::size_type stride) {
	return strided_range_impl<Iterable>(std::forward<Iterable>(i), offset, stride);
}

} // namespace iterutils

#endif

