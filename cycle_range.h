#ifndef _ITERUTILS_CYCLE_RANGE_H_
#define _ITERUTILS_CYCLE_RANGE_H_

#include <iterator>
#include <limits>

namespace iterutils {

namespace detail {
	template<typename Tag, typename Iterator>
	class cycle_iterator_impl {};

	template<typename Tag, typename Iterator>
	cycle_iterator_impl<Tag, Iterator>&
	operator++(cycle_iterator_impl<Tag, Iterator>& it) {
		if (++it.it_ == it.end_)
			it.it_ = it.begin_;
		return it;
	}
	template<typename Tag, typename Iterator>
	cycle_iterator_impl<Tag, Iterator>
	operator++(cycle_iterator_impl<Tag, Iterator>& it, int) {
		auto copy = it;
		++it;
		return copy;
	}
	template<typename Tag, typename Iterator>
	cycle_iterator_impl<Tag, Iterator>&
	operator--(cycle_iterator_impl<Tag, Iterator>& it) {
		if (it.it_ == it.begin_)
			it.it_ = it.end_;
		--it.it_;
		return it;
	}
	template<typename Tag, typename Iterator>
	cycle_iterator_impl<Tag, Iterator>
	operator--(cycle_iterator_impl<Tag, Iterator>& it, int) {
		auto copy = it;
		--it;
		return copy;
	}
	template<typename Tag, typename Iterator>
	cycle_iterator_impl<Tag, Iterator>&
	operator+=(cycle_iterator_impl<Tag, Iterator>& it, typename cycle_iterator_impl<Tag, Iterator>::difference_type n) {
		it.it_ = it.begin_ + (n + (it.it_ - it.begin_)) % (it.end_ - it.begin_);
		return it;
	}
	template<typename Tag, typename Iterator>
	cycle_iterator_impl<Tag, Iterator>
	operator+(const cycle_iterator_impl<Tag, Iterator>& it, typename cycle_iterator_impl<Tag, Iterator>::difference_type n) {
		auto copy = it;
		return copy += n;
	}
	template<typename Tag, typename Iterator>
	cycle_iterator_impl<Tag, Iterator>&
	operator-=(cycle_iterator_impl<Tag, Iterator>& it, typename cycle_iterator_impl<Tag, Iterator>::difference_type n) {
		it.it_ = it.end_ - 1 -  ((it.end_ - it.it_) + n) % (it.end_ - it.begin_);
		return it;
	}
	template<typename Tag, typename Iterator>
	cycle_iterator_impl<Tag, Iterator>
	operator-(const cycle_iterator_impl<Tag, Iterator>& it, typename cycle_iterator_impl<Tag, Iterator>::difference_type n) {
		auto copy = it;
		return copy -= n;
	}


	// cannot have a cycle iterator for input iterators
	// we require multipass behaviour to cycle at least once!

	template<typename Iterator>
	class cycle_iterator_impl<std::forward_iterator_tag, Iterator> {
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = typename Iterator::value_type;
		using reference = typename Iterator::reference;
		using pointer = typename Iterator::pointer;
		using difference_type = typename Iterator::difference_type;

		cycle_iterator_impl(const cycle_iterator_impl& other) = default;
		cycle_iterator_impl() = default;
		cycle_iterator_impl(Iterator begin, Iterator end) : begin_(begin), end_(end), it_(begin) {}
		cycle_iterator_impl(Iterator begin, Iterator end, Iterator it) : begin_(begin), end_(end), it_(it) {}

		reference operator*() const { return *it_; }
		bool operator==(const cycle_iterator_impl& other) const { return it_ == other.it_; }
		bool operator!=(const cycle_iterator_impl& other) const { return it_ != other.it_; }

		friend cycle_iterator_impl& operator++<iterator_category, Iterator>(cycle_iterator_impl&);
		friend cycle_iterator_impl operator++<iterator_category, Iterator>(cycle_iterator_impl&, int);
	protected:
		Iterator begin_;
		Iterator end_;
		Iterator it_;
	};
	template<typename Iterator>
	class cycle_iterator_impl<std::bidirectional_iterator_tag, Iterator> : public cycle_iterator_impl<std::forward_iterator_tag, Iterator> {
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = typename Iterator::value_type;
		using reference = typename Iterator::reference;
		using pointer = typename Iterator::pointer;
		using difference_type = typename Iterator::difference_type;

		using cycle_iterator_impl<std::forward_iterator_tag, Iterator>::cycle_iterator_impl;

		friend cycle_iterator_impl& operator++<iterator_category, Iterator>(cycle_iterator_impl&);
		friend cycle_iterator_impl  operator++<iterator_category, Iterator>(cycle_iterator_impl&, int);
		friend cycle_iterator_impl& operator--<iterator_category, Iterator>(cycle_iterator_impl&);
		friend cycle_iterator_impl  operator--<iterator_category, Iterator>(cycle_iterator_impl&, int);
	};
	template<typename Iterator>
	class cycle_iterator_impl<std::random_access_iterator_tag, Iterator> : public cycle_iterator_impl<std::bidirectional_iterator_tag, Iterator> {
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = typename Iterator::value_type;
		using reference = typename Iterator::reference;
		using pointer = typename Iterator::pointer;
		using difference_type = typename Iterator::difference_type;

		using cycle_iterator_impl<std::bidirectional_iterator_tag, Iterator>::cycle_iterator_impl;

		friend cycle_iterator_impl& operator++<iterator_category, Iterator>(cycle_iterator_impl&);
		friend cycle_iterator_impl  operator++<iterator_category, Iterator>(cycle_iterator_impl&,       int);
		friend cycle_iterator_impl& operator--<iterator_category, Iterator>(cycle_iterator_impl&);
		friend cycle_iterator_impl  operator--<iterator_category, Iterator>(cycle_iterator_impl&,       int);
		friend cycle_iterator_impl& operator+=<iterator_category, Iterator>(cycle_iterator_impl&,       difference_type);
		friend cycle_iterator_impl  operator+ <iterator_category, Iterator>(const cycle_iterator_impl&, difference_type);
		friend cycle_iterator_impl& operator-=<iterator_category, Iterator>(cycle_iterator_impl&,       difference_type);
		friend cycle_iterator_impl  operator- <iterator_category, Iterator>(const cycle_iterator_impl&, difference_type);

		bool operator< (const cycle_iterator_impl& other) const { return this->it_ <  other.it_; }
		bool operator<=(const cycle_iterator_impl& other) const { return this->it_ <= other.it_; }
		bool operator> (const cycle_iterator_impl& other) const { return this->it_ >  other.it_; }
		bool operator>=(const cycle_iterator_impl& other) const { return this->it_ >= other.it_; }
	};
}

template<typename Iterator>
using cycle_iterator = detail::cycle_iterator_impl<typename Iterator::iterator_category, Iterator>;

template<typename Iterable>
cycle_iterator<typename Iterable::iterator> cycle_begin(Iterable& iterable) {
	return cycle_iterator<typename Iterable::iterator>(std::begin(iterable), std::end(iterable));
}
template<typename Iterable>
cycle_iterator<typename Iterable::iterator> cycle_end(Iterable& iterable) {
	return cycle_iterator<typename Iterable::iterator>(std::begin(iterable), std::end(iterable), std::end(iterable));
}

namespace detail {
	template<typename Iterable>
	class cycle_range_impl {
	public:
		using iterator = cycle_iterator<typename std::remove_reference_t<Iterable>::iterator>;
		using value_type = typename iterator::value_type;
		using pointer = typename iterator::pointer;
		using reference = typename iterator::reference;

		cycle_range_impl(std::add_rvalue_reference_t<Iterable> iterable) : iterable_(iterable) {
		}
		iterator begin() { return cycle_begin(iterable_); }
		iterator end() { return cycle_end(iterable_); }
		std::size_t size() const { return std::numeric_limits<std::size_t>::max(); }
	private:
		Iterable iterable_;
	};
}

template<typename Iterable>
auto cycle_range(Iterable&& iterable) {
	return detail::cycle_range_impl<Iterable>(std::forward<Iterable>(iterable));
}

} // namespace iterutils

#endif
