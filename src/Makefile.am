includedir = @includedir@/fap3

AM_CPPFLAGS = -D_PLAT_LINUX_ -g -O0 -pthread

lib_LTLIBRARIES = libfap3.la

libfap3_la_SOURCES = \
		       miface.h \
		       mowning.h \
		       nconn.h \
		       menv.h \
		       munit.h \
		       ifu.h \
		       ifu.cpp \
		       curi.h \
		       curi.cpp \
		       ifr.h \
		       ifr.cpp \
		       cont.h \
		       cont.cpp \
		       cont2.h \
		       cont2.cpp \
		       vert.h \
		       vert.cpp \
		       syst.h \
		       syst.cpp \
		       unit.h \
		       unit.cpp

include_HEADERS = \
		  miface.h


libfap3_la_LDFLAGS = -rdynamic -no-undefined -lrt
