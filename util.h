#ifndef _ITERUTILS_UTIL_H_
#define _ITERUTILS_UTIL_H_

namespace iterutils {
namespace detail{

/*template<typename TemplateName, typename ...Iterators>
struct specialize_iterator_view_for {
	using type = TemplateName<
		std::common_type_t<typename Iterators::iterator_category...>,
		Iterators...
	>;
};

template<typename TemplateName, typename ...Iterators>
using specialize_iterator_view_for_t = typename specialized_iterator_view_for<TemplateName, Iterators...>::type;*/

template<template<typename ...TArgs> class TemplateName, typename ...Iterators>
using specialize_iterator_from_iterators = TemplateName<
	std::common_type_t<typename Iterators::iterator_category...>,
	Iterators...
>;
template<template<typename ...TArgs> class TemplateName, typename ...Iterables>
using specialize_iterator_from_iterables = specialize_iterator_from_iterators<TemplateName, typename std::remove_reference_t<Iterables>::iterator...>;
template<template<typename ...TArgs> class TemplateName, typename ...Iterables>
using specialize_const_iterator_from_iterables = specialize_iterator_from_iterators<TemplateName, typename std::remove_reference_t<Iterables>::const_iterator...>;

} // namespace detail
} // namespace iterutils

#endif
