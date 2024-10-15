/*****************************************************************/ /**
 * @file   target_info.h
 * @brief  Contains TargetInfo, which describes basic informations
 *         about the target to compile to.
 * 
 * @author RPC
 * @date   October 2024
 *********************************************************************/
#ifndef HG_COLT_TARGET_INFO
#define HG_COLT_TARGET_INFO

#include <colt/bit/endian.h>
#include <colt/meta/reflect.h>

DECLARE_ENUM_WITH_TYPE(
    u8, clt::lng, PrimitiveType,
    iw08b, // integer with 8 bits
    iw16b, // integer with 16 bits
    iw32b, // integer with 32 bits
    iw64b, // integer with 64 bits
    fw32b, // float   with 32 bits
    fw64b, // float   with 64 bits
    dtptr, // data      pointer
    fnptr  // function  pointer
);

namespace clt::lng
{
  /// @brief Describes a target informations low-level informations.
  /// Describes the alignment and size informations of primitive types.
  /// For now, there is no need to add struct support as the primitive
  /// types might be enough to compute alignment and size
  struct TargetInfo
  {
    /// @brief The alignment and size information of primitive types.
    struct PrimitiveTypeInfo
    {
      /// @brief The size in bytes
      u8 size;
      /// @brief The alignment in bytes
      /// This is usually equal to the size.
      u8 align;
    };

    /// @brief The endianness of the target
    enum class Endianess : u8
    {
      /// @brief Little Endian
      little_endian,
      /// @brief Big Endian
      big_endian,
    };

    /// @brief The endianness of the target
    Endianess endian;

    /// @brief The information that describe primitive types.
    std::array<PrimitiveTypeInfo, meta::reflect<PrimitiveType>::max()>
        primitive_infos;
  };
} // namespace clt::lng

#endif // !HG_COLT_TARGET_INFO
