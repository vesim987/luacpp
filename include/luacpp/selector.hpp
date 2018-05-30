#include <functional>
#include <tuple>
#include <type_traits>

#include "function_dispatcher.hpp"
#include "util.hpp"

namespace luacpp {

class State;

template <int Depth = 0> class Selector {
  friend State;
  friend Selector<Depth - 1>;

private:
  lua_State *const l = nullptr;
  const std::array<std::string, Depth + 1> path = {}; // FIXME: travelsal is not working too well

  template <class... T> Selector(lua_State *l, T &&... name) : l(l), path({std::forward<T>(name)...}) {}

  Selector(Selector &) = delete;
  Selector(Selector &&) = delete;

  void operator=(Selector &) = delete;
  void operator=(Selector &&) = delete;

public:
  ~Selector() = default;

private:
  template <class T, std::size_t... Is> Selector<Depth + 1> create_nested(T &&name, std::index_sequence<Is...>) const {
    return {l, path[Is]..., std::forward<T>(name)};
  }

public:
  template <class T>
  std::enable_if_t<std::is_constructible_v<std::string, T>, Selector<Depth + 1>> operator[](T &&name) const {
    return create_nested(std::forward<T>(name), std::make_index_sequence<Depth + 1>{});
  }

  template <class T>
  std::enable_if_t<std::is_invocable_v<decltype(ValueDispatcher<T>::set_value), lua_State *, T>>
  operator=(T &&value) const {
    for (int i = 0; i < Depth; ++i) {
      lua_createtable(l, 0, 0); // FIXME: it is creating new table everytime is called
    }

    using Dispatcher = ValueDispatcher<T>;
    Dispatcher::set_value(l, std::forward<T>(value));

    if constexpr (Depth == 0) {
      lua_setglobal(l, path[0].c_str());
    } else {
      for (int i = Depth - 1; i >= 0; --i) {
        if (i == 0) {
          lua_setglobal(l, path[i].c_str());
        } else {
          lua_setfield(l, -2, path[i].c_str());
        }
      }
    }
  }

  template <auto Func> void register_function() const {
    for (int i = 0; i < Depth; ++i) { // TODO: wrap it into function
      lua_createtable(l, 0, 0);
    }

    lua_pushcfunction(l, (&detail::FunctionDispatcher<Func>::callback));

    if constexpr (Depth == 0) {
      lua_setglobal(l, path[0].c_str());
    } else {
      for (int i = path.size() - 1; i >= 0; --i) {
        if (i == 0) {
          lua_setglobal(l, path[i].c_str());
        } else {
          lua_setfield(l, -2, path[i].c_str());
        }
      }
    }
  }

  template <class T,
            std::enable_if_t<std::is_invocable_v<decltype(ValueDispatcher<T>::template get_value<-1>), lua_State *>> * =
                nullptr>
  operator T() const {
    traverse();
    return ValueDispatcher<T>::template get_value<-1>(l);
  }

  class LazyReturnValue { // FIXME: that will fail when value is not immediately casted
    friend Selector<Depth>;
    lua_State *const l;

    LazyReturnValue(lua_State *l) : l(l) {}

  public:
    template <class T,
              std::enable_if_t<std::is_invocable_v<decltype(ValueDispatcher<T>::template get_value<-1>), lua_State *>>
                  * = nullptr>
    operator T() const {
      return ValueDispatcher<T>::template get_value<1>(l);
    }
  };

  template <class... Args> LazyReturnValue operator()(Args &&... args) {
    traverse();

    // push arguments
    (0 | ... | (ValueDispatcher<Args>::set_value(l, std::forward<Args>(args)), 0));

    lua_pcall(l, sizeof...(Args), LUA_MULTRET, 0); // TODO: error handler
    return {l};
  }

private:
  void traverse() const { // TODO: use pack expansion for that
    if constexpr (Depth == 0) {
      lua_getglobal(l, path[0].c_str());
    } else {
      for (int i = 0; i < Depth; ++i) {
        if (i == 0) {
          lua_getglobal(l, path[i].c_str());
        } else {
          lua_getfield(l, -1, path[i].c_str());
        }
      }
    }
  }
};

} // namespace luacpp
