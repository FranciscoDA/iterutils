#ifndef _ITERUTILS_REPEAT_RANGE_H_
#define _ITERUTILS_REPEAT_RANGE_H_

#include <type_traits>

namespace iterutils {

template<typename T>
class repeat_iterator {
public:
	using value_type = std::remove_reference_t<T>;
	using pointer = std::add_pointer_t<std::remove_reference_t<T>>;
	using reference = std::add_lvalue_reference_t<T>;
	using difference_type = int;
	using iterator_category = std::random_access_iterator_tag;

	repeat_iterator(T&& e) : element(std::forward<T>(e)) {
	}
	reference operator*() {
		return element;
	}
	reference operator[](int) {
		return element;
	}
	repeat_iterator& operator++()    { return *this; }
	repeat_iterator operator++(int)  { return *this; }
	repeat_iterator& operator--()    { return *this; }
	repeat_iterator operator--(int)  { return *this; }
	repeat_iterator& operator+=(int) { return *this; }
	repeat_iterator operator+(int)   { return *this; }
	repeat_iterator& operator-=(int) { return *this; }
	repeat_iterator operator-(int)   { return *this; }

	bool operator!=(const repeat_iterator& other) const { return true;  }
	bool operator==(const repeat_iterator& other) const { return false; }
	bool operator< (const repeat_iterator& other) const { return false; }
	bool operator<=(const repeat_iterator& other) const { return false; }
	bool operator> (const repeat_iterator& other) const { return false; }
	bool operator>=(const repeat_iterator& other) const { return false; }

protected:
	T element;
};

template<typename T>
class repeat_range {
public:
	using iterator   = repeat_iterator<std::remove_reference_t<T>>;
	using value_type = std::remove_reference_t<T>;
	using reference  = std::add_lvalue_reference_t<value_type>;
	using pointer    = std::add_pointer_t<value_type>;

	repeat_range(T&& e) : element(std::forward<T>(e)) {
	}
	iterator begin() { return {std::forward<T>(element)}; }
	iterator end()   { return {std::forward<T>(element)}; }
protected:
	T element;
};

template<typename T>
struct is_infinite<repeat_range<T>> : public std::true_type {};

} // namespace iterutils

#endif
