/*****************************************************************/ /**
 * @file   error_reporter.h
 * @brief  Contains composable reporters.
 *
 * @author RPC
 * @date   January 2024
 *********************************************************************/
#ifndef HG_COLTC_ERROR_REPORTER
#define HG_COLTC_ERROR_REPORTER

#include <colt/dsa/option.h>
#include <colt/dsa/string_view.h>
#include "io_reporter.h"

namespace clt::lng
{
  /// @brief A Reporter is any type supporting message(...), error(...), warn(...) methods
  template<typename T, typename... Args>
  concept Reporter = requires(
      T reporter, StringView str, const Option<SourceInfo>& src_info,
      const Option<ReportNumber>& msg_nb) {
    {
      reporter.message(str, src_info, msg_nb)
    } -> std::same_as<void>;
    {
      reporter.error(str, src_info, msg_nb)
    } -> std::same_as<void>;
    {
      reporter.warn(str, src_info, msg_nb)
    } -> std::same_as<void>;
  };

  /// @brief What is the diagnostic type
  enum class ReportKind : u8
  {
    /// @brief A Message
    MESSAGE,
    /// @brief A Warning
    WARNING,
    /// @brief An Error
    ERROR
  };

  namespace details
  {
    template<Reporter T>
    class ToErrorReporter;
  }

  /// @brief Base class for all error reporting mechanism
  class ErrorReporter
  {
    template<Reporter T>
    friend class details::ToErrorReporter;

    /// @brief The error count
    u64 _error_count = 0;
    /// @brief The warning count
    u64 _warn_count = 0;
    /// @brief The message count
    u64 _message_count = 0;

  public:
    /// @brief Reports a message
    /// @param str The message string
    /// @param src_info The source information
    /// @param msg_nb The message number
    virtual void message(
        StringView str, const Option<SourceInfo>& src_info = None,
        const Option<ReportNumber>& msg_nb = None) noexcept = 0;
    /// @brief Reports a warning
    /// @param str The warning string
    /// @param src_info The source information
    /// @param msg_nb The warning number
    virtual void warn(
        StringView str, const Option<SourceInfo>& src_info = None,
        const Option<ReportNumber>& msg_nb = None) noexcept = 0;
    /// @brief Reports an error
    /// @param str The error string
    /// @param src_info The source information
    /// @param msg_nb The error number
    virtual void error(
        StringView str, const Option<SourceInfo>& src_info = None,
        const Option<ReportNumber>& msg_nb = None) noexcept = 0;

    /// @brief Returns the count of errors generated
    /// @return The count of errors
    u64 error_count() const noexcept { return _error_count; }
    /// @brief Returns the count of warnings generated
    /// @return The count of warnings
    u64 warn_count() const noexcept { return _warn_count; }
    /// @brief Returns the count of messages generated
    /// @return The count of messages
    u64 message_count() const noexcept { return _message_count; }

    /// @brief Destructor
    virtual ~ErrorReporter() noexcept {};
  };

  namespace details
  {
    template<Reporter Rep>
    /// @brief Helper to convert a 'Reporter' to an ErrorReporter
    struct ToErrorReporter
        : public ErrorReporter
        , public Rep
    {
      template<typename... Args>
      constexpr ToErrorReporter(Args&&... args) noexcept(
          std::is_nothrow_constructible_v<Rep, Args...>)
          : Rep(std::forward<Args>(args)...)
      {
      }

      void message(
          StringView str, const Option<SourceInfo>& src_info = None,
          const Option<ReportNumber>& msg_nb = None) noexcept override
      {
        ++ErrorReporter::_message_count;
        Rep::message(str, src_info, msg_nb);
      }

      void warn(
          StringView str, const Option<SourceInfo>& src_info = None,
          const Option<ReportNumber>& msg_nb = None) noexcept override
      {
        ++ErrorReporter::_warn_count;
        Rep::warn(str, src_info, msg_nb);
      }

      void error(
          StringView str, const Option<SourceInfo>& src_info = None,
          const Option<ReportNumber>& msg_nb = None) noexcept override
      {
        ++ErrorReporter::_error_count;
        Rep::error(str, src_info, msg_nb);
      }

      ~ToErrorReporter() override{};
    };
  } // namespace details

  /*template<
      Reporter Rep, auto ALLOCATOR = mem::GlobalAllocatorDescription,
      typename... Args>
  /// @brief Creates an error reporter from a composable error reporter
  /// @tparam Rep The error reporter type
  /// @param args The arguments to forward to the error reporter constructor
  /// @return UniquePtr to the error reporter
  UniquePtr<ErrorReporter> make_error_reporter(Args&&... args) noexcept(
      std::is_nothrow_constructible_v<Rep, Args...>)
  {
    return make_unique<details::ToErrorReporter<Rep>>(std::forward<Args>(args)...);
  }*/
} // namespace clt::lng

#endif // !HG_COLTC_ERROR_REPORTER
