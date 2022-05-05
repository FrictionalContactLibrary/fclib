# =================================================================
# cmake configuration to compile and install fclib library
# =================================================================

#
# ------ Global cmake Settings -------
#
# Set minimum version for cmake
# We advise the most recent version of cmake.
# Use python3 -m pip install cmake to get it.
cmake_minimum_required(VERSION 3.14)

# policy https://cmake.org/cmake/help/git-stage/policy/CMP0074.html
cmake_policy(SET CMP0074 NEW)

# Set cmake modules directory (i.e. the one which contains all
# user-defined FindXXX.cmake files among other things)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Force out-of-source build
if ("${CMAKE_SOURCE_DIR}" MATCHES "${CMAKE_BINARY_DIR}")
  message (SEND_ERROR "In source building not supported (not recommanded indeed). You need to :
    * cleanup your source directory :  \"rm -rf ./CMakeFiles/ ./CMakeCache.txt\"
    * try configure process again in a new directory
    (e.g. mkdir <anywhere>/build ; cd <anywhere>/build ; cmake ${CMAKE_SOURCE_DIR}) ...")
  return()
endif()

# If not given, turn build type to release mode.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None Debug Release." FORCE)
endif()

# ------- FCLib setup -------

# User defined options
option(FCLIB_WITH_MERIT_FUNCTIONS "enable merit functions. Default = ON" OFF)
option(FCLIB_HEADER_ONLY "static interface. Default = ON" OFF)
option(VERBOSE_MODE "enable verbose mode for cmake exec. Default = ON" ON)
option(USE_MPI "compile and link fclib with mpi when this mode is enable. Default = ON" OFF)
option(BUILD_SHARED_LIBS "Enable dynamic library build, default = ON" ON)
option(WITH_TESTS "Enable testing. Default = ON" ON)
option(FORCE_SKIP_RPATH "Do not build shared libraries with rpath. Useful only for packaging. Default = OFF" OFF)
option(USE_SYSTEM_SUITESPARSE "Use the system-installed SuiteSparse library for CXSparse if one is found." ON)
option(SKIP_PKGCONFIG "Do not configure or install the pkg-config file." OFF)
option(HARDCODE_NOT_HEADER_ONLY "Pre-define as 'not header-only' in the installed header." OFF)

set(WARNINGS_LEVEL 0 CACHE INTERNAL "Set compiler diagnostics level. 0: no warnings, 1: developer's minimal warnings, 2: strict level, warnings to errors and so on. Default =0")

if(HARDCODE_NOT_HEADER_ONLY AND FCLIB_HEADER_ONLY)
  message(FATAL_ERROR
    "HARDCODE_NOT_HEADER_ONLY=${HARDCODE_NOT_HEADER_ONLY} "
    "and FCLIB_HEADER_ONLY=${FCLIB_HEADER_ONLY} are inconsistent.")
endif()

if(FCLIB_WITH_MERIT_FUNCTIONS AND FCLIB_HEADER_ONLY)
  message(FATAL_ERROR
    "FCLIB_WITH_MERIT_FUNCTIONS=${FCLIB_WITH_MERIT_FUNCTIONS} "
    "and FCLIB_HEADER_ONLY=${FCLIB_HEADER_ONLY} are inconsistent.")
endif()

if(HARDCODE_NOT_HEADER_ONLY)
  set(OPTDEFS FCLIB_NOT_HEADER_ONLY)
  if(FCLIB_WITH_MERIT_FUNCTIONS)
    list(APPEND OPTDEFS FCLIB_WITH_MERIT_FUNCTIONS)
  endif()
  set(DEFS)
  foreach(_D IN LISTS OPTDEFS)
    set(DEFS "${DEFS}\\n#ifndef ${_D}\\n#define ${_D}\\n#endif\\n")
  endforeach()
  add_custom_target(fclib_h
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/src/fclib.h
      | sed 's,/\\*@ CONFIG @\\*/,${DEFS},'
      | sed '/@@/,/@@/d'
      | sed 's/FCLIB_STATIC //' >fclib.h
    BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/fclib.h
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/fclib.h
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Customising fclib.h")
	set(EXTRA_TARGETS fclib_h)
endif()


# cmake project name
set(PROJECT_NAME fclib)


# ============= Project setup =============
if(WITH_CXX)
  project(${PROJECT_NAME} CXX C)
  set(CMAKE_CXX_STANDARD 17)
  set(fclib_language CXX)
else()
  project(${PROJECT_NAME} C)
  set(fclib_language C)
endif()
set(CMAKE_C_STANDARD 11)
include(FClibVersion)
include(fclib_setup)

if(FCLIB_HEADER_ONLY)
  add_library(fclib INTERFACE)
  set(LIB_SCOPE INTERFACE)
  target_compile_definitions(fclib INTERFACE FCLIB_HEADER_ONLY)
else()
  # --- Build fclib as a library ---
  message("Build fclib library ...")
  if(BUILD_SHARED_LIBS) # shared library
    add_library(fclib SHARED src/fclib.c)
  else() # static library
    add_library(fclib STATIC src/fclib.c)
  endif()
  set(LIB_SCOPE PUBLIC)

  if(FCLIB_WITH_MERIT_FUNCTIONS)
    target_compile_definitions(fclib PUBLIC FCLIB_WITH_MERIT_FUNCTIONS)
  endif()
  set_source_files_properties(src/fclib.c
    PROPERTIES LANGUAGE ${fclib_language})

  set_target_properties(fclib PROPERTIES
    OUTPUT_NAME "fclib"
    VERSION "${${PACKAGE_NAME}_SOVERSION}"
    SOVERSION  "${${PACKAGE_NAME}_SOVERSION_MAJOR}"
    LINKER_LANGUAGE ${fclib_language})
  
  # MSVC extra conf
  if(MSVC)
    option(USING_HDF5_DLLs "WIN ONLY: turn on if the shared library of HDSL is linked" ON)
    if(USING_HDF5_DLLs)
      target_compile_definitions(fclib PRIVATE H5_BUILT_AS_DYNAMIC_LIB)
    endif()
    target_compile_definitions(fclib PRIVATE "FCLIB_APICOMPILE=__declspec( dllexport )")
    target_compile_definitions(fclib PRIVATE _CRT_SECURE_NO_WARNINGS)
  endif()

  # - SuiteSparse -
  if(FCLIB_WITH_MERIT_FUNCTIONS)
    find_package(SuiteSparse REQUIRED COMPONENTS CXSparse)
    target_link_libraries(${PROJECT_NAME} PRIVATE SuiteSparse::CXSparse)
  endif()


endif()



  # --- dependencies ---

   
  # - hdf5 -
  set(HDF5_PREFER_PARALLEL TRUE)
  find_package(HDF5  COMPONENTS C HL REQUIRED)
  if(HDF5_IS_PARALLEL)
    set(USE_MPI ON)
  endif()

  if(${CMAKE_VERSION} VERSION_LESS "3.19")
    target_include_directories(${PROJECT_NAME} ${LIB_SCOPE} ${HDF5_C_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${LIB_SCOPE} ${HDF5_C_LIBRARIES})
    target_link_libraries(${PROJECT_NAME} ${LIB_SCOPE} ${HDF5_HL_LIBRARIES})
  else()
    target_link_libraries(${PROJECT_NAME} ${LIB_SCOPE} hdf5::hdf5 hdf5::hdf5_hl) 
  endif()
  # - mpi -
  if(USE_MPI)
      find_package(MPI REQUIRED)
      target_link_libraries(${PROJECT_NAME} PRIVATE MPI::MPI_C)
  endif()

  # 
  # --- install lib --
  install(TARGETS fclib
    EXPORT fclibTargets
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION include
    )
 
# --- Install headers ---
if(HARDCODE_NOT_HEADER_ONLY)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/fclib.h DESTINATION include)
else()
  install(FILES src/fclib.h DESTINATION include) 
endif()
target_include_directories(fclib INTERFACE $<INSTALL_INTERFACE:include>)
target_include_directories(fclib PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)
if(EXTRA_TARGETS)
  add_dependencies(fclib ${EXTRA_TARGETS})
endif()

# ============= Doc and website =============
if(WITH_DOCUMENTATION)
  include(FCLibDoc)
endif()


# ============= Fclib Package configuration =============
# i.e. what should be generated/configured at install.
include(FClibPackageSetup)

#  ============= Tests =============
if(WITH_TESTS)
  enable_testing()
  add_executable(fctest1 src/tests/fctst.c)
  add_test(fctest1 fctest1)
  target_link_libraries(fctest1 PRIVATE fclib)
  if(USE_MPI)
    target_link_libraries(fctest1 PRIVATE MPI::MPI_C)
  endif()

  if(FORCE_SKIP_RPATH)
    set_tests_properties(fctest1 PROPERTIES ENVIRONMENT LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR})
  endif()

  if(FCLIB_WITH_MERIT_FUNCTIONS)
    target_link_libraries(fctest1 PRIVATE SuiteSparse::CXSparse)
    add_executable(fctest_merit src/tests/fctst_merit.c)
    add_test(fctest_merit fctest_merit)
    target_link_libraries(fctest_merit PRIVATE fclib)
    target_include_directories(fctest_merit PRIVATE src)
    target_link_libraries(fctest_merit PRIVATE SuiteSparse::CXSparse)
    if(USE_MPI)
      target_link_libraries(fctest_merit PRIVATE MPI::MPI_C)
     endif()
    if(FORCE_SKIP_RPATH)
      set_tests_properties(fctest_merit PROPERTIES ENVIRONMENT LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR})
    endif()
    configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/src/tests/data/local_problem_test.hdf5
      ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
  endif()
endif()


message(STATUS "====================== Summary ======================")
message(STATUS " Compiler : ${CMAKE_C_COMPILER}")
message(STATUS " Sources are in : ${CMAKE_SOURCE_DIR}")
message(STATUS " Project uses MPI : ${USE_MPI}")
message(STATUS " Project uses HDF5 : ${HDF5_LIBRARIES}")
message(STATUS " Project will be installed in ${CMAKE_INSTALL_PREFIX}")
message(STATUS "====================== ======= ======================")
