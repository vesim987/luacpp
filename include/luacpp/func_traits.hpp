#pragma once

#include <cinttypes>
#include <tuple>

namespace luacpp {

template <class T> struct FuncTraits;

enum class Type { Function = 0, Member = 1, Field = 2 };

template <class ReturnType, class... Arguments> struct FuncTraits<ReturnType(Arguments...)> {
  using Ret = ReturnType;
  static constexpr Type type = Type::Function;
  template <std::size_t I> using Arg = typename std::tuple_element<I, std::tuple<Arguments...>>::type;
  static constexpr std::size_t ArgCount = sizeof...(Arguments);
};

template <class ReturnType, class C, class... Arguments> struct FuncTraits<ReturnType (C::*)(Arguments...)> {
  using Ret = ReturnType;
  using Class = C;
  static constexpr Type type = Type::Member;

  template <std::size_t I> using Arg = typename std::tuple_element<I, std::tuple<Arguments...>>::type;
  static constexpr std::size_t ArgCount = sizeof...(Arguments);
};

template <class ReturnType, class C> struct FuncTraits<ReturnType(C::*)> {
  using Ret = ReturnType;
  using Class = C;
  static constexpr Type type = Type::Field;
};

} // namespace luacpp
