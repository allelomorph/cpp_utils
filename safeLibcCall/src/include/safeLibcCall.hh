#ifndef SAFELIBCCALL_HH
#define SAFELIBCCALL_HH


#include <functional>
#include <stdexcept>     // runtime_error
#include <string_view>
#include <sstream>
#include <system_error>  // error_condition, system_category


/*
 * Using child classes instead of aliases due to need to differentiate testing
 *   only errno (int) vs only an int return value.
 * If performace becomes an issue similar aliases to function pointer types
 *   would work, but at the cost of making the errno/int return differentiation,
 *   and being able to pass in functors. For comparison of std::function vs
 *   function pointers: https://stackoverflow.com/q/25848690
 * Note that when instantiating these types with a lambda, ReturnType must
 *   be passed as a template parameter to allow its deduction in safeLibcCall
 */
template<typename ReturnType>
class LibcRetErrTest : public std::function<bool(const ReturnType, const int)> {};

template<typename ReturnType>
class LibcRetTest : public std::function<bool(const ReturnType)> {};

class LibcErrTest : public std::function<bool(const int)> {};


template<typename FuncType, typename ReturnType, typename ...ParamTypes>
ReturnType safeLibcCall(FuncType&& libc_func,
                        const std::string_view& libc_func_name,
                        const LibcRetErrTest<ReturnType>& is_failure,
                        ParamTypes ...params) {
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

template<typename FuncType, typename ReturnType, typename ...ParamTypes>
ReturnType safeLibcCall(FuncType&& libc_func,
                        const std::string_view& libc_func_name,
                        const LibcRetTest<ReturnType>& is_failure,
                        ParamTypes ...params) {
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

template<typename FuncType, typename ...ParamTypes>
void safeLibcCall(FuncType&& libc_func,
                  const std::string_view& libc_func_name,
                  const LibcErrTest& is_failure,
                  ParamTypes ...params) {
    errno = 0;
    libc_func(params...);
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
}

template<typename FuncType, typename ...ParamTypes>
void safeLibcCall(FuncType&& libc_func,
                  const std::string_view& libc_func_name,
                  ParamTypes ...params) {
    errno = 0;
    libc_func(params...);
    if (errno != 0) {
        const std::error_condition econd {
            std::system_category().default_error_condition(errno) };
        throw std::system_error(econd.value(), econd.category(),
                                libc_func_name.data());
    }
}


#endif  // SAFELIBCCALL_HH
