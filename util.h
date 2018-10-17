#ifndef _ITERUTILS_UTIL_H_
#define _ITERUTILS_UTIL_H_

#include <type_traits>
#include <iterator>

namespace iterutils {

template<typename T>
struct is_infinite : public std::false_type { };
template<typename T>
using is_infinite_v = typename is_infinite<T>::value;

namespace detail{

template<template<typename ...TArgs> class TemplateName, typename ...Iterators>
using specialize_iterator_from_iterators = TemplateName<
	std::common_type_t<typename std::iterator_traits<Iterators>::iterator_category...>,
	Iterators...
>;
template<template<typename ...TArgs> class TemplateName, typename ...Iterables>
using specialize_iterator_from_iterables = specialize_iterator_from_iterators<TemplateName, typename std::remove_reference_t<Iterables>::iterator...>;
template<template<typename ...TArgs> class TemplateName, typename ...Iterables>
using specialize_const_iterator_from_iterables = specialize_iterator_from_iterators<TemplateName, typename std::remove_reference_t<Iterables>::const_iterator...>;

template<size_t I, typename Iterable>
std::pair<size_t, size_t> shortest_iterable_impl(const Iterable& last) {
	static_assert(!is_infinite<Iterable>::value, "Iterable is infinite");
	return {std::size(last), I};
}
template<size_t I, typename FirstIterable, typename ...Iterables>
std::pair<size_t, size_t> shortest_iterable_impl(const FirstIterable& iterable1, const Iterables& ...iterables) {
	if constexpr (is_infinite<FirstIterable>::value)
		return shortest_iterable_impl<I+1, Iterables...>(iterables...);
	else if constexpr (std::conjunction_v<is_infinite<Iterables>...>)
		return shortest_iterable_impl<I, FirstIterable>(iterable1);
	else
		return std::min(shortest_iterable_impl<I, FirstIterable>(iterable1), shortest_iterable_impl<I+1, Iterables...>(iterables...));
}

} // namespace detail
template<typename ...Iterables>
auto shortest_iterable(const Iterables& ...iterables) {
	return detail::shortest_iterable_impl<0, Iterables...>(iterables...);
}

} // namespace iterutils

#endif
