#ifndef TYPENAME_HH
#define TYPENAME_HH


#if __cplusplus < 201103L

#error "typeName.hh requires compilation as C++11 or higher"

#elif __cplusplus < 201703L

/*
 *  C++11 solution adapted from:
 *    - https://stackoverflow.com/a/20170989
 */

#include <type_traits>  // remove_reference
#include <typeinfo>     // typeid
#    ifndef _MSC_VER
#include <cxxabi.h>  // abi::__cxa_demangle
#    endif  // not MSVC
#include <memory>       // unique_ptr
#include <string>
#include <cstdlib>      // free

/*
 * @brief Returns string of a data type name as it would be printed by the
 *   compiler in error messages.
 *
 * @notes `typeid(obj).name()` return is implementation-dependent; with g++
 *   the names are "mangled" to ensure unique identification of overloaded
 *   functions. Object names can be demangled, also in an
 *   implementation-specific manner. With GNU, that could be at the command
 *   line with `c++filt`, or in c++ code with (__cxxabiv1)abi::__cxa_demangle(),
 *   see:
 *   - https://stackoverflow.com/a/4465907
 *   - http://gcc.gnu.org/onlinedocs/libstdc++/manual/ext_demangling.html
 *   - https://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-html-USERS-4.3/a01696.html
 *   - https://en.wikipedia.org/wiki/Name_mangling
 *   - https://www.cplusplus.com/forum/beginner/175177/#msg866884
 */
template <class T>
std::string typeName(void) {
    typedef typename std::remove_reference<T>::type TR;
#    ifndef _MSC_VER
    std::unique_ptr<char, void(*)(void*)> demangled (
            abi::__cxa_demangle(typeid(TR).name(), nullptr,
                                nullptr, nullptr),
            std::free);
    std::string result { demangled.get() };
#    else   // MSVC
    std::string result { typeid(TR).name() };
#    endif  // MSVC
    if (std::is_const<TR>::value)
        result += " const";
    if (std::is_volatile<TR>::value)
        result += " volatile";
    if (std::is_lvalue_reference<T>::value)
        result += "&";
    else if (std::is_rvalue_reference<T>::value)
        result += "&&";
    return result;
}

#else   // C++17 and above

/*
 * C++17 solution adapted from:
 *   - https://stackoverflow.com/a/59522794
 *
 * As an upgade to the C++11 version, this solution will resolve at compile time.
 */

#include <string_view>

namespace impl {

/*
 * @brief Leverages compiler builtin functions to return raw type name C-string.
 *
 * @notes Both these builtins here yield the name of the function itself, but
 *   since rawTypeName is templated, the resolved template type is also in the
 *   function name.
 */
template <typename T>
[[nodiscard]] constexpr std::string_view rawTypeName() {
#    ifndef _MSC_VER
#        if defined(__GNUC__) && __GNUC__ < 8
    // __PRETTY_FUNCTION__ return is not constexpr in lower versions of g++, see:
    //   - https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66639
#error "typeName requires g++ version 8.0.0 and above when " +
    "compiling as C++17 and above"
#        endif  // GNU libstdc++ version < 8.0.0
    return __PRETTY_FUNCTION__;
#    else   // MSVC
    return __FUNCSIG__;
#    endif  // MSVC
}

/*
 * @brief Stores data about extraneous characters in the output of rawTypeName.
 *
 * @notes rawTypeName output will have prefixes and suffixes that can vary by
 *   compiler and version.
 */
struct TypeNameFormat {
    std::size_t prefix_sz { 0 };
    std::size_t total_extra_chars { 0 };
};

/*
 * @brief Using known data type `int` to measure prefix and total extra char
 *   counts for compiler used.
 */
constexpr TypeNameFormat type_name_format {
    []() constexpr {
        std::string_view tn { "int" };
        std::string_view raw_tn { rawTypeName<int>() };
        return TypeNameFormat {
            raw_tn.find(tn),           // prefix_sz
            raw_tn.size() - tn.size()  // total_extra_chars
        };
    }()
};
static_assert(type_name_format.prefix_sz != std::string_view::npos,
              "Unable to determine the type name format on this compiler.");

/*
 * @brief Making the result static to save re-evaluating for successive calls
 *   with the same data type.
 */
template <typename T>
static constexpr std::string_view type_name_storage {
    []() constexpr {
        constexpr std::string_view raw_tn { rawTypeName<T>() };
        return raw_tn.substr(
            type_name_format.prefix_sz,
            raw_tn.size() - type_name_format.total_extra_chars
            );
    }()
};

}  // namespace impl

/*
 * @brief Returns string_view of a data type name as it would be printed by the
 *   compiler in error messages.
 */
template <typename T>
[[nodiscard]] constexpr std::string_view typeName() {
    return impl::type_name_storage<T>;
}

/*
 * @brief Returns C-string of a data type name as it would be printed by the
 *   compiler in error messages.
 */
template <typename T>
[[nodiscard]] constexpr const char *typeNameCstr() {
    return impl::type_name_storage<T>.data();
}

#endif  // C++17 and above


#endif  // TYPENAME_HH
