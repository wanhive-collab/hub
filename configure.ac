#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([wanhive], [4.0.0], [info@wanhive.com],,[www.wanhive.com])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_SRCDIR([src/wanhive-reactor.h])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_MACRO_DIRS([m4])
PKG_INSTALLDIR

# Checks for programs.
AC_PROG_CXX([g++])
AC_PROG_CC([gcc])
## https://www.gnu.org/software/autoconf-archive/ax_cxx_compile_stdcxx.html
AX_CXX_COMPILE_STDCXX(14, [noext], [mandatory])

# Checks for libraries.
LT_INIT
AC_CHECK_LIB([crypto], [BN_clear_free],,[AC_MSG_ERROR([libcrypto required for cryptographic routines not found.])])
AC_CHECK_LIB([pthread], [pthread_create],,[AC_MSG_ERROR([libpthread required for posix thread not found.])])
AC_CHECK_LIB([sqlite3], [sqlite3_open_v2],,[AC_MSG_ERROR([libsqlite3 required for embedded database not found.])])
AC_CHECK_LIB([ssl], [OPENSSL_init_ssl],,[AC_MSG_ERROR([libssl required for secure socket not found.])])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h sys/file.h sys/time.h syslog.h unistd.h endian.h pthread.h openssl/ssl.h sqlite3.h sys/epoll.h],,
	[AC_MSG_ERROR([Required system headers not found.])])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_C_RESTRICT
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRERROR_R
AC_CHECK_FUNCS([clock_gettime getcwd localtime_r memmove memset mkdir select socket strcasecmp strchr strdup strrchr])

# Conditionally build the additional packages
AC_ARG_WITH([executables],
	[AS_HELP_STRING([--without-executables], [Disable building of executable programs.])])
AS_IF([test "x$with_executables" != "xno"],
	[AC_CHECK_LIB([pq], [PQfinish],,[AC_MSG_ERROR([libpq required for postgresql database not found.])])])
AS_IF([test "x$with_executables" != "xno"],
	[AC_CHECK_HEADERS([postgresql/libpq-fe.h],,[AC_MSG_ERROR([Required headers not found.])])])
AM_CONDITIONAL([WH_WITH_EXECUTABLES], [test "x$with_executables" != "xno"])

AC_CONFIG_FILES([Makefile data/Makefile docs/Makefile src/Makefile src/wanhive.pc])
AC_OUTPUT

