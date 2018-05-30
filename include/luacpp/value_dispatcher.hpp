#pragma once

#include <string>
#include <tuple>

#include "util.hpp"

namespace luacpp {
template <class T> struct ValueDispatcher {};

template <> struct ValueDispatcher<double> {
  static constexpr std::size_t Count = 1;
  template <int Index> static double get_value(lua_State *l) { return lua_tonumber(l, Index); }
  static void set_value(lua_State *l, double value) { lua_pushnumber(l, value); }
};

template <> struct ValueDispatcher<int> {
  static constexpr std::size_t Count = 1;
  template <int Index> static int get_value(lua_State *l) { return lua_tointeger(l, Index); }
  static void set_value(lua_State *l, int value) { lua_pushinteger(l, value); }
};

template <> struct ValueDispatcher<bool> {
  static constexpr std::size_t Count = 1;
  template <int Index> static bool get_value(lua_State *l) { return lua_toboolean(l, Index); }
  static void set_value(lua_State *l, bool value) { lua_pushboolean(l, value); }
};

template <> struct ValueDispatcher<const char *> {
  static constexpr std::size_t Count = 1;
  template <int Index> static const char *get_value(lua_State *l) { return lua_tostring(l, Index); }
  static void set_value(lua_State *l, const char *value) { lua_pushstring(l, value); }
};

template <std::size_t N> struct ValueDispatcher<const char (&)[N]> : ValueDispatcher<const char *> {};

template <> struct ValueDispatcher<std::string> {
  template <int Index> static std::string get_value(lua_State *l) { return lua_tostring(l, Index); }
  static void set_value(lua_State *l, const std::string &value) { lua_pushstring(l, value.c_str()); }
};

template <class T> struct ValueDispatcher<T *> {
  template <int Index> static T *get_value(lua_State *l) {
    if (lua_rawlen(l, Index) != sizeof(T))
      return nullptr;
    return static_cast<T *>(lua_touserdata(l, Index));
  }
  template <class... Args> static void set_value(lua_State *l, Args &&... args) {
    new (lua_newuserdata(l, sizeof(T))) T(std::forward<Args>(args)...);
  }

  static void set_value(lua_State *l) { new (lua_newuserdata(l, sizeof(T))) T(); }
};

template <class... Types> struct ValueDispatcher<std::tuple<Types...>> {
  static constexpr std::size_t Count = sizeof...(Types);
  template <int Index> static std::tuple<Types...> get_value(lua_State *l) {
    return std::move(get_value_helper<Index>(l, std::index_sequence_for<Types...>{}));
  }

  template <int Index, std::size_t... Is>
  static std::tuple<Types...> get_value_helper(lua_State *l, std::index_sequence<Is...>) {
    return {ValueDispatcher<Types>::template get_value<Index + Is>(l)...};
  }

  static void set_value(lua_State *l, const std::tuple<Types...> &args) {
    set_value_helper(l, args, std::index_sequence_for<Types...>{});
  }

  template <std::size_t... Is>
  static void set_value_helper(lua_State *l, const std::tuple<Types...> &args, std::index_sequence<Is...>) {
    (0 | ... | (ValueDispatcher<std::tuple_element_t<Is, std::tuple<Types...>>>::set_value(l, std::get<Is>(args)), 0));
  }
};

} // namespace luacpp
