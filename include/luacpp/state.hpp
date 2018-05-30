#include <stdexcept>
#include <string_view>

#include "class_dispatcher.hpp"
#include "selector.hpp"

namespace luacpp {

class State {
private:
  lua_State *l;

public:
  State() {
    l = luaL_newstate();
    if (l == nullptr)
      throw std::runtime_error("Failed to create lua state"); // TODO: custom exception type
  }

  State(const State &) = delete;
  State(State &&) = delete;

  void operator=(const State &) = delete;
  void operator=(State &&) = delete;

  ~State() {
    if (l != nullptr) {
      lua_gc(l, LUA_GCCOLLECT, 0);
      lua_close(l);
    }
    l = nullptr;
  }

  bool operator()(const char *code) {
    int status = luaL_dostring(l, code);
    int stack = lua_gettop(l);

    if (status && !lua_isnil(l, -1)) {
      auto msg = lua_tostring(l, -1);
      if (msg != nullptr)
        throw std::runtime_error(msg); // TODO: custom exception type
    }

    lua_settop(l, stack);
    // TODO: exception handling
    //
    return !status;
  }

  template <class T> Selector<0> operator[](T &&name) { return {l, std::forward<T>(name)}; }

  template <class Class, class... Members> void register_class() const {
    lua_register(l, Class::Name, (&detail::ClassDispatcher<Class>::new_));
    luaL_newmetatable(l, Class::Name);

    lua_pushcclosure(l, (&detail::ClassDispatcher<Class>::delete_), 0);
    lua_setfield(l, -2, "__gc");

    lua_pushvalue(l, -1);
    lua_setfield(l, -2, "__index");

    (0 | ... | (register_member<Class, Members>(), 0));
  }

private:
  template <class Class, class Member> void register_member() const {
    lua_pushcfunction(l, (&detail::ClassDispatcher<Class>::template callback<Member>));
    lua_setfield(l, -2, Member::Name);
  }
};

} // namespace luacpp
