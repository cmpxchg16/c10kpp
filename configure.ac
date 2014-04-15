#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([c10kpp], [0.01])
AC_CONFIG_SRCDIR([asio/header.hpp])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([subdir-objects foreign])
AM_MAINTAINER_MODE([enable])

# Checks for programs.
AC_LANG([C++])
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([boost_system], [_init], [ LIBS="$LIBS -lboost_system" ],
		[ AC_ERROR([Cannot build - cannot link with boost]) ])
AC_CHECK_LIB([boost_thread], [_init], [ LIBS="$LIBS -lboost_thread" ],
		[ AC_ERROR([Cannot build - cannot link with boost]) ])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h netdb.h netinet/in.h stdlib.h string.h sys/socket.h unistd.h])
AC_CHECK_HEADER([boost/bind.hpp],[],[ AC_ERROR([Cannot find boost headers]) ])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([memset socket strstr])

dnl Test program to make sure compiler supports all relevant C++11 features
[
testprogheaders="`cat <<"EOP"
#include <atomic>
#include <stdexcept>
#include <mutex>

class a : public std::runtime_error {
public:
    explicit a() : std::runtime_error("a")
    {}
};

EOP`"
testprog="`cat <<"EOP"
std::mutex mutex;
EOP`"
]
AC_MSG_CHECKING([what flags are needed for compiler support of the ISO C++11 standard])
saved_CXXFLAGS=$CXXFLAGS
CXXFLAGS="$saved_CXXFLAGS -Werror"
AC_COMPILE_IFELSE( [AC_LANG_PROGRAM([
	    $testprogheaders
        ],
        [
            $testprog
        ])],
    [
        AC_MSG_RESULT([none])
        CXXFLAGS="$saved_CXXFLAGS"
    ],
    [
        CXXFLAGS="$saved_CXXFLAGS -std=c++11 -Werror"
        AC_COMPILE_IFELSE( [AC_LANG_PROGRAM([
		    $testprogheaders
                ],
                [
                    $testprog
                ])],
            [
                AC_MSG_RESULT([-std=c++11])
                CXXFLAGS="$saved_CXXFLAGS -std=c++11"
            ],
            [
                AC_MSG_RESULT([not supported])
                AC_ERROR([We need a compiler that supports C++11. If you are using gcc, that
                  means version 4.7 or higher.])
            ]
        )
    ]
)

AC_MSG_CHECKING([which wakeup mechanism to use])
AC_ARG_ENABLE( [pipe-ipc],
	AC_HELP_STRING(
	    [--enable-pipe-ipc],
	    [Use pipe IPC (instead of lock based shared memory) to activate worker threads]),
	[ AC_MSG_RESULT([pipes]) ],
	[
	    AC_DEFINE([PTHREAD_IPC], [1], [Use shared memory and a lock to wake up worker threads])
	    AC_MSG_RESULT([pthread condition])
       	] )

AC_MSG_CHECKING([whether to set thread affinity])
AC_ARG_ENABLE( [affinity],
	AC_HELP_STRING(
	    [--disable-affinity],
	    [Disable thread to CPU affinity]),
	[
	    AC_MSG_RESULT([$enableval])
	    AFFINITY="$enableval"
	],
	[
	    AC_MSG_RESULT([yes])
	    AFFINITY="yes"
       	] )
[
if test "yes" = "$AFFINITY"
then
]
    AC_DEFINE([USE_AFFINITY], [1], [Set strong affinity between worker theread and CPU])
[
fi
]

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
