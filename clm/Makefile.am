#
# FAP3 command line monitor
#

bin_PROGRAMS = fapm3

AM_CPPFLAGS = -D_PLAT_LINUX_ -g -O0

fapm3_SOURCES = \
	       monitor.h monitor.cpp main.cpp

fapm3_LDFLAGS=-L../src/.libs/ -lfap3 -lrt
fapm3_CXXFLAGS=-I@srcdir@/../src -pg -pthread

