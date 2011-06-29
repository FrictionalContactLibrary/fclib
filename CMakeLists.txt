#=======================================================
# cmake utility to compile and install fclib library
#
#=======================================================

# ============= Global cmake Settings ============= 
# Set minimum version for cmake
cmake_minimum_required(VERSION 2.8)
# Set policy
cmake_policy(VERSION 2.8)
# Set cmake modules directory (i.e. the one which contains all user-defined FindXXX.cmake files among other things)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)
# Force out-of-source build
include(OutOfSourceBuild)
# Some usefull macros
include(MyTools)

# User defined options
option(VERBOSE_MODE "enable verbose mode for cmake exec. Default = on" ON)
option(USE_MPI "compile and link parmes with mpi when this mode is enable. Default = on." OFF)
option(BUILD_SHARED_LIBS "Enable dynamic library build, default = ON" ON)
option(WITH_TESTS "Enable testing. Default = on" ON)

# cmake project name
set(PROJECT_NAME fclib)
# --- Name for the package ---
# This name will be used to install Parmes (library, headers, ...) and when another lib or soft will need to search for Parmes.
set(PACKAGE_NAME "FCLib")
# --- Set a version number for the package ---
set(${PACKAGE_NAME}_version 1.0.0)
# --- The name (without extension) of the lib to be created ---
set(PROJECT_LIBRARY_NAME ${PROJECT_NAME})
# The list of all dirs containing sources to be compiled for the Parmes lib
# Any file in those dirs will be used to create libparmes
set(${PROJECT_LIBRARY_NAME}_SRCDIRS 
  src
  )
# Matching expr for files to be compiled. 
set(EXTS *.c)
# Matching expr for headers (install purpose)
set(EXTS_HDRS *.hpp *.h)

# ============= The project =============
# Set project name and project languages 
# => this automatically defines:
#   - ${PROJECT_NAME}_BINARY_DIR : where you have run cmake, i.e. the place for compilation
#   - ${PROJECT_NAME}_SOURCE_DIR : where sources (.f and .h and this CMakeLists.txt) are located
# Note that because of OutOfSourceBuild, binary_dir and source_dir must be different. 
project(${PROJECT_NAME} C)

# ============= Tests =============
if(WITH_TESTS)
  enable_testing()
endif(WITH_TESTS)

# ============= Search for libraries  =============
# set the compile/link conf (-I and -L opt)

# --- Boost ---
find_package(HDF5 REQUIRED)
include_directories(${HDF5_INCLUDE_DIRS})
set(LIBS ${LIBS} ${HDF5_LIBRARIES})

# --- MPI ---
if(USE_MPI)
  # Find MPI for C++ and fortran.
  find_package(MPI REQUIRED)
  if(MPI_COMPILER)
    set(CMAKE_CXX_COMPILER mpic++)
  elseif(MPI_COMPILER)
    # -I
    include_directories(${MPI_INCLUDE_PATH})
    # Add compilation flags
    append_c_flags(${MPI_COMPILE_FLAGS})
    set(${PROJECT_NAME}_LINK_FLAGS ${${PROJECT_NAME}_LINK_FLAGS} ${MPI_LINK_FLAGS})
  endif(MPI_COMPILER)
  set(LIBS ${LIBS} ${MPI_LIBRARIES} )
endif(USE_MPI)

# ============= Prepare compilation =============
# Force a default build type if not provided by user
# CMAKE_BUILD_TYPE = empty, Debug, Release, RelWithDebInfo or MinSizeRel.
if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE RELEASE CACHE STRING "Choose the type of build, options are: None, Debug, Release, RelWithDebInfo or MinSizeRel." FORCE)  
endif (NOT CMAKE_BUILD_TYPE)

# ============= Source and header files list =============
# We scan all files with matching extension in directories 
# containing sources.
# Source and header files list:
foreach(_DIR ${${PROJECT_LIBRARY_NAME}_SRCDIRS})
  set(_DIR_FILES)
  foreach(_EXT ${EXTS}) # Source files
    file(GLOB _DIR_FILES_EXT ${_DIR}/${_EXT})
    if(_DIR_FILES_EXT)
      list(APPEND ${PROJECT_LIBRARY_NAME}_SRC ${_DIR_FILES_EXT})
    endif()
  endforeach()
  foreach(_EXT ${EXTS_HDRS}) # Headers
    file(GLOB _DIR_FILES_EXT ${_DIR}/${_EXT})
    if(_DIR_FILES_EXT)
      list(APPEND ${PROJECT_LIBRARY_NAME}_HDRS ${_DIR_FILES_EXT})
    endif()
  endforeach()
endforeach()
# We add headers to source files
list(APPEND ${PROJECT_LIBRARY_NAME}_SRC ${${PROJECT_LIBRARY_NAME}_HDRS})

# Add directories to those searched by compiler ...
# -I
include_directories(${${PROJECT_LIBRARY_NAME}_SRCDIRS})

# ============= Creates the library =============
if(BUILD_SHARED_LIBS) # shared library
  add_library(${PROJECT_LIBRARY_NAME} SHARED ${${PROJECT_LIBRARY_NAME}_SRC})
else() # static library
  add_library(${PROJECT_LIBRARY_NAME} STATIC ${${PROJECT__LIBRARY_NAME}_SRC})
endif()
# Libs to link with PROJECT__LIBRARY_NAME
target_link_libraries(${PROJECT_LIBRARY_NAME} ${LIBS})

# ============== Add tests ==============
if(WITH_TESTS)
  message(STATUS "Enable testing ...")
  begin_test(src/tests/)
  new_test(test fctst.c)
  end_test()
endif(WITH_TESTS)

# ============= Prepare install =============

# The library
# The library, the headers and mod files, the cmake generated files
# will be install in CMAKE_INSTALL_PREFIX/lib include and share
include(InstallPackage)

install_package(${PACKAGE_NAME} ${PROJECT_LIBRARY_NAME} ${${PROJECT_NAME}_HDRS})

# ============= RPATH =============
# Concerning rpath see for example http://www.itk.org/Wiki/CMake_RPATH_handling

# --------------------------------------------
# do not skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH  FALSE)
# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
# the RPATH to be used when installing
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# ============= Summary =============
if(VERBOSE_MODE)
  message(STATUS "====================== Summary ======================")
  message(STATUS " Compiler : ${CMAKE_C_COMPILER}")
  message(STATUS " Sources are in : ${CMAKE_SOURCE_DIR}")
  message(STATUS " Project uses MPI : ${USE_MPI}")
  message(STATUS " Project uses HDF5 : ${HDF5_LIBRARIES}")
  message(STATUS " Project will be installed in ${CMAKE_INSTALL_PREFIX}")
  message(STATUS "====================== ======= ======================")
endif()
