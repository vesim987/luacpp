#include <gtest/gtest.h>

#include <string>
#include <tuple>

#include <luacpp.hpp>

TEST(State, StateCreation) { luacpp::State state; }

TEST(State, CallOperator) {
  luacpp::State state;

  EXPECT_TRUE(state("x = 10"));
}

TEST(Selector, GetBool) {
  luacpp::State state;

  state("x = true");

  ASSERT_EQ(bool(state["x"]), true);
}

TEST(Selector, SetAndGetBool) {
  luacpp::State state;

  state["x"] = true;

  ASSERT_EQ(bool(state["x"]), true);
}

TEST(Selector, GetInteger) {
  luacpp::State state;

  state("x = 10");

  ASSERT_EQ(int(state["x"]), 10);
}

TEST(Selector, SetAndGetInteger) {
  luacpp::State state;

  state["x"] = 10;

  ASSERT_EQ(int(state["x"]), 10);
}

TEST(Selector, SetAndGetIntegerFromMap) {
  luacpp::State state;

  state["map"]["x"] = 10;
  state["x"] = 20;

  ASSERT_EQ(int(state["map"]["x"]), 10);
}

TEST(Selector, CallLuaWithoutArgumentsAndGetValue) {
  luacpp::State state;

  EXPECT_TRUE(state(R"lua(
      function foo()
        return 100
      end
   )lua"));

  int result = state["foo"]();

  ASSERT_EQ(result, 100);
}

TEST(Selector, CallLuaWithArgumentsAndGetValue) {
  luacpp::State state;

  EXPECT_TRUE(state(R"lua(
      function foo(x)
        return x*2
      end
   )lua"));

  int result = state["foo"](20);

  ASSERT_EQ(result, 40);
}

TEST(Selector, CallLuaAndGetTuple) {
  luacpp::State state;

  EXPECT_TRUE(state(R"lua(
      function foo()
        return "a", 10
      end
   )lua"));

  std::tuple<std::string, int> tuple = state["foo"]();

  ASSERT_EQ(std::get<std::string>(tuple), "a");
  ASSERT_EQ(std::get<int>(tuple), 10);
}

static int test(int a, int b) { return a + b; }

TEST(Selector, SetFunction) {
  luacpp::State state;

  state["test"].register_function<test>();
}

TEST(Selector, SetAndExecuteFunction) {
  luacpp::State state;

  state["test"].register_function<test>();

  EXPECT_TRUE(state("x = test(5, 5)"));
}

TEST(Selector, SetAndExecuteTestReturnValueFunction) {
  luacpp::State state;

  state["test"].register_function<test>();

  state("x = test(5, 5)");

  ASSERT_EQ(int(state["x"]), 10);
}

static std::tuple<int, std::string> test_tuple() { return {20, "30"}; }

TEST(Selector, SetAndExecuteFucntionWithTuple) {
  luacpp::State state;

  state["test"].register_function<test_tuple>();

  state("x, y = test()");

  int x = state["x"];
  std::string y = state["y"];

  ASSERT_EQ(x, 20);
  ASSERT_EQ(y, "30");
}
    

/* TEST(Selector, SetStatelessLambda) { */
/*     luacpp::State state; */

/*     static auto test_lambda = []() -> int { return 50; }; */


/*     state["test"].register_function<&test_lambda>(); */ //TODO: i don't know how but i will make it work at somepoint
/* } */

/* TEST(Selector, RegisterEmptyClass) { */
/*     luacpp::State state; */

/*     class Foo {}; */

/*     state.register_class<CLASS(Foo)>(); */ //TODO: add some black magic to support CLASS macro overloading
/* } */ 

class Foo {
 public:
  Foo() {}
  ~Foo() {}

  void foo(int val) { baz = val; }
  int bar() { return baz; }
  int baz = 0;

};

TEST(Selector, RegisterClassWithMembers) {
  luacpp::State state;
  using Foo_ = ::luacpp::detail::Class<Foo, 'F', 'o', 'o'>;
  using Foo_foo = ::luacpp::detail::Member<&Foo::foo, 'f', 'o', 'o'>;
  using Foo_bar = ::luacpp::detail::Member<&Foo::bar, 'b', 'a', 'r'>;
  state.register_class<Foo_, Foo_foo, Foo_bar>();
}

TEST(Selector, RegisterClassWithMembersUsingMacro) {
  luacpp::State state;
  state.register_class<CLASS(Foo, (foo, bar))>();
}

TEST(Selector, RegisterClassWithFields) {
  luacpp::State state;
  state.register_class<CLASS(Foo, (baz))>();
}

TEST(Selector, RegisterClassWithMemebersAndFields) {
  luacpp::State state;
  state.register_class<CLASS(Foo, (foo, bar, baz))>();
}

TEST(Selector, RegisterClassAddCalFromCpp) {
  luacpp::State state;
  state.register_class<CLASS(Foo, (foo, bar, baz))>();

  Foo *obj = state["Foo"]();

  ASSERT_FALSE(obj == nullptr);
}

TEST(Selector, CallClassMembersFromLua) {
  luacpp::State state;
  state.register_class<CLASS(Foo, (foo, bar))>();

  EXPECT_TRUE(state(R"lua(
      foo = Foo()
      foo:foo(10)
      x = foo:bar()
   )lua"));

  Foo *obj = state["foo"];
  int x_from_obj = obj->bar();

  int x_from_method_call = state["foo"]["bar"]();

  ASSERT_EQ(x_from_obj, 10);
  ASSERT_EQ(x_from_method_call, 10);

  ASSERT_EQ(int(state["x"]), 10);
}

TEST(Selector, SetAndGetClassFieldFromLua) {
  luacpp::State state;
  state.register_class<CLASS(Foo, (foo, bar, baz))>();

  EXPECT_TRUE(state(R"lua(
      foo = Foo()
      foo:baz(20)
      x = foo:baz()
   )lua"));

  ASSERT_EQ(int(state["x"]), 20);
}
