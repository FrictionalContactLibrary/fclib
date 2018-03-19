#
# Specify C compiler
#

CC = cc

# 
# Debug or optimized version switch (yes/no)
#

DEBUG = yes

#
# HDF5 library paths
#

HDF5INC = $(shell pkg-config --cflags hdf5)
HDF5LIB = $(shell pkg-config --libs hdf5) -lhdf5_hl

#
# MPI includes may be needed
#

MPIINC = -I/usr/include

#
# System or built-in SuiteSparse
#

SUITESPARSE_DIR = ../externals/SuiteSparse/CSparse
SUITESPARSE_OBJ = csparse.o
# or
SUITESPARSE_DIR = /usr/include/suitesparse
SUITESPARSE_OBJ =
SUITESPARSE_LIB = -lcxsparse

#
# fclib options
#
DEFS = -DFCLIB_HEADER_ONLY -DFCLIB_WITH_MERIT_FUNCTIONS
