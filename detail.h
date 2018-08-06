#ifndef _ITERUTILS_DETAIL_H_
#define _ITERUTILS_DETAIL_H_

#include <functional>
#include <utility>
#include <type_traits>
#include <tuple>

namespace iterutils::detail {
	// converts a type captured by universal reference to
	// a type usable in a constructor argument list
	// i.e:
	//  T  (captured by rvalue ref) -> T&&
	//  T& (captured by lvalue ref) -> T&
	template<typename T> using arg_from_uref_t = std::conditional_t<std::is_lvalue_reference_v<T>, T, T&&>;
}

#endif
