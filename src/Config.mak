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

HDF5INC = -I/Users/Franck/Softs/install/hdf5/include
HDF5LIB = -L/Users/Franck/Softs/install/hdf5/lib -lhdf5 -lhdf5_hl


#
# MPI includes may be needed
#

MPIINC = -I/usr/include

