#ifndef HG_COLT_AST_GLOBALS
#define HG_COLT_AST_GLOBALS

#include <colt_pch.h>

DECLARE_ENUM_WITH_TYPE(
    u8, clt::lng, Evaluability,
    /// @brief Compile-time only
    CTE,
    /// @brief Runtime only
    RTE,
    /// @brief Both (at least theoretically)
    ANY);

DECLARE_ENUM_WITH_TYPE(
    u8, clt::lng, ArgSpecifier,
    /// @brief No argument specifier
    NONE,
    /// @brief By pointer to initialized non-mutable (or by copying for small types)
    IN,
    /// @brief By pointer to initialized mutable
    INOUT,
    /// @brief By pointer to uninitialized
    OUT);

DECLARE_ENUM_WITH_TYPE(
    u8, clt::lng, Visibility,
    /// @brief The global is visible in other modules (and is exported if dylib)
    PUBLIC,
    /// @brief The global is only visible in the current module
    PRIVATE);

DECLARE_ENUM_WITH_TYPE(
    u8, clt::lng, MetaInfoKind,
    /// @brief Represents an error
    ERROR,
    /// @brief Represents a module
    MODULE,
    /// @brief Scope (which contains information about local variables)
    SCOPE,
    /// @brief Local variable (has a Scope as parent)
    VAR_LOCAL,
    /// @brief Global variable (either FN or MODULE as parent)
    VAR_GLOBAL,
    /// @brief Function
    FN,
    /// @brief Function parameter
    FN_PARAM,
    /// @brief Function return
    FN_RET,
    /// @brief Class
    CLASS,
    /// @brief Class member (aka field)
    CLASS_MEMBER,

    /// @brief Generic global variable
    GEN_VAR_GLOBAL,
    /// @brief Generic function
    GEN_FN,
    /// @brief Generic class
    GEN_CLASS,
    /// @brief Generic parameter
    GEN_PARAM, );

namespace clt::lng
{
  /// @brief When can something be evaluated/exist
  enum class Evaluability : u8;

  /// @brief How an argument is to be passed
  enum class ArgSpecifier : u8;

  /// @brief The visibility of the global
  enum class Visibility : u8;

  /// @brief The meta information kind
  enum class MetaInfoKind : u8;

  /// @brief Represents the result of reflecting over Colt constructs.
  class MetaInfo
  {
    /// @brief The info kind
    MetaInfoKind kind;

  public:
    /// @brief True if the current MetaInfo is an error
    /// @return True if an error
    bool is_error() noexcept { return kind == MetaInfoKind::ERROR; }
    /// @brief True if the current MetaInfo is a module
    /// @return True if a module
    bool is_module() noexcept { return kind == MetaInfoKind::MODULE; }
    /// @brief True if the current MetaInfo is a scope
    /// @return True if a scope
    bool is_scope() noexcept { return kind == MetaInfoKind::SCOPE; }
    /// @brief True if the current MetaInfo is a local variable
    /// @return True if a local variable
    bool is_local_var() noexcept { return kind == MetaInfoKind::VAR_LOCAL; }
    /// @brief True if the current MetaInfo is a global variable
    /// @return True if a global variable
    bool is_global_var() noexcept { return kind == MetaInfoKind::VAR_GLOBAL; }
    /// @brief True if the current MetaInfo is a function
    /// @return True if a function
    bool is_fn() noexcept { return kind == MetaInfoKind::FN; }
    /// @brief True if the current MetaInfo is a function parameter
    /// @return True if a function parameter
    bool is_fn_param() noexcept { return kind == MetaInfoKind::FN_PARAM; }
    /// @brief True if the current MetaInfo is a function return
    /// @return True if a function return
    bool is_fn_ret() noexcept { return kind == MetaInfoKind::FN_RET; }
    /// @brief True if the current MetaInfo is a class
    /// @return True if a class
    bool is_class() noexcept { return kind == MetaInfoKind::CLASS; }
    /// @brief True if the current MetaInfo is a class member
    /// @return True if a class member
    bool is_class_member() noexcept { return kind == MetaInfoKind::CLASS_MEMBER; }
    /// @brief True if the current MetaInfo is a generic variable
    /// @return True if a class member
    bool is_generic_var() noexcept { return kind == MetaInfoKind::GEN_VAR_GLOBAL; }
    /// @brief True if the current MetaInfo is a generic function
    /// @return True if a generic function
    bool is_generic_fn() noexcept { return kind == MetaInfoKind::GEN_FN; }
    /// @brief True if the current MetaInfo is a generic class
    /// @return True if a generic class
    bool is_generic_class() noexcept { return kind == MetaInfoKind::GEN_CLASS; }
    /// @brief True if the current MetaInfo is a generic parameter
    /// @return True if generic parameter
    bool is_generic_param() noexcept { return kind == MetaInfoKind::GEN_PARAM; }
  };

  class Global
  {
    /// @brief The name of the global
    u8StringView name_;
    /// @brief The evaluability of the global
    Evaluability eval_;
    /// @brief The visibility of the global
    Visibility visibility_;
    /// @brief The parent of the global (can be null)
    const Global* parent_;

  public:
  };

  class BuiltinFnGlobal : public Global
  {
  };
} // namespace clt::lng

#endif // !HG_COLT_AST_GLOBALS
