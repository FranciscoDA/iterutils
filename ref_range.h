#ifndef _ITERUTILS_REF_RANGE_H_
#define _ITERUTILS_REF_RANGE_H_

template<typename T, typename... Args>
class ref_range {
private:
	std::array<T*, sizeof...(Args)+1> refs;
public:
	ref_range(T& arg1, Args&... args) : refs{&arg1, &args...} {
	}
	class iterator {
	public:
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using reference = T&;
		using pointer = T*;
		using iterator_category = std::random_access_iterator_tag;

		iterator(T** _base) : base(_base) {}
		T& operator*() { return *(*base); }
		iterator& operator++() { ++base; return *this; }
		bool operator!=(const iterator& other) const { return base != other.base; }
	private:
		T** base;
	};
	using value_type = typename iterator::value_type;
	using difference_type = typename iterator::difference_type;
	using reference = typename iterator::reference;
	using pointer = typename iterator::pointer;
	iterator begin() { return iterator(refs.begin()); }
	iterator end() { return iterator(refs.end()); }
	std::size_t size() const { return sizeof...(Args)+1; }
};

#endif

