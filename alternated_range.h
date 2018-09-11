#ifndef _ITERUTILS_ALTERNATED_RANGE_H_
#define _ITERUTILS_ALTERNATED_RANGE_H_

#include <variant>
#include <array>

namespace iterutils {

template<typename ...Iterators>
class alternated_iterator {
public:
	using value_type = std::common_type_t<typename Iterators::value_type...>;
	using reference = std::add_lvalue_reference_t<value_type>;
	using pointer = std::add_pointer_t<value_type>;

	alternated_iterator(Iterators... iterators, std::size_t index=0) : _its(iterators...), _index(index) {
	}
	alternated_iterator& operator++() {
		++_index;
		return *this;
	}
	alternated_iterator operator++(int) {
		alternated_iterator copy = *this;
		++(*this);
		return copy;
	}
	reference operator*() {
		return std::visit([](auto&& it){ return *it; }, _its[_index % sizeof...(Iterators)]);
	}
	bool operator!=(const alternated_iterator& other) const {
		return _logical_neq(other, std::index_sequence_for<Iterators...>());
	}
private:
	template<std::size_t ...I>	
	bool _logical_neq(const alternated_iterator& other, std::index_sequence<I...>) const {
		return (... && (std::get<I>(_its) != std::get<I>(other._its)));
	}
	std::array<std::variant<Iterators...>, sizeof...(Iterators)> _its;
	std::size_t _index;
};

template<typename ...Iterables>
auto alternated_begin(Iterables&... iterables) {
	return alternated_iterator<Iterables...>(std::begin(iterables)...);
}

template<typename ...Iterables>
auto alternated_end(Iterables&... iterables) {
	return alternated_iterator<Iterables...>(std::end(iterables)...);
}

template<typename ...Iterables>
class alternated_range_impl {
public:
	using iterator = alternated_iterator<typename std::remove_reference_t<Iterables>::iterator...>;
	using value_type = typename iterator::value_type;
	using reference = typename iterator::reference;
	using pointer = typename iterator::pointer;

	alternated_range_impl(std::add_rvalue_reference_t<Iterables>... iterables)
		: t(iterables...) {
	}
	iterator begin() { return std::apply(alternated_begin<std::remove_reference_t<Iterables>...>, t); }
	iterator end() { return std::apply(alternated_end<std::remove_reference_t<Iterables>...>, t); }
	std::size_t size() const { return _size(); }
private:
	template<std::size_t I>
	std::size_t _size() const {
		if constexpr (I < sizeof...(Iterables)-1) {
			return std::min(_element_size<I>(), _element_size<I+1>());
		}
		return _element_size<I>();
	}
	template<std::size_t I>
	std::size_t _element_size() const {
		// the size of a cycle_range_impl would overflow due to multiplication or addition
		// add a special case to handle cycle_range_impls
		using T = std::tuple_element_t<I, decltype(t)>;
		if constexpr(std::is_same_v<std::remove_reference_t<T>, cycle_range<T>>) {
			return std::numeric_limits<std::size_t>::max();
		}
		return std::get<I>(t).size() * sizeof...(Iterables) + I;
	}
	std::tuple<Iterables...> t;
};

template<typename ...Iterables>
auto alternated_range(Iterables&&... iterables) {
	return alternated_range_impl<Iterables...>(std::forward<Iterables>(iterables)...);
}

} // namespace iterutils

#endif

