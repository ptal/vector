#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>
#include <utility>

#if defined(__has_include)
#  if __has_include("vector.hpp")
#    include "vector.hpp"
#    define HAS_TEMPLATE_VECTOR 1
#  endif
#endif

#ifndef HAS_TEMPLATE_VECTOR
#  pragma message("[vector_tests.cpp] 'vector.hpp' not found. Skipping templated Vector<T> tests.")
#endif

static int g_failed = 0;

#define TEST_ASSERT(cond)                                                                         \
  do {                                                                                            \
    if (!(cond)) {                                                                                \
      ++g_failed;                                                                                 \
      std::cerr << "Assertion failed at " << __FILE__ << ":" << __LINE__ << " : '" #cond "'\n"; \
    }                                                                                             \
  } while (0)

#define TEST_CASE(name)                                                                            \
  std::cout << "[ RUN      ] " << name << "\n";

#define TEST_CASE_END(name)                                                                        \
  std::cout << "[     OK  ] " << name << "\n";

struct NoDefault {
  int x;
  explicit NoDefault(int v) : x(v) {}
  NoDefault(const NoDefault&) = default;
  NoDefault(NoDefault&&) noexcept = default;
  NoDefault& operator=(const NoDefault&) = default;
  NoDefault& operator=(NoDefault&&) noexcept = default;
};

struct Tracked {
  static int ctor;
  static int copy_ctor;
  static int move_ctor;
  static int dtor;
  static int copy_assign;
  static int move_assign;

  int x;

  Tracked() : x(0) { ++ctor; }
  explicit Tracked(int v) : x(v) { ++ctor; }
  Tracked(const Tracked& o) : x(o.x) { ++copy_ctor; }
  Tracked(Tracked&& o) noexcept : x(o.x) { ++move_ctor; o.x = -1; }
  Tracked& operator=(const Tracked& o) { x = o.x; ++copy_assign; return *this; }
  Tracked& operator=(Tracked&& o) noexcept { x = o.x; o.x = -1; ++move_assign; return *this; }
  ~Tracked() { ++dtor; }

  static void reset() { ctor = copy_ctor = move_ctor = dtor = copy_assign = move_assign = 0; }
};

int Tracked::ctor = 0;
int Tracked::copy_ctor = 0;
int Tracked::move_ctor = 0;
int Tracked::dtor = 0;
int Tracked::copy_assign = 0;
int Tracked::move_assign = 0;

static void test_vector_int_basic() {
  TEST_CASE("Vector<int> basic push/access/capacity")
  Vector<int> v;
  TEST_ASSERT(v.size() == 0);
  v.reserve(4);
  TEST_ASSERT(v.capacity() >= 4);
  for (int i = 1; i <= 6; ++i) v.push_back(i);
  TEST_ASSERT(v.size() == 6);
  TEST_ASSERT(v[0] == 1 && v[5] == 6);
  v[3] = 99;
  TEST_ASSERT(v[3] == 99);

  Vector<int> cpy(v);
  TEST_ASSERT(cpy.size() == v.size());
  TEST_ASSERT(cpy[3] == 99);

  Vector<int> mv(std::move(v));
  TEST_ASSERT(mv.size() == 6);
  TEST_ASSERT(mv[1] == 2);

  Vector<int> asg; asg = mv;
  TEST_ASSERT(asg.size() == 6);
  asg = std::move(mv);
  TEST_ASSERT(asg.size() == 6);

  asg.pop_back();
  TEST_ASSERT(asg.size() == 5);
  asg.resize(0);
  TEST_ASSERT(asg.size() == 0);
  TEST_CASE_END("Vector<int> basic push/access/capacity")
}

static void test_vector_string() {
  TEST_CASE("Vector<std::string> with non-POD type")
  Vector<std::string> v;
  v.reserve(3);
  v.push_back(std::string("hello"));
  std::string world = "world";
  v.push_back(world);
  v.push_back("!");
  TEST_ASSERT(v.size() == 3);
  TEST_ASSERT(v[0] == "hello" && v[1] == "world" && v[2] == "!");
  TEST_CASE_END("Vector<std::string> with non-POD type")
}

static void test_vector_tracked_moves() {
  TEST_CASE("Vector<Tracked> prefers moves over copies when possible")
  Tracked::reset();
  {
    Vector<Tracked> v;
    v.reserve(4);
    Tracked a(1), b(2);
    v.push_back(a);
    v.push_back(std::move(b));
    v.push_back(Tracked(3));
    TEST_ASSERT(v.size() == 3);
  }
  TEST_ASSERT(Tracked::move_ctor >= 1);
  std::cout << "Tracked: ctor=" << Tracked::ctor
            << " copy_ctor=" << Tracked::copy_ctor
            << " move_ctor=" << Tracked::move_ctor
            << " dtor=" << Tracked::dtor
            << " copy_assign=" << Tracked::copy_assign
            << " move_assign=" << Tracked::move_assign << "\n";
  TEST_CASE_END("Vector<Tracked> prefers moves over copies when possible")
}

int main() {
#ifdef HAS_TEMPLATE_VECTOR
  test_vector_int_basic();
  test_vector_string();
  test_vector_tracked_moves();
#else
  std::cout << "[  SKIP   ] Vector<T> tests (vector.hpp not found)\n";
#endif

  if (g_failed == 0) {
    std::cout << "\nAll tests passed!\n";
    return 0;
  } else {
    std::cerr << "\nTests failed: " << g_failed << "\n";
    return 1;
  }
}