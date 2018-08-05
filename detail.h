#ifndef _ITERUTILS_DETAIL_H_
#define _ITERUTILS_DETAIL_H_

#include <functional>
#include <utility>
#include <type_traits>

namespace iterutils::detail {
	template<typename T>
	using arg_from_uref_t = std::conditional_t<std::is_lvalue_reference_v<T>, T, T&&>;

	template<typename F, typename M, typename T, std::size_t ...I>
	auto transform_apply_impl(F&& f, M&& m, T&& tup, std::index_sequence<I...>) {
		return std::invoke(std::forward<F>(f), std::invoke(std::forward<M>(m), std::get<I>(std::forward<T>(tup)))...);
	}
	template<typename F, typename M, typename T>
	auto transform_apply(F&& f, M&& m, T&& tup) {
		return transform_apply_impl(std::forward<F>(f), std::forward<M>(m), std::forward<T>(tup));
	}
}

#endif
