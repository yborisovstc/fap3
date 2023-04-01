
AC_INIT([fap3], [0.1], [yuri.borisov.v@gmail.com])

AM_INIT_AUTOMAKE([-Wall])

AC_CONFIG_FILES([
		 Makefile
		 src/Makefile
		 test/Makefile
		 clm/Makefile
		 ])


AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL

# Configure options: --enable-debug[=no].
AC_ARG_ENABLE([debug],
	      [AS_HELP_STRING([--enable-debug],
			      [enable debug code (default is no)])],
			      [debug="$withval"], [debug=no])


PKG_CHECK_MODULES([LIBCPPU], [cppunit >= 1.13], , [AC_MSG_ERROR([
You must have the libcppunit-1.13 and development headers installed to build.
])])

AC_SUBST(LIBCPPU_CFLAGS)
AC_SUBST(LIBCPPU_LIBS)

CXXFLAGS="-std=c++11 -DPROFILING_ENABLED"

if test x$debug = xyes; then
    CXXFLAGS="$CXXFLAGS -g -O0"
#    LDFLAGS="$LDFLAGS -lprofiler -gp"
else
    CXXFLAGS="$CXXFLAGS -g -O0"
#    LDFLAGS="$LDFLAGS -lprofiler -gp"
fi

LDFLAGS="$LDFLAGS -lpthread"

CXXFLAGS="$CXXFLAGS $LIBCPPU_CFLAGS"
LDFLAGS="$LDFLAGS $LIBCPPU_LIBS"

AC_CONFIG_FILES([libfap3.pc])

AC_OUTPUT

echo CXXFLAGS $CXXFLAGS
echo "
($PACKAGE_NAME) version $PACKAGE_VERSION
Prefix.........: $prefix
Debug Build....: $debug
C++ Compiler...: $CXX $CXXFLAGS $CPPFLAGS
Linker.........: $LD $LDFLAGS $LIBS
Doxygen........: ${DOXYGEN:-NONE}
"

