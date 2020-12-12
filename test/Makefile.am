bin_PROGRAMS = ut-fap3-lib

AM_CPPFLAGS = -D_PLAT_LINUX_ -g -O0

ut_fap3_lib_SOURCES = \
		       ut_fap3_main.cpp \
		       ut_nn.cpp

ut_fap3_lib_LDFLAGS=-L../src/.libs/ -lfap3  -lcppunit -lrt
ut_fap3_lib_CXXFLAGS=-I@srcdir@/../src

check: test_fap3_lib

test_fap3_lib:
	 ./ut-fap3-lib


