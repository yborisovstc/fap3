bin_PROGRAMS = ut-fap3-lib

AM_CPPFLAGS = -D_PLAT_LINUX_

ut_fap3_lib_SOURCES = \
		       ut_fap3_main.cpp \
		       ut_node.cpp \
		       ut_unit.cpp \
		       ut_elem.cpp \
		       ut_syst.cpp \
		       ut_ifr.cpp \
		       ut_chromo2.cpp \
		       ut_des.cpp \
		       ut_adp.cpp \
		       ut_dessdc.cpp \
		       ut_desctx.cpp \
		       ut_desas.cpp \
		       ut_deslsc.cpp \
		       ut_nn.cpp

ut_fap3_lib_LDFLAGS=-L../src/.libs/ -lfap3  -lcppunit -lrt
ut_fap3_lib_CXXFLAGS=-I@srcdir@/../src

check: test_fap3_lib

test_fap3_lib:
	 ./ut-fap3-lib


