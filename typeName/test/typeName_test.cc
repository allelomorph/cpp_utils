#if (_CATCH_VERSION_MAJOR == 3)
  //#include <catch2/catch_version_macros.hpp>  // CATCH_VERSION_MAJOR
  #include <catch2/catch_test_macros.hpp>     // TEST_CASE, SECTION, REQUIRE
#elif (_CATCH_VERSION_MAJOR == 2)
  #include <catch2/catch.hpp>
#endif

#include "typeName.hh"

TEST_CASE("C++ fundamental types",
          "[builtin]")
{
    SECTION("No modifiers")
    {
        SECTION("Integrals")
        {
            SECTION("Single keyword")
            {
                REQUIRE(typeName<char>() == "char");
                REQUIRE(typeName<int>() == "int");
            }
            SECTION("No naked `short` and `long`")
            {
                REQUIRE(typeName<short>() == "short int");
                REQUIRE(typeName<long>() == "long int");
            }
            SECTION("`signed` and `signed int` treated as `int`")
            {
                REQUIRE(typeName<signed>() == "int");
                REQUIRE(typeName<signed int>() == "int");
            }
            SECTION("No naked `unsigned`")
            {
                REQUIRE(typeName<unsigned>() == "unsigned int");
            }
            SECTION("Size modifers before `unsigned`")
            {
                REQUIRE(typeName<unsigned long int>() == "long unsigned int");
                REQUIRE(typeName<long unsigned int>() == "long unsigned int");
            }
        }
        SECTION("Booleans")
        {
            REQUIRE(typeName<bool>() == "bool");
        }
        SECTION("Characters")
        {
            SECTION("Single keyword")
            {
                REQUIRE(typeName<char>() == "char");
                REQUIRE(typeName<wchar_t>() == "wchar_t");
            }
            SECTION("`signed` not ignored even if char is signed by default")
            {
                REQUIRE(typeName<signed char>() == "signed char");
            }
        }
        SECTION("Floating point")
        {
            REQUIRE(typeName<float>() == "float");
            REQUIRE(typeName<double>() == "double");
            REQUIRE(typeName<long double>() == "long double");
        }
    }
}

/*
// not sure why undefined function prototypes can be called below...
int& foo_lref();
int&& foo_rref();
int foo_value();

int main() {
    int i {};
    const int ci {};

    std::cout << "decltype(i) is " << typeName<decltype(i)>() << '\n';
    std::cout << "decltype((i)) is " << typeName<decltype((i))>() << '\n';
    std::cout << "decltype(ci) is " << typeName<decltype(ci)>() << '\n';
    std::cout << "decltype((ci)) is " << typeName<decltype((ci))>() << '\n';
    std::cout << "decltype(static_cast<int&>(i)) is " << typeName<decltype(static_cast<int&>(i))>() << '\n';
    std::cout << "decltype(static_cast<int&&>(i)) is " << typeName<decltype(static_cast<int&&>(i))>() << '\n';
    std::cout << "decltype(static_cast<int>(i)) is " << typeName<decltype(static_cast<int>(i))>() << '\n';
    std::cout << "decltype(foo_lref()) is " << typeName<decltype(foo_lref())>() << '\n';
    std::cout << "decltype(foo_rref()) is " << typeName<decltype(foo_rref())>() << '\n';
    std::cout << "decltype(foo_value()) is " << typeName<decltype(foo_value())>() << '\n';
}
*/
