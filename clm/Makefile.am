#
# FAP3 command line monitor
#

bin_PROGRAMS = fapm

AM_CPPFLAGS = -D_PLAT_LINUX_ -g -O0

fapm_SOURCES = \
	       monitor.h monitor.cpp main.cpp

fapm_LDFLAGS=-L../src/.libs/ -lfap3 -lrt
fapm_CXXFLAGS=-I@srcdir@/../src -pg -pthread

