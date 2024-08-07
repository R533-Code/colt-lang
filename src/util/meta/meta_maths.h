/*****************************************************************/ /**
 * @file   meta_maths.h
 * @brief  Contains constexpr equivalent of some mathematical functions.
 *
 * @author RPC
 * @date   January 2024
 *********************************************************************/
#ifndef HG_COLT_MATH
#define HG_COLT_MATH

#include <cmath>
#include <type_traits>
#include <concepts>
#include <numeric>
#include <numbers>

#include "common/types.h"

namespace clt
{
  template<std::forward_iterator ForwardIt>
  constexpr ForwardIt min_element(ForwardIt first, ForwardIt last)
  {
    if (first == last)
      return last;

    ForwardIt smallest = first;
    ++first;

    for (; first != last; ++first)
      if (*first < *smallest)
        smallest = first;

    return smallest;
  }

  template<std::forward_iterator ForwardIt>
  constexpr ForwardIt max_element(ForwardIt first, ForwardIt last)
  {
    if (first == last)
      return last;

    ForwardIt largest = first;
    ++first;

    for (; first != last; ++first)
      if (*largest < *first)
        largest = first;

    return largest;
  }

  template<meta::Integral Int>
    requires std::is_signed_v<Int>
  /// @brief Returns the absolute value (distance from zero) of an integer
  /// @param value The value whose absolute value to compute
  /// @return The absolute value
  constexpr std::make_unsigned_t<Int> abs(Int value) noexcept
  {
    assert_true(
        "Invalid argument for 'abs'!", value != std::numeric_limits<Int>::min());
    if (std::is_constant_evaluated())
    {
      if (value < 0)
        return static_cast<std::make_unsigned_t<Int>>(-value);
      return static_cast<std::make_unsigned_t<Int>>(value);
    }
    else
      return static_cast<std::make_unsigned_t<Int>>(std::abs(value));
  }

  template<meta::FloatingPoint Flt>
  /// @brief Returns the absolute value (distance from zero) of a floating point
  /// @param value The value whose absolute value to compute
  /// @return The absolute value
  constexpr Flt abs(Flt value) noexcept
  {
    if (std::is_constant_evaluated())
    {
      if (value < static_cast<Flt>(0))
        return -value;
      return value;
    }
    else
      return std::abs(value);
  }

  template<meta::Integral Int>
  /// @brief Returns the minimum of two values
  /// @param a The first integer
  /// @param b The second integer
  /// @return The minimum of the two values
  constexpr Int min(Int a, Int b) noexcept
  {
    return (b < a) ? b : a;
  }

  template<typename T>
  /// @brief Returns the minimum of a list
  /// @param ilist The list whose minimum to return
  /// @return The minimum of a list
  constexpr T min(std::initializer_list<T> ilist)
  {
    return *clt::min_element(ilist.begin(), ilist.end());
  }

  template<meta::Integral Int>
  /// @brief Returns the maximum of two values
  /// @param a The first integer
  /// @param b The second integer
  /// @return The maximum of the two values
  constexpr Int max(Int a, Int b) noexcept
  {
    return (a < b) ? b : a;
  }

  template<typename T>
  /// @brief Returns the maximum of a list
  /// @param ilist The list whose maximum to return
  /// @return The maximum of a list
  constexpr T max(std::initializer_list<T> ilist) noexcept
  {
    return *clt::max_element(ilist.begin(), ilist.end());
  }

  template<meta::FloatingPoint Fp>
  /// @brief Rounds a floating point value
  /// @param x The float to round
  /// @return The rounded float
  constexpr Fp round(Fp x) noexcept
  {
    if (std::is_constant_evaluated())
    {
      if (x >= (std::numeric_limits<Fp>::radix / std::numeric_limits<Fp>::epsilon()))
        return x;
      if (x
          <= -(std::numeric_limits<Fp>::radix / std::numeric_limits<Fp>::epsilon()))
        return x;

      if (x > 0)
      {
        auto floor_x = (Fp)(uintmax_t)x;
        if (x - floor_x >= static_cast<Fp>(0.5))
          floor_x += static_cast<Fp>(1.0);
        return floor_x;
      }

      if (x < 0)
        return -clt::round(-x);
      return x; //  x is 0.0, -0.0 or NaN
    }
    else
      return std::round(x);
  }

  template<meta::FloatingPoint Fp>
  //Forward declaration as 'ceil' makes use of 'floor'
  constexpr Fp floor(Fp x) noexcept;

  template<meta::FloatingPoint Fp>
  /// @brief Returns the ceiling of a floating point value
  /// @param x The float value to ceil
  /// @return The ceiled value
  constexpr Fp ceil(Fp x) noexcept
  {
    if (std::is_constant_evaluated())
    {
      if (x >= (std::numeric_limits<Fp>::radix / std::numeric_limits<Fp>::epsilon()))
        return x;
      if (x
          <= -(std::numeric_limits<Fp>::radix / std::numeric_limits<Fp>::epsilon()))
        return x;

      if (x > 0)
        return (Fp)((uintmax_t)x + 1);

      if (x < 0)
        return -clt::floor(-x);
      return x; // x is 0.0, -0.0 or NaN
    }
    else
      return std::ceil(x);
  }

  template<meta::FloatingPoint Fp>
  /// @brief Returns the flooring of a floating point value
  /// @param x The float value to floor
  /// @return The floored value
  constexpr Fp floor(Fp x) noexcept
  {
    if (std::is_constant_evaluated())
    {
      if (x >= (std::numeric_limits<Fp>::radix / std::numeric_limits<Fp>::epsilon()))
        return x;
      if (x
          <= -(std::numeric_limits<Fp>::radix / std::numeric_limits<Fp>::epsilon()))
        return x;

      if (x > 0)
        return (Fp)(uintmax_t)x;

      if (x < 0)
        return -clt::ceil(-x);
      return x; // x is 0.0, -0.0 or NaN
    }
    else
      return std::floor(x);
  }

  namespace details
  {
    template<meta::FloatingPoint Fp>
    /// @brief Computes the root of a floating point
    /// @param A The base
    /// @param n The root (greater than 1.0)
    /// @return The n-th root of A
    constexpr Fp root(Fp A, Fp n) noexcept
    {
      Fp result;

      Fp delta, newVal, oldVal = A / n;
      do
      {
        newVal = oldVal;
        for (int i = 1; i < n - 1; ++i)
          newVal *= oldVal;

        newVal = (A / newVal + (n - 1) * oldVal) / n;
        delta  = oldVal - newVal;
        if (delta < 0)
          delta = -delta;
        result = oldVal = newVal;
      } while (delta > 0.00001);
      return result;
    }
  } // namespace details

  template<meta::FloatingPoint Fp>
  /// @brief Returns 'base' to the power 'power'
  /// @param base The base
  /// @param power The power
  /// @return Power
  constexpr Fp pow(Fp base, Fp power) noexcept
  {
    if (std::is_constant_evaluated())
    {
      if (power == static_cast<Fp>(0.0))
        return static_cast<Fp>(1.0);
      if (base == static_cast<Fp>(1.0))
        return static_cast<Fp>(1.0);
      bool is_neg = power < static_cast<Fp>(0.0);
      power       = clt::abs(power);

      auto result = static_cast<Fp>(1.0);
      while (power >= static_cast<Fp>(1.0))
      {
        result *= base;
        power -= static_cast<Fp>(1.0);
      }
      if (power != static_cast<Fp>(0.0))
        result *= details::root(base, 1 / power);

      return is_neg ? 1 / result : result;
    }
    else
      return std::pow(base, power);
  }

  template<meta::FloatingPoint Fp = double, meta::Integral Int>
  /// @brief Returns 'what' to the power 'power'
  /// @param what The base
  /// @param power The exponent
  /// @return Power
  constexpr Fp pow(Int what, Int power) noexcept
  {
    return clt::pow(static_cast<Fp>(what), static_cast<Fp>(power));
  }

  template<meta::FloatingPoint Fp>
  /// @brief Returns the square root of 'x'
  /// @param x The float whose square root to return
  /// @return The square root of 'x'
  constexpr Fp sqrt(Fp x) noexcept
  {
    if (std::is_constant_evaluated())
      return clt::details::root(x, static_cast<Fp>(2.0));
    else
      return std::sqrt(x);
  }

  template<meta::FloatingPoint Fp = double, meta::Integral Int>
  /// @brief Returns the square root of 'x'
  /// @param x The integral whose square root to return
  /// @return The square root of 'x'
  constexpr Fp sqrt(Int x) noexcept
  {
    return clt::sqrt(static_cast<Fp>(x));
  }

  template<meta::FloatingPoint Fp>
  /// @brief Returns the e to the power 'x'
  /// @param x The power of 'e'
  /// @return exp(x)
  constexpr Fp exp(Fp x) noexcept
  {
    if (std::is_constant_evaluated())
      return clt::pow(std::numbers::e_v<Fp>, x);
    else
      return std::exp(x);
  }

  template<meta::FloatingPoint Fp = double, meta::Integral Int>
  /// @brief Returns the e to the power 'x'
  /// @param x The power of 'e'
  /// @return exp(x)
  constexpr Fp exp(Int x) noexcept
  {
    return clt::exp(static_cast<Fp>(x));
  }

  template<meta::FloatingPoint Fp>
  /// @brief Returns the natural log of 'x'
  /// @param x The number whose natural log to return
  /// @return ln(x)
  constexpr Fp log(Fp x) noexcept
  {
    if (std::is_constant_evaluated())
    {
      Fp result = 0.0;
      while (x > static_cast<Fp>(4.0 / 3))
      {
        result += 1;
        x /= static_cast<Fp>(2.0);
      }
      result *= std::numbers::ln2_v<Fp>;

      Fp sum        = 0;
      Fp term       = 2 * (x - 1) / (x + 1);
      Fp multiplier = term * term;
      size_t k      = 1;
      while (term > static_cast<Fp>(1e-10))
      {
        sum += term / k;
        term *= multiplier;
        k += 2;
      }
      return result + sum;
    }
    else
      return std::log(x);
  }

  template<meta::FloatingPoint Fp = double, meta::Integral Int>
  /// @brief Returns the natural log of 'x'
  /// @param x The number whose natural log to return
  /// @return ln(x)
  constexpr Fp log(Int x) noexcept
  {
    return clt::log(static_cast<Fp>(x));
  }

  template<meta::FloatingPoint Fp>
  /// @brief Returns the log base 10 of 'x'
  /// @param x The number whose log base 10 to return
  /// @return log_10(x)
  constexpr Fp log10(Fp x) noexcept
  {
    if (std::is_constant_evaluated())
      return clt::log(x) / clt::log(static_cast<Fp>(10.0));
    else
      return std::log10(x);
  }

  template<meta::FloatingPoint Fp = double, meta::Integral Int>
  /// @brief Returns the log base 10 of 'x'
  /// @param x The number whose log base 10 to return
  /// @return log_10(x)
  constexpr Fp log10(Int x) noexcept
  {
    return clt::log10(static_cast<Fp>(x));
  }

  template<meta::FloatingPoint Fp>
  /// @brief Returns the log base 2 of 'x'
  /// @param x The number whose log base 2 to return
  /// @return log_2(x)
  constexpr Fp log2(Fp x) noexcept
  {
    if (std::is_constant_evaluated())
      return clt::log(x) / clt::log(static_cast<Fp>(2.0));
    else
      return std::log2(x);
  }

  template<meta::FloatingPoint Fp = double, meta::Integral Int>
  /// @brief Returns the log base 2 of 'x'
  /// @param x The number whose log base 2 to return
  /// @return log_2(x)
  constexpr Fp log2(Int x) noexcept
  {
    return clt::log2(static_cast<Fp>(x));
  }
} // namespace clt

#endif //!HG_COLT_MATH