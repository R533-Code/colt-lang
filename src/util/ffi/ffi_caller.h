/*****************************************************************/ /**
 * @file   ffi_caller.h
 * @brief  Contains wrapper over libffi.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#ifndef HG_UTIL_FFI_CALLER
#define HG_UTIL_FFI_CALLER

#include <type_traits>
#include <concepts>
#include <limits>
#include <ffi.h>
#include <colt/typedefs.h>
#include <colt/meta/reflect.h>

namespace clt::ffi
{
  /// @brief Wrapper over libffi.
  /// This class is NOT thread safe as type informations are stored
  /// in a mutable global variable.
  class FFICaller
  {
    /// @brief Converts a C++ type to its FFI equivalent
    /// @tparam T The type
    /// @return Pointer to ffi_type
    template<std::floating_point T>
    static consteval ffi_type* type_to_ffi() noexcept
    {
      if constexpr (std::is_same_v<T, float>)
        return &ffi_type_float;
      else if constexpr (std::is_same_v<T, double>)
        return &ffi_type_double;
      else if constexpr (std::is_same_v<T, long double>)
        return &ffi_type_longdouble;
    }

    /// @brief Converts a C++ type to its FFI equivalent
    /// @tparam T The type
    /// @return Pointer to ffi_type
    template<typename T>
      requires std::is_pointer_v<T>
    static consteval ffi_type* type_to_ffi() noexcept
    {
      return &ffi_type_pointer;
    }

    /// @brief Converts a C++ type to its FFI equivalent
    /// @tparam T The type
    /// @return Pointer to ffi_type
    template<std::same_as<void> T>
    static consteval ffi_type* type_to_ffi() noexcept
    {
      return &ffi_type_void;
    }

    /// @brief Converts a C++ type to its FFI equivalent
    /// @tparam T The type
    /// @return Pointer to ffi_type
    template<std::integral T>
    static consteval ffi_type* type_to_ffi() noexcept
    {
      if constexpr (
          std::is_same_v<T, u8>
          || (std::is_same_v<T, char> && !std::numeric_limits<char>::is_signed))
        return &ffi_type_uint8;
      else if constexpr (
          std::is_same_v<T, i8>
          || (std::is_same_v<T, char> && std::numeric_limits<char>::is_signed))
        return &ffi_type_sint8;
      else if constexpr (std::is_same_v<T, u16>)
        return &ffi_type_uint16;
      else if constexpr (std::is_same_v<T, i16>)
        return &ffi_type_sint16;
      else if constexpr (std::is_same_v<T, u32>)
        return &ffi_type_uint32;
      else if constexpr (std::is_same_v<T, i32>)
        return &ffi_type_sint32;
      else if constexpr (std::is_same_v<T, u64>)
        return &ffi_type_uint64;
      else if constexpr (std::is_same_v<T, i64>)
        return &ffi_type_sint64;
    }

    template<meta::reflectable T>
    static consteval auto generate_table()
    {
      using namespace meta;

      constexpr size_t SIZE = meta::reflect<T>::members_count();
      std::array<ffi_type*, SIZE + 1> ret;
      ret[SIZE] = nullptr;
      size_t i  = 0;
      for_each(
          Members, T{},
          [&]<typename Ty>(const Ty&) { ret[i++] = type_to_ffi<Ty>(); });
      return ret;
    }

    /// @brief Represents the array of ffi_type representing the
    /// members of a type
    /// @tparam T The reflectable type
    template<meta::reflectable T>
    static inline auto STRUCT_FFI_MEMBER_ARRAY = generate_table<T>();

    /// @brief Represents a custom FFI information for reflectable type.
    /// The memory is mutable
    /// @tparam T The reflectable type
    template<meta::reflectable T>
    static inline ffi_type STRUCT_FFI_TYPE = {
        0, 0, FFI_TYPE_STRUCT, STRUCT_FFI_MEMBER_ARRAY<T>.data()};

    /// @brief Converts a C++ type to its FFI equivalent
    /// @tparam T The type
    /// @return Pointer to ffi_type
    template<meta::reflectable T>
      requires(!std::is_fundamental_v<T> && !std::is_pointer_v<T>)
    static consteval ffi_type* type_to_ffi() noexcept
    {
      return &STRUCT_FFI_TYPE<T>;
    }

  public:
    /// @brief Calls a Foreign Function
    /// @tparam Ret The return type of the function
    /// @tparam ...Args The arguments type
    /// @param fn The address of the function
    /// @param ...args The arguments to pass to the function
    /// @return The return value of calling 'fn'
    template<typename Ret, typename... Args>
    static Ret call(void* fn, Args&&... args)
    {
      constexpr size_t SIZE         = sizeof...(Args) + 1;
      ffi_type* CONSTANT_ARGS[SIZE] = {type_to_ffi<Ret>(), type_to_ffi<Args>()...};

      std::conditional_t<(sizeof(Ret) < sizeof(long)), ffi_arg, Ret> result;

      ffi_cif cif;

      // Prepare the ffi_cif structure.
      ffi_status status = ffi_prep_cif(
          &cif, FFI_DEFAULT_ABI, SIZE - 1, CONSTANT_ARGS[0], CONSTANT_ARGS + 1);
      if (status != FFI_OK)
      {
        // Handle the ffi_status error.
      }
      void* arg_values[SIZE] = {((void*)&args)...};

      ffi_call(&cif, FFI_FN(fn), &result, arg_values);
      return static_cast<Ret>(result);
    }
  };
} // namespace clt

#endif // !HG_UTIL_FFI_CALLER
