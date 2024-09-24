#include <includes.h>
#include <util/ffi/ffi_caller.h>
#include <colt/os/dynamic_lib.h>

struct MyStruct2
{
  int a;
  int b;
  COLT_ENABLE_REFLECTION();
};
COLT_DECLARE_TYPE(MyStruct2, a, b);

struct MyStruct
{
  int a;
  void* b;
  MyStruct2 c;

  COLT_ENABLE_REFLECTION();
};
COLT_DECLARE_TYPE(MyStruct, a, b, c);

MyStruct test()
{
  return {10, (void*)0xdeadbeefULL, {100, 200}};
}

extern "C" CLT_EXPORT u32 colt_test_dl()
{
  return 32;
}

TEST_CASE("coltc FFICaller")
{
  auto value = clt::ffi::FFICaller::call<MyStruct>(&test);
  REQUIRE(value.a == 10);
  REQUIRE(value.b == (void*)0xdeadbeefULL);
  REQUIRE(value.c.a == 100);
  REQUIRE(value.c.b == 200);
}

TEST_CASE("coltc DynamicLib")
{
  using namespace clt;
  auto err = os::DynamicLib::open();
  REQUIRE(err.is_value());
  auto ptr = err->find<u32 (*)()>("colt_test_dl");
  REQUIRE(ptr != nullptr);
  REQUIRE((*ptr)() == 42);
}
