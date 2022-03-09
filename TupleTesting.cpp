#include <initializer_list>
#include <iostream>
#include <ostream>
#include <tuple>
#include <utility>

/**
 * Counting class.
 */
template <int N> struct Test {
  static inline int Normal{0};
  static inline int Normal2{0};
  static inline int Copy{0};
  static inline int Move{0};
  static inline int CopyAssign{0};
  static inline int MoveAssign{0};
  static inline int Destructor{0};

  Test() = delete;
  Test(int) noexcept { ++Normal; }
  Test(int, int) noexcept { ++Normal2; }
  Test(Test const &) noexcept { ++Copy; }
  Test(Test &&) noexcept { ++Move; }
  Test &operator=(Test const &) noexcept { ++CopyAssign; }
  Test &operator=(Test &&) noexcept { ++MoveAssign; }
  ~Test() noexcept { ++Destructor; }

  static void zeroValues() noexcept {
    Normal = 0;
    Normal2 = 0;
    Copy = 0;
    Move = 0;
    CopyAssign = 0;
    MoveAssign = 0;
    Destructor = 0;
  }

  /**
   * An inner class is used for printing to minimize the instantiations of the
   * counted class.
   */
  struct Print {
    friend std::ostream &operator<<(std::ostream &os, Print) {
      return os << "Normal: " << Normal << ", Normal2: " << Normal2
                << ", Copy: " << Copy << ", Move: " << Move
                << ", CopyAssign: " << CopyAssign
                << ", MoveAssign: " << MoveAssign
                << ", Destructor: " << Destructor;
    }
  };
};

/**
 * A wrapper class that provides an tuple converting constructor.
 * If the tuple generation can use a converting constructor, it creates the
 * elements in place (no move needed).
 */
template <int N, template <int> typename F> struct ThinWrapper : F<N> {
private:
  template <typename... Args, std::size_t... IDs>
  ThinWrapper(std::tuple<Args...> const &param, std::index_sequence<IDs...>)
      : F<N>(std::get<IDs>(param)...) {}

public:
  template <typename... Args>
  ThinWrapper(std::tuple<Args...> const &param)
      : ThinWrapper(param, std::make_index_sequence<sizeof...(Args)>{}) {}
};
template <int N> using TW_Test = ThinWrapper<N, Test>;

/**
 * Using the class itself with a converting constructor.
 */
std::tuple<Test<0>, Test<1>, Test<2>> bestCase() { return {0, 1, 2}; }

/**
 * Same as the best case, however it is a deeply nested recursive function.
 * Relies on RVO.
 * NRVO could be used to make a more realistic example with the same effect.
 */
template <unsigned N = 100>
std::tuple<Test<0>, Test<1>, Test<2>> constexprRecursion() {
  if constexpr (N > 0) {
    return constexprRecursion<N - 1>();
  } else {
    return {0, 1, 2};
  }
}

/**
 * Same as the best case, however it is a deeply nested recursive function
 * (runtime). Relies on RVO. NRVO could be used to make a more realistic example
 * with the same effect.
 */
std::tuple<Test<0>, Test<1>, Test<2>> runtimeRecursion(unsigned i = 100) {
  if (i > 0) {
    return runtimeRecursion(i - 1);
  } else {
    return {0, 1, 2};
  }
}

/**
 * Same as the best case, however it is a deeply nested recursive function.
 * Relies on NRVO.
 */
template <unsigned N = 5>
std::tuple<Test<0>, Test<1>, Test<2>> recursionNRVO() {
  if constexpr (N > 0) {
    // this line can cause a move (gcc-11: yes, clang-13: no)
    auto res = constexprRecursion<N - 1>();
    return res;
  } else {
    std::tuple<Test<0>, Test<1>, Test<2>> res = {0, 1, 2};
    return res;
  }
}

/**
 * Using the constructor of the class directly.
 */
std::tuple<Test<0>, Test<1>, Test<2>> typicalCase() {
  return {Test<0>{0, 0}, Test<1>{1, 1}, Test<2>{2, 2}};
}

/**
 * Using the constructor of the class directly without specifying it directly.
 */
std::tuple<Test<0>, Test<1>, Test<2>> typicalNonSpecific() {
  return {{0, 0}, {1, 1}, {2, 2}};
}

/**
 * Using a wrapper class, that has a converting constructor for tuple.
 * This enables the use of multiple parameters in the context of a converting
 * constructor.
 */
std::tuple<TW_Test<0>, TW_Test<1>, TW_Test<2>> wrappedCase() {
  using tup = std::tuple<int, int>;
  tup tmp{42, 43};
  return {tmp, tmp, tmp};
}

/**
 * Prints the results and resets all counters so the class can be used again.
 */
void evalAndReset(std::string const &Name) {
  std::string delim("\n  ");
  std::cout << Name << ":" << delim << Test<0>::Print{} << delim
            << Test<1>::Print{} << delim << Test<2>::Print{} << "\n\n";
  Test<0>::zeroValues();
  Test<1>::zeroValues();
  Test<2>::zeroValues();
}

/**
 * CAUTION: Wrapping the test calls in their own braces forces the destruction
 * of all objects. Do not remove them!
 *
 * CAUTION 2: This test sweep does not consider the times the parameters are
 * copied/moved. If your parameters are expensive, this can be problematic.
 */
int main() {
  { auto res = bestCase(); }
  evalAndReset("bestCase");

  { auto res = constexprRecursion(); }
  evalAndReset("constexprRecursion");

  { auto res = runtimeRecursion(); }
  evalAndReset("runtimeRecursion");

  { auto res = recursionNRVO(); }
  evalAndReset("recursionNRVO");

  { auto res = typicalCase(); }
  evalAndReset("typicalCase");

  { auto res = typicalNonSpecific(); }
  evalAndReset("typicalNonSpecific");

  { auto res = wrappedCase(); }
  evalAndReset("wrappedCase");
}
