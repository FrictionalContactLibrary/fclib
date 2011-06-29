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

HDF5INC = -I/opt/local/include
HDF5LIB = -L/opt/local/lib -lhdf5 -lhdf5_hl

#
# MPI includes may be needed
#

MPIINC = -I/usr/include
