#pragma once

#include <ez/Functional.hpp>
#include <ez/Traits.hpp>
#include <ez/Utils.hpp>

#include <exception>
#include <type_traits>

namespace ez {
/// ErrorInfo serves as the base class for specific error info types.
/// Each specific info class holds an explanatory messages for errors.
/// An ErrorInfo is specialized for each error code enum using EZ_ERROR_INFO
/// that requires an error info name and a code to string free function.
/// The objects of error info classes are treated as singletons, passed by const pointer.
class ErrorInfo {
public:
    ErrorInfo(const std::string& name) : m_name{name} {}

    ErrorInfo(const ErrorInfo&) = default;
    ErrorInfo(ErrorInfo&&) = default;
    ErrorInfo& operator=(const ErrorInfo&) = default;
    ErrorInfo& operator=(ErrorInfo&&) = default;

    virtual ~ErrorInfo() {}

    virtual std::string message(int value) const noexcept = 0;
    virtual const std::type_info& underlying_enum_type() const = 0;

    const std::string& name() const noexcept { return m_name; }

    bool operator==(const ErrorInfo& rhs) const noexcept
    {
        return underlying_enum_type() == rhs.underlying_enum_type();
    }
    bool operator!=(const ErrorInfo& rhs) const noexcept
    {
        return underlying_enum_type() != rhs.underlying_enum_type();
    }

    bool operator<(const ErrorInfo& rhs) const noexcept
    {
        return underlying_enum_type().before(rhs.underlying_enum_type());
    }

private:
    std::string m_name;
};

template <typename Enum>
const ErrorInfo* error_info() noexcept;

namespace internal {
template <typename ErrorEnum, typename ErrorMessageFn>
class ErrorInfoImpl : public ErrorInfo {
public:
    ErrorInfoImpl(const std::string& name, ErrorMessageFn errorMsgFn)
        : ErrorInfo{name}, m_value_to_message(std::move(errorMsgFn))
    {
    }

    std::string message(int code) const noexcept override
    {
        return m_value_to_message(static_cast<ErrorEnum>(code));
    }

    const std::type_info& underlying_enum_type() const noexcept override
    {
        return typeid(ErrorEnum);
    }

private:
    ErrorMessageFn m_value_to_message;
};

template <typename ErrorEnum, typename ErrorMessageFn>
ErrorInfoImpl<ErrorEnum, std::decay_t<ErrorMessageFn>> make_error_info_impl(const std::string& name,
                                                                            ErrorMessageFn&& fn)
{
    return {name, std::forward<ErrorMessageFn>(fn)};
}

}  // namespace internal
}  // namespace ez

#define EZ_ERROR_INFO(Enum, Name, value_to_message)                           \
    template <>                                                               \
    inline const ErrorInfo* ErrorInfo<Enum>() noexcept                        \
    {                                                                         \
        static const auto error_info =                                        \
            ez::internal::make_error_info_impl<Enum>(Name, value_to_message); \
        return &error_info;                                                   \
    }
///////////////////////////////////////////////////////////////////////////////

namespace ez {
/// Error is a generic type holding errors.
/// Each Error object holds an error code originating from an error code enum interface and a
/// pointer to an object of type ErrorInfo.
class Error : public std::exception {
public:
    Error(trait::IsEnum auto code)
        : m_code{ez::as_int(code)},
          m_info{error_info<decltype(code)>()},
          m_message{m_info->message(m_code)}
    {
    }

    Error(trait::IsEnum auto code, const std::string& message)
        : m_code{asInt(code)}, m_info{error_info<decltype(code)>()}, m_message{message}
    {
    }

    Error(const Error&) = default;
    Error(Error&&) = default;
    Error& operator=(const Error&) = default;
    Error& operator=(Error&&) = default;

    bool operator==(const Error& other) const;
    bool operator!=(const Error& other) const;
    bool operator<(const Error& other) const;

    const std::string& name() const noexcept;

    int code() const noexcept;
    const ErrorInfo& info() const noexcept;

    template <typename Enum>
    bool is() const noexcept;
    std::string id() const;

    const std::string& message() const noexcept;
    const char* what() const noexcept override;

private:
    int m_code = 0;
    const ErrorInfo* m_info = nullptr;
    std::string m_message;
};

///////////////////////////////////////////////////////////////////////////////

inline bool Error::operator==(const Error& other) const
{
    return m_code == other.m_code && (*m_info == *other.m_info);
}

inline bool Error::operator!=(const Error& other) const
{
    return m_code != other.m_code || (*m_info != *other.m_info);
}

inline bool Error::operator<(const Error& other) const
{
    return m_code < other.m_code || (m_code == other.m_code && *m_info < *other.m_info);
}

inline const std::string& Error::name() const noexcept { return m_info->name(); }

inline int Error::code() const noexcept { return m_code; }

inline std::string Error::id() const { return name() + "-" + std::to_string(m_code); }

inline const std::string& Error::message() const noexcept { return m_message; }

inline const char* Error::what() const noexcept { return m_message.c_str(); }

inline const ErrorInfo& Error::info() const noexcept { return *m_info; }

template <typename Enum>
bool Error::is() const noexcept
{
    return (*m_info == *error_info<Enum>());
}

}  // namespace ez
