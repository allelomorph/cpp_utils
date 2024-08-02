#ifndef ISSTREAMABLE_HH
#define ISSTREAMABLE_HH


#include <type_traits>  // true_type, false_type
#include <utility>      // declval
#include <iostream>     // basic_istream, basic_ostream
#include <concepts>

#if  __cplusplus >= 202002L

template<typename StreamT>
concept IsInputStream = std::derived_from<
    StreamT, std::basic_istream<typename StreamT::char_type>>;

template<typename StreamT>
concept IsOutputStream = std::derived_from<
    StreamT, std::basic_ostream<typename StreamT::char_type>>;

template<typename StreamT, typename T>
concept IsInputStreamable = requires (StreamT& stream, T obj) {
    stream >> obj;
};

template<typename StreamT, typename T>
concept IsOutputStreamable = requires (StreamT& stream, T obj) {
    stream << obj;
};

template<typename StreamT, typename T>
concept IsStreamable = (
    IsInputStreamable<StreamT, T> ||
    IsOutputStreamable<StreamT, T>
    );

#else  // __cplusplus < 202002L

// use of C++11 detection idiom for ostream operator adapted from:
//   - https://stackoverflow.com/a/22759544
// see also:
//   - https://benjaminbrock.net/blog/detection_idiom.php
//   - https://blog.tartanllama.xyz/detection-idiom/

template<typename StreamT, typename T>
class is_output_streamable {
private:
    template<typename TestStreamT, typename TestT>
    static auto test_ostream_op(int) -> decltype(
        std::declval<TestStreamT&>() << std::declval<TestT>(),
        std::true_type()
        );

    template<typename, typename>
    static auto test_ostream_op(...) -> std::false_type;

public:
    static constexpr bool value = decltype(
        test_ostream_op<StreamT, T>(int())
        )::value;
};

// TBD: currently the detection idiom is not working for istream operators as it
//   does for ostream ops
template<typename StreamT, typename T>
class is_input_streamable {
private:
    template<typename TestStreamT, typename TestT>
    static auto test_istream_op(int) -> decltype(
        std::declval<TestStreamT&>() >> std::declval<TestT>(),
        std::true_type()
        );

    template<typename, typename>
    static auto test_istream_op(...) -> std::false_type;

public:
    static constexpr bool value = decltype(
        test_istream_op<StreamT, T>(int())
        )::value;
};

template<typename StreamT, typename T>
struct is_streamable :
    public std::integral_constant<bool,
                                  is_input_streamable<StreamT, T>::value ||
                                  is_output_streamable<StreamT, T>::value>
{};

  #if defined(__cpp_variable_templates)  // C++14+

    #if defined(__cpp_inline_variables)  // C++17+

template<typename StreamT, typename T>
inline constexpr bool is_streamable_v = is_streamable<StreamT, T>::value;

    #else  // no inline variables (C++14-)

// TBD: find accepted idiom to work around lack of inline variables, see:
//   - https://stackoverflow.com/a/38043566
      #error "IsStreamable/is_streamable requires compilation as C++17 or above"

    #endif  // no inline variables

  #endif  // defined(__cpp_variable_templates)

#endif  // __cplusplus < 202002L


#endif  // ISSTREAMABLE_HH
