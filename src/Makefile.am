# Jerasure AM file
TVM_ROOT=/home/tvm
# DMLC_CORE=${TVM_ROOT}/3rdparty/dmlc-core

AM_CPPFLAGS = -I$(top_srcdir)/include\
              # -I${TVM_ROOT}/include\
              # -I${DMLC_CORE}/include\
              # -I${TVM_ROOT}/3rdparty/dlpack/include\
              # -DDMLC_USE_LOGGING_LIBRARY=\<tvm/runtime/logging.h\>
AM_CFLAGS = $(SIMD_FLAGS)
# AM_LDFLAGS = -L${TVM_ROOT}/build -ldl -pthread

lib_LTLIBRARIES = libJerasure.la
libJerasure_la_SOURCES = galois.c jerasure.c reed_sol.c cauchy.c liberation.c
# libJerasure_la_CFLAGS = -L${TVM_ROOT}/deploy/lib
libJerasure_la_LDFLAGS = -version-info 2:0:0
libJerasure_la_LIBADD = -ltvm_bitmatrix -lgf_complete
include_HEADERS = ../include/jerasure.h

# Install additional Jerasure header files in their own directory.
jerasureincludedir = $(includedir)/jerasure
jerasureinclude_HEADERS = \
  ../include/cauchy.h \
  ../include/galois.h \
  ../include/liberation.h \
  ../include/reed_sol.h \
  ../include/tvm_jerasure.h

noinst_HEADERS = ../include/timing.h
noinst_LIBRARIES = libtiming.a
libtiming_a_SOURCES = timing.c
