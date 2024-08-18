#if (_CATCH_VERSION_MAJOR == 3)
  #include <catch2/catch_version_macros.hpp>               // CATCH_VERSION_MAJOR
  #include <catch2/catch_test_macros.hpp>                  // TEST_CASE, SECTION, REQUIRE
  #include <catch2/matchers/catch_matchers.hpp>            // REQUIRES_*THROW*
  #include <catch2/matchers/catch_matchers_exception.hpp>  // Catch::Matchers::Message
#elif (_CATCH_VERSION_MAJOR == 2)
  #include <catch2/catch.hpp>
#endif

#include "safeLibcCall.hh"

#include <fcntl.h>   // open
#include <unistd.h>  // close, unlink


#if (CATCH_VERSION_MAJOR > 2)
using Catch::Matchers::Message;
#else
using Catch::Message;
#endif

#define _TFNAME "safeLibcCall_testfile"

TEST_CASE("Detection by return value or errno with LibcRetErrTest",
    "[LibcRetErrTest, retval, errno]")
{
    const LibcRetErrTest<int> open_test { [](const int ret, const int err) {
        return (ret == -1 || err);
    } };

    const LibcRetErrTest<int> open_test_inverse_ret { [](const int ret, const int err) {
        return (ret != -1 || err);
    } };

    SECTION("Success")
    {
        int fd;
        REQUIRE_NOTHROW(
            fd = safeLibcCall(open, "open", open_test,
                              _TFNAME, O_RDONLY | O_CREAT)
            );
        close(fd);
        unlink(_TFNAME);
    }
    SECTION("Failure by return value")
    {
        SECTION("errno set")
        {
            REQUIRE_THROWS_MATCHES(
                safeLibcCall(open, "open", open_test,
                             "", O_RDONLY),
                std::system_error,
                Message("open: No such file or directory")
                );
        }
        SECTION("errno not set")
        {
            // TBD: find glibc func that fails only by retval
            int fd;
            REQUIRE_THROWS_MATCHES(
                fd = safeLibcCall(open, "open", open_test_inverse_ret,
                                  _TFNAME, O_RDONLY | O_CREAT),
                std::runtime_error,
                Message("open: failure without setting errno")
                );
            close(fd);
            unlink(_TFNAME);
        }
    }
    SECTION("Failure by errno")
    {
        // TBD: find glibc func that fails only by errno
        REQUIRE_THROWS_MATCHES(
            safeLibcCall(open, "open", open_test_inverse_ret,
                         "", O_RDONLY),
            std::system_error,
            Message("open: No such file or directory")
            );
    }
}

TEST_CASE("Detection by return value with LibcRetTest",
    "[LibcRetTest, retval]")
{
    const LibcRetTest<int> open_test {
        [](const int ret) { return (ret == -1); }
    };

    SECTION("Success")
    {
        int fd;
        REQUIRE_NOTHROW(
            fd = safeLibcCall(open, "open", open_test,
                              _TFNAME, O_RDONLY | O_CREAT)
            );
        close(fd);
        unlink(_TFNAME);
    }
    SECTION("Failure")
    {
        SECTION("errno set")
        {
            REQUIRE_THROWS_MATCHES(
                safeLibcCall(open, "open", open_test,
                             "", O_RDONLY),
                std::system_error,
                Message("open: No such file or directory")
                );
        }
        SECTION("errno not set")
        {
            const LibcRetTest<int> open_test_inverse {
                [](const int ret) { return (ret != -1); }
            };

            int fd;
            REQUIRE_THROWS_MATCHES(
                fd = safeLibcCall(open, "open", open_test_inverse,
                                  _TFNAME, O_RDONLY | O_CREAT),
                std::runtime_error,
                Message("open: failure without setting errno")
                );
            close(fd);
            unlink(_TFNAME);
        }
    }
}

TEST_CASE("Detection by errno with LibcErrTest",
    "[LibcErrTest, errno]")
{
    const LibcErrTest open_test { [](const int err) {
        return (err);
    } };

    SECTION("Success")
    {
        int fd;
        REQUIRE_NOTHROW(
            fd = safeLibcCall(open, "open", open_test,
                              _TFNAME, O_RDONLY | O_CREAT)
            );

        close(fd);
        unlink(_TFNAME);
    }
    SECTION("Failure")
    {
        REQUIRE_THROWS_MATCHES(
            safeLibcCall(open, "open", open_test,
                         "", O_RDONLY),
            std::system_error,
            Message("open: No such file or directory")
            );
    }
}

TEST_CASE("Detection by any non-zero errno with no test functor",
    "[errno]")
{
    SECTION("Success")
    {
        int fd;
        REQUIRE_NOTHROW(
            fd = safeLibcCall(open, "open",
                              _TFNAME, O_RDONLY | O_CREAT)
            );
        close(fd);
        unlink(_TFNAME);
    }
    SECTION("Failure")
    {
        REQUIRE_THROWS_MATCHES(
            safeLibcCall(open, "open",
                         "", O_RDONLY),
            std::system_error,
            Message("open: No such file or directory")
            );
    }
}
