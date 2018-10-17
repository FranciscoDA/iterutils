#ifndef _ITERUTILS_SERIES_RANGE_H_
#define _ITERUTILS_SERIES_RANGE_H_

#include <type_traits>

namespace iterutils {

template<typename T>
class series_iterator {
public:
	using value_type = T;
	using pointer = std::add_pointer_t<T>;
	using reference = std::add_lvalue_reference_t<T>;
	using difference_type = T;
	using iterator_category = std::random_access_iterator_tag;
	series_iterator(T i, T b) : i_(i), b_(b) {}
	series_iterator& operator++() {
		i_ += b_;
		return *this;
	}
	series_iterator operator++(int) {
		series_iterator copy = *this;
		++(*this);
		return copy;
	}
	series_iterator& operator--() {
		i_ -= b_;
		return *this;
	}
	series_iterator operator--(int) {
		series_iterator copy = *this;
		--(*this);
		return copy;
	}
	series_iterator& operator+=(difference_type n) {
		i_ += b_ * n;
		return *this;
	}
	series_iterator& operator-=(difference_type n) {
		i_ -= b_ * n;
		return *this;
	}
	series_iterator operator+(difference_type n) {
		series_iterator copy = *this;
		return copy += n;
	}
	series_iterator operator-(difference_type n) {
		series_iterator copy = *this;
		return copy -= n;
	}
	difference_type operator-(const series_iterator& other) const {
		return (i_ - other.i_) / b_;
	}
	T& operator*() const {
		return i_;
	}
	T operator[](int x) const {
		return i_ + b_ * x;
	}
	bool operator!=(const series_iterator& other) const { return i_ != other._i; }
	bool operator==(const series_iterator& other) const { return i_ == other._i; }
	bool operator< (const series_iterator& other) const { return i_ <  other._i; }
	bool operator<=(const series_iterator& other) const { return i_ <= other._i; }
	bool operator> (const series_iterator& other) const { return i_ >  other._i; }
	bool operator>=(const series_iterator& other) const { return i_ >= other._i; }
private:
	T i_;
	const T b_;
};

template<typename T>
class series_range {
public:
	series_range(T start, T stop, T step) {
	}
	series_iterator<T> begin() const { return {start, step}; }
	series_iterator<T> end()   const { return begin() + (stop-start+1) / step; }
private:
	T start, stop, step;
};

} // namespace iterutils

#endif
