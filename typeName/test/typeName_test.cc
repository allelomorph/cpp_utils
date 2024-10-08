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
    SECTION("CV-unqualified")
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
    SECTION("CV-qualified")
    {
        SECTION("`const` comes before type")
        {
            REQUIRE(typeName<const int>() == "const int");
            REQUIRE(typeName<int const>() == "const int");
        }
        SECTION("`volatile` comes before type")
        {
            REQUIRE(typeName<volatile int>() == "volatile int");
            REQUIRE(typeName<int volatile>() == "volatile int");
        }
        SECTION("`const` comes before `volatile`")
        {
            REQUIRE(typeName<const volatile int>() == "const volatile int");
            REQUIRE(typeName<volatile const int>() == "const volatile int");
        }
    }
    SECTION("Pointers")
    {
        SECTION("Asterisk(s) at end of type token")
        {
            REQUIRE(typeName<int*>() == "int*");
            REQUIRE(typeName<int *>() == "int*");
            REQUIRE(typeName<int**>() == "int**");
            REQUIRE(typeName<int **>() == "int**");
        }
        SECTION("Const-qualifiers in [const] <type>* [const] order")
        {
            REQUIRE(typeName<const int*>() == "const int*");
            REQUIRE(typeName<int* const>() == "int* const");
            REQUIRE(typeName<const int* const>() == "const int* const");
        }
    }
    SECTION("References")
    {
        SECTION("Ampersand(s) at end of type token")
        {
            REQUIRE(typeName<int&>() == "int&");
            REQUIRE(typeName<int &>() == "int&");
            REQUIRE(typeName<int&&>() == "int&&");
            REQUIRE(typeName<int &&>() == "int&&");
        }
    }
}

TEST_CASE("Output of `decltype`",
          "[decltype]")
{
    SECTION("Local variables")
    {
        SECTION("C++ fundamental types")
        {
            int i {};
            const int ci {};

            REQUIRE(typeName<decltype(i)>() == "int");
            REQUIRE(typeName<decltype(ci)>() == "const int");

            SECTION("`decltype(())`")
            {
                REQUIRE(typeName<decltype((i))>() == "int&");
                REQUIRE(typeName<decltype((ci))>() == "const int&");
            }

            SECTION("after static_cast")
            {
                REQUIRE(typeName<
                        decltype(static_cast<int>(i))
                        >() == "int");
                REQUIRE(typeName<
                        decltype(static_cast<int&>(i))
                        >() == "int&");
                REQUIRE(typeName<
                        decltype(static_cast<int&&>(i))
                        >() == "int&&");
            }
        }
    }
}
