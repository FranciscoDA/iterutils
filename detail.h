#ifndef _ITERUTILS_DETAIL_H_
#define _ITERUTILS_DETAIL_H_

#include <functional>
#include <utility>
#include <type_traits>
#include <tuple>

namespace iterutils::detail {
	template<typename T> using arg_from_uref_t = std::conditional_t<std::is_lvalue_reference_v<T>, T, T&&>;
}

#endif
