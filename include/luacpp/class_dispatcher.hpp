#pragma once

#include "func_traits.hpp"
#include "util.hpp"
#include "value_dispatcher.hpp"

namespace luacpp::detail {
template <class Class> struct ClassDispatcher {
  using ClassType = typename Class::type;

  static int new_(lua_State *l) {
    ValueDispatcher<ClassType *>::set_value(l);
    luaL_setmetatable(l, Class::Name);

    return 1;
  }

  static int delete_(lua_State *l) {
    auto obj = ValueDispatcher<ClassType *>::template get_value<1>(l);
    if constexpr (std::is_destructible_v<ClassType>)
      obj->~ClassType();

    return 0;
  }

  template <class Member> static int callback(lua_State *l) {
    using FuncTraits = luacpp::FuncTraits<typename Member::type>;
    static_assert(FuncTraits::type == Type::Member || FuncTraits::type == Type::Field);
    auto obj = reinterpret_cast<ClassType *>(luaL_checkudata(l, 1, Class::Name));

    if constexpr (FuncTraits::type == Type::Member) {
      return method_callback_helper<Member::Func, FuncTraits>(l, obj, std::make_index_sequence<FuncTraits::ArgCount>{});
    } else if (FuncTraits::type == Type::Field) {
      if (lua_gettop(l) == 2) {
        (obj->*(Member::Func)) = ValueDispatcher<int>::template get_value<2>(l);
        return 0;
      } else if (lua_gettop(l) == 1) {
        ValueDispatcher<int>::set_value(l, (obj->*(Member::Func)));
        return 1;
      } else {
        throw "Inavlid number of arguments for class member"; // TODO: throw that into lua
      }
    }
  }

private:
  template <auto Method, class FuncTraits, std::size_t... I>
  static inline int method_callback_helper(lua_State *l, ClassType *obj, std::index_sequence<I...>) {
    using ClassType = typename Class::type;
    using Ret = typename FuncTraits::Ret;

    if constexpr (!std::is_same_v<Ret, void>) {
      auto value =
          (obj->*Method)(ValueDispatcher<typename FuncTraits::template Arg<I>>::template get_value<I + 2>(l)...);
      ValueDispatcher<Ret>::set_value(l, value);
      return ValueDispatcher<Ret>::Count;
    } else {
      (obj->*Method)(ValueDispatcher<typename FuncTraits::template Arg<I>>::template get_value<I + 2>(l)...);
      return 0;
    }
  }
};

} // namespace luacpp::detail
