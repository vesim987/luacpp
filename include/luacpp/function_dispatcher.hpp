#pragma once

#include "func_traits.hpp"
#include "util.hpp"
#include "value_dispatcher.hpp"

namespace luacpp::detail {
template <auto Func> struct FunctionDispatcher {
  static int callback(lua_State *l) {
    using FuncTraits = luacpp::FuncTraits<std::remove_pointer_t<decltype(Func)>>;
    return callback_helper<FuncTraits>(l, std::make_index_sequence<FuncTraits::ArgCount>{});
  }

private:
  template <class FuncTraits, std::size_t... I>
  inline int static callback_helper(lua_State *l, std::index_sequence<I...>) {
    using Ret = typename FuncTraits::Ret;

    if constexpr (!std::is_same_v<Ret, void>) {
      auto value = Func(ValueDispatcher<typename FuncTraits::template Arg<I>>::template get_value<I + 1>(l)...);
      ValueDispatcher<Ret>::set_value(l, value);
      return ValueDispatcher<Ret>::Count;
    } else {
      Func(ValueDispatcher<typename FuncTraits::template Arg<I>>::template get_value<I + 1>(l)...);
      return 0;
    }
  }
};

} // namespace luacpp::detail
