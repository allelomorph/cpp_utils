#include <catch2/catch_version_macros.hpp>               // CATCH_VERSION_MAJOR
#if (CATCH_VERSION_MAJOR != 3)
  #error "tests currently only support Catch2 v3.x"
#endif
#include <catch2/catch_test_macros.hpp>                  // TEST_CASE, SECTION, REQUIRE
#include <catch2/matchers/catch_matchers.hpp>            // REQUIRES_*THROW*
#include <catch2/matchers/catch_matchers_exception.hpp>  // Catch::Matchers::Message

#include "safeLibcCall.hh"


namespace {

enum class ErrorIndication { None, Return, Errno, ReturnAndErrno };

int _mockLibcFunc(const int test_retval, const int test_errno,
                  const ErrorIndication fail_type) {
    int retval {};
    switch (fail_type) {
    case ErrorIndication::Return:
        retval = test_retval;
        break;
    case ErrorIndication::Errno:
        errno = test_errno;
        break;
    case ErrorIndication::ReturnAndErrno:
        retval = test_retval;
        errno = test_errno;
        break;
    default:
        break;
    }
    return retval;
}

}  // namespace

using Catch::Matchers::Message;

TEST_CASE("Detection by return value or errno with LibcRetErrTest",
    "[LibcRetErrTest, retval, errno]")
{
    const LibcRetErrTest<int> ret_err_test { [](const int ret, const int err) {
        return (ret == -1 || err);
    } };

    SECTION("Success")
    {
        REQUIRE_NOTHROW(
            safeLibcCall(_mockLibcFunc, "_mockLibcFunc", ret_err_test,
                         0, 0, ErrorIndication::ReturnAndErrno)
            );
    }
    SECTION("Failure by return value")
    {
        SECTION("errno set")
        {
            REQUIRE_THROWS_MATCHES(
                safeLibcCall(_mockLibcFunc, "_mockLibcFunc", ret_err_test,
                             -1, EINVAL, ErrorIndication::ReturnAndErrno),
                std::system_error,
                Message("_mockLibcFunc: Invalid argument")
                );
        }

        SECTION("errno not set")
        {
            errno = 0;
            REQUIRE_THROWS_MATCHES(
                safeLibcCall(_mockLibcFunc, "_mockLibcFunc", ret_err_test,
                             -1, 0, ErrorIndication::Return),
                std::runtime_error,
                Message("_mockLibcFunc: failure without setting errno")
                );
        }

    }
    SECTION("Failure by errno")
    {
        REQUIRE_THROWS_MATCHES(
            safeLibcCall(_mockLibcFunc, "_mockLibcFunc", ret_err_test,
                         -1, EINVAL, ErrorIndication::ReturnAndErrno),
            std::system_error,
            Message("_mockLibcFunc: Invalid argument")
            );
    }
}

TEST_CASE("Detection by return value with LibcRetTest",
    "[LibcRetTest, retval]")
{
    const LibcRetTest<int> ret_test {
        [](const int ret) { return (ret == -1); }
    };

    SECTION("Success")
    {
        REQUIRE_NOTHROW(
            safeLibcCall(_mockLibcFunc, "_mockLibcFunc", ret_test,
                         0, 0, ErrorIndication::ReturnAndErrno)
            );
    }
    SECTION("Failure")
    {
        SECTION("errno set")
        {
            REQUIRE_THROWS_MATCHES(
                safeLibcCall(_mockLibcFunc, "_mockLibcFunc", ret_test,
                             -1, EINVAL, ErrorIndication::ReturnAndErrno),
                std::system_error,
                Message("_mockLibcFunc: Invalid argument")
                );
        }
        SECTION("errno not set")
        {
            errno = 0;
            REQUIRE_THROWS_MATCHES(
                safeLibcCall(_mockLibcFunc, "_mockLibcFunc", ret_test,
                             -1, 0, ErrorIndication::Return),
                std::runtime_error,
                Message("_mockLibcFunc: failure without setting errno")
                );
        }
    }
}

TEST_CASE("Detection by errno with LibcErrTest",
    "[LibcErrTest, errno]")
{
    const LibcErrTest err_test { [](const int err) {
        return (err);
    } };

    SECTION("Success")
    {
        REQUIRE_NOTHROW(
            safeLibcCall(_mockLibcFunc, "_mockLibcFunc", err_test,
                         0, 0, ErrorIndication::Errno)
            );
    }
    SECTION("Failure")
    {
        REQUIRE_THROWS_MATCHES(
            safeLibcCall(_mockLibcFunc, "_mockLibcFunc", err_test,
                         0, EINVAL, ErrorIndication::Errno),
            std::system_error,
            Message("_mockLibcFunc: Invalid argument")
            );
    }
}

TEST_CASE("Detection by any non-zero errno with no test functor",
    "[errno]")
{
    SECTION("Success")
    {
        REQUIRE_NOTHROW(
            safeLibcCall(_mockLibcFunc, "_mockLibcFunc",
                         0, 0, ErrorIndication::Errno)
            );
    }
    SECTION("Failure")
    {
        REQUIRE_THROWS_MATCHES(
            safeLibcCall(_mockLibcFunc, "_mockLibcFunc",
                         0, EINVAL, ErrorIndication::Errno),
            std::system_error,
            Message("_mockLibcFunc: Invalid argument")
            );
    }
}
