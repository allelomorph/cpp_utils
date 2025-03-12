#ifndef SAFELIBCCALL_HH
#define SAFELIBCCALL_HH

/**
 * @file safeLibcCall.hh
 */

#include <functional>
#include <stdexcept>     // runtime_error
#include <string_view>
#include <sstream>
#include <system_error>  // error_condition, system_category
#include <type_traits>   // invoke_result_t


/**
 * @file safeLibcCall.hh
 *
 * @note
 * [LibcRetErrTest](#LibcRetErrTest), [LibcRetTest](#LibcRetTest), and
 *   [LibcErrTest](#LibcErrTest) made children of std::function instead of
 *   aliases due to need to differentiate testing only errno (int) vs only an
 *   int return value.
 * If performace becomes an issue similar aliases to function pointer types
 *   would work, but at the cost of making the errno/int return differentiation,
 *   and being able to pass in functors.
 * Note that when instantiating these types with a lambda, ReturnType must
 *   be passed as a template parameter to allow its deduction in safeLibcCall.
 *
 * @see [Comparison of std::function vs function pointers](https://stackoverflow.com/q/25848690)
 */

/**
 * @brief Allows passing of user-defined test of glibc function failure
 *   condition (either by return or errno values) into safeLibcCall.
 *
 * @tparam ReturnType return type of glibc function
 *
 * @see [safeLibcCall](#safeLibcCall)
 */
template<typename ReturnType>
class LibcRetErrTest : public std::function<bool(const ReturnType, const int)> {};

/**
 * @brief Allows passing of user-defined test of glibc function failure
 *   condition (by return value only) into safeLibcCall.
 *
 * @tparam ReturnType return type of glibc function
 *
 * @see [safeLibcCall](#safeLibcCall)
 */
template<typename ReturnType>
class LibcRetTest : public std::function<bool(const ReturnType)> {};

/**
 * @brief Allows passing of user-defined test of glibc function failure
 *   condition (by errno value only) into safeLibcCall.
 *
 * @see [safeLibcCall](#safeLibcCall)
 */
class LibcErrTest : public std::function<bool(const int)> {};


/**
 * @brief Wraps a glibc function call, throwing a descriptive exception when
 *   the function fails (when detecting by return or errno values).
 *
 * @tparam FuncType prototype of glibc function
 * @tparam ReturnType return type of glibc function
 * @tparam ParamType parameter pack of glibc function parameter types
 *
 * @return return value of glibc function
 *
 * @throw std::runtime_error "(libc_func_name): failure without setting errno"
 *   in case of failure by return value
 * @throw std::system_error "(libc_func_name): (system error string for errno)"
 *   in case of failure by errno (eg error string is usually std::strerror(errno))
*/
template<typename FuncType, typename ReturnType, typename ...ParamTypes>
ReturnType safeLibcCall(const FuncType&& libc_func,
                        const std::string_view& libc_func_name,
                        const LibcRetErrTest<ReturnType>& is_failure,
                        ParamTypes ...params)
{
    errno = 0;
    ReturnType retval { libc_func(params...) };
    if (is_failure(retval, errno)) {
        std::ostringstream msg;
        msg << libc_func_name;
        if (errno == 0) {
            msg << ": failure without setting errno";
            throw std::runtime_error(msg.str());
        } else {
            const std::error_condition econd {
                std::system_category().default_error_condition(errno) };
            throw std::system_error(econd.value(), econd.category(),
                                    msg.str());
        }
    }
    return retval;
}

/**
 * @brief Wraps a glibc function call, throwing a descriptive exception when
 *   the function fails (when detecting only by return value).
 *
 * @tparam FuncType prototype of glibc function
 * @tparam ReturnType return type of glibc function
 * @tparam ParamType parameter pack of glibc function parameter types
 *
 * @return return value of glibc function
 *
 * @throw std::runtime_error "(libc_func_name): failure without setting errno"
 *   in case of failure by return value
 * @throw std::system_error "(libc_func_name): (system error string for errno)"
 *   in case of failure by errno (eg error string is usually std::strerror(errno))
 */
template<typename FuncType, typename ReturnType, typename ...ParamTypes>
ReturnType safeLibcCall(const FuncType&& libc_func,
                        const std::string_view& libc_func_name,
                        const LibcRetTest<ReturnType>& is_failure,
                        ParamTypes ...params)
{
    errno = 0;
    ReturnType retval { libc_func(params...) };
    if (is_failure(retval)) {
        std::ostringstream msg;
        msg << libc_func_name;
        if (errno == 0) {
            msg << ": failure without setting errno";
            throw std::runtime_error(msg.str());
        } else {
            const std::error_condition econd {
                std::system_category().default_error_condition(errno) };
            throw std::system_error(econd.value(), econd.category(),
                                    msg.str());
        }
    }
    return retval;
}

/**
 * @brief Wraps a glibc function call, throwing a descriptive exception when
 *   the function fails (when detecting only by arbitrary errno value).
 *
 * @tparam FuncType prototype of glibc function
 * @tparam ParamType parameter pack of glibc function parameter types
 *
 * @return return value of glibc function
 *
 * @throw std::runtime_error "(libc_func_name): failure without setting errno"
 *   in case of failure with errno of 0
 * @throw std::system_error "(libc_func_name): (system error string for errno)"
 *   in case of failure by errno (eg error string is usually std::strerror(errno))
 */
template<typename FuncType, typename ...ParamTypes>
auto safeLibcCall(const FuncType&& libc_func,
                  const std::string_view& libc_func_name,
                  const LibcErrTest& is_failure,
                  ParamTypes ...params) ->
    std::invoke_result_t<FuncType, ParamTypes...>
{
    errno = 0;
    auto retval { libc_func(params...) };
    if (is_failure(errno)) {
        std::ostringstream msg;
        msg << libc_func_name;
        if (errno == 0) {
            msg << ": failure without setting errno";
            throw std::runtime_error(msg.str());
        } else {
            const std::error_condition econd {
                std::system_category().default_error_condition(errno) };
            throw std::system_error(econd.value(), econd.category(),
                                    msg.str());
        }
    }
    return retval;
}

/**
 * @brief Wraps a glibc function call, throwing a descriptive exception when
 *   the function fails (when detecting only by non-zero errno value).
 *
 * @tparam FuncType prototype of glibc function
 * @tparam ParamType parameter pack of glibc function parameter types
 *
 * @return return value of glibc function
 *
 * @throw std::system_error "(libc_func_name): (system error string for errno)"
 *   in case of failure by errno (eg error string is usually std::strerror(errno))
 */
template<typename FuncType, typename ...ParamTypes>
auto safeLibcCall(const FuncType&& libc_func,
                  const std::string_view& libc_func_name,
                  ParamTypes ...params) ->
    std::invoke_result_t<FuncType, ParamTypes...>
{
    errno = 0;
    auto retval { libc_func(params...) };
    if (errno != 0) {
        const std::error_condition econd {
            std::system_category().default_error_condition(errno) };
        throw std::system_error(econd.value(), econd.category(),
                                libc_func_name.data());
    }
    return retval;
}

#endif  // SAFELIBCCALL_HH
