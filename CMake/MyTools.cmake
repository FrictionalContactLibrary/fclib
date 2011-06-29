include(LibFindMacros)

# Basic list manipulation
macro(CAR var)
  set(${var} ${ARGV1})
endmacro(CAR)

macro(CDR var junk)
  set(${var} ${ARGN})
endmacro(CDR)

# LIST(APPEND ...) is not correct on <COMPILER>_FLAGS 
macro(APPEND_FLAGS)
  CAR(_V ${ARGV})
  CDR(_F ${ARGV})
  set(${_V} "${${_V}} ${_F}")
endmacro(APPEND_FLAGS)


# Add all dirs in _DIRS into the searched directories for include files
# Set, for dir in _DIRS:
# - ${PROJECT_NAME}_INCLUDE_DIRECTORIES 
# - ${PROJECT_NAME}_REMEMBER_INC_${dir} (to avoid multiple sets)
macro(REMEMBER_INCLUDE_DIRECTORIES _DIRS)
  foreach(_D ${_DIRS})
    if(NOT ${PROJECT_NAME}_REMEMBER_INC_${_D})
      set(${PROJECT_NAME}_REMEMBER_INC_${_D} TRUE)
      list(APPEND ${PROJECT_NAME}_INCLUDE_DIRECTORIES ${_D})
      include_directories(${_DIRS})
    endif(NOT ${PROJECT_NAME}_REMEMBER_INC_${_D})
  endforeach(_D ${_DIRS})
endmacro(REMEMBER_INCLUDE_DIRECTORIES _DIRS)

# Add all dirs in _DIRS into the searched directories for linked libraries
# Set, for dir in _DIRS:
# - ${PROJECT_NAME}_LINK_DIRECTORIES 
# - ${PROJECT_NAME}_REMEMBER_LINK_${dir} (to avoid multiple sets)
macro(REMEMBER_LINK_DIRECTORIES _DIRS)
  foreach(_D ${_DIRS})
    if(NOT ${PROJECT_NAME}_REMEMBER_LINK_${_D})
      set(${PROJECT_NAME}_REMEMBER_LINK_${_D} TRUE)
      list(APPEND ${PROJECT_NAME}_LINK_DIRECTORIES ${_D})
      link_directories(${_D})
    endif(NOT ${PROJECT_NAME}_REMEMBER_LINK_${_D})
  endforeach(_D ${_DIRS})
endmacro(REMEMBER_LINK_DIRECTORIES _DIRS)

# Add all libraries in _LIBS into the searched directories for linked libraries
# Set, for lib in _LIBS:
# - ${PROJECT_NAME}_LINK_LIBRARIES 
# - ${PROJECT_NAME}_REMEMBER_LINK_LIBRARIES_${lib} (to avoid multiple sets)
macro(REMEMBER_LINK_LIBRARIES _LIBS)
  foreach(_LIB ${_LIBS})
    if(NOT ${PROJECT_NAME}_REMEMBER_LINK_LIBRARIES_${_LIB})
      set(${PROJECT_NAME}_REMEMBER_LINK_LIBRARIES_${_LIB} TRUE)
      list(APPEND ${PROJECT_NAME}_LINK_LIBRARIES ${_LIB})
    endif(NOT ${PROJECT_NAME}_REMEMBER_LINK_LIBRARIES_${_LIB})
  endforeach(_LIB ${_LIBS})
endmacro(REMEMBER_LINK_LIBRARIES _LIBS)

# The use of ADD_DEFINITION results in a warning with Fortran compiler
macro(APPEND_C_FLAGS)
  APPEND_FLAGS(CMAKE_C_FLAGS ${ARGV})
endmacro(APPEND_C_FLAGS)

macro(APPEND_CXX_FLAGS)
  APPEND_FLAGS(CMAKE_CXX_FLAGS ${ARGV})
endmacro(APPEND_CXX_FLAGS)

macro(APPEND_Fortran_FLAGS)
  APPEND_FLAGS(CMAKE_Fortran_FLAGS ${ARGV})
endmacro(APPEND_Fortran_FLAGS)

# debug
macro(display V)
  message(STATUS "${V} = ${${V}}")
endmacro(display V)

# Tests
macro(begin_test _D)
  set(_CURRENT_TEST_DIRECTORY ${_D})
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${_D})
  
  # create a CMakeLists.txt for the current test in build_dir/test-name
  # Source = file CMakeListsForTests.cmake. 
  configure_file(${CMAKE_SOURCE_DIR}/CMake/CMakeListsForTests.cmake 
    ${CMAKE_CURRENT_BINARY_DIR}/${_CURRENT_TEST_DIRECTORY}/CMakeLists.txt @ONLY)
 
  # init the list of test executables
  set(_EXE_LIST_${_CURRENT_TEST_DIRECTORY})
endmacro(begin_test _D)

macro(end_test)
  # add the subdir corresponding to the current test (i.e. take the new CMakeLists.txt into account)
  add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/${_CURRENT_TEST_DIRECTORY} ${CMAKE_CURRENT_BINARY_DIR}/${_CURRENT_TEST_DIRECTORY})
endmacro(end_test)


# Declare a new test
macro(new_test)
  car(_EXE ${ARGV})
  cdr(_SOURCES ${ARGV})
  # Add the current test to the list of all executables
  list(APPEND _EXE_LIST_${_CURRENT_TEST_DIRECTORY} ${_EXE})
  # Get the list of sources for the current executable
  set(${_EXE}_FSOURCES)
  foreach(_F ${_SOURCES})
    list(APPEND ${_EXE}_FSOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${_CURRENT_TEST_DIRECTORY}/${_F})
  endforeach(_F ${_SOURCES})
endmacro(new_test)  
