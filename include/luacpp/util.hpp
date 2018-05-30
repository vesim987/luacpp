#pragma once

#include <utility>

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}

#include "const_string.hpp"
#include "macro_foreach.hpp"

namespace luacpp::detail {
template <auto F, char... N> struct Member {
  using type = decltype(F);
  static constexpr auto Func = F;
  static constexpr const char Name[] = {N...};
};

template <typename T, char... N> struct Class {
  using type = T;
  static constexpr const char Name[] = {N...};
};

template <char... N> struct StringWrapper {
  static constexpr const char String[] = {N...};
};

template <std::size_t Length> constexpr std::size_t strlen(const char (&str)[Length]) {
  std::size_t i = 0;
  for (i = 0; str[i]; ++i)
    ;
  return i;
}

template <typename T, char... N, std::size_t... Is, typename String = StringWrapper<N...>>
Class<T, String::String[Is]...> create_class_helper(std::index_sequence<Is...>) {
  return {};
}

template <typename T, char... N>
decltype(create_class_helper<T, N...>(std::make_index_sequence<strlen({N...}) + 1>{})) create_class() {
  return {};
}

template <auto F, char... N, std::size_t... Is, typename String = StringWrapper<N...>>
Member<F, String::String[Is]...> create_member_helper(std::index_sequence<Is...>) {
  return {};
}

template <auto F, char... N>
decltype(create_member_helper<F, N...>(std::make_index_sequence<strlen({N...}) + 1>{})) create_member() {
  return {};
}

} // namespace luacpp::detail

#define ESC(...) __VA_ARGS__

#define MEMBER(C, ...) FOR_EACH(MEMBER_HELPER, C, __VA_ARGS__)
#define MEMBER_HELPER(C, S) , decltype(::luacpp::detail::create_member<&C::S, CONST_STRING(#S)>())

#define MEMBER_(C, M) MEMBER(C, ESC M)

// TODO: add namespace support
#define CLASS(NAME, ...)                                                                                               \
  decltype(::luacpp::detail::create_class<NAME, CONST_STRING(#NAME)>()) MEMBER_(NAME, __VA_ARGS__)
