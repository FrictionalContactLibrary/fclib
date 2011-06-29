#
# Use this to force out-of source build.
#
option(IN_SOURCE_BUILD "Switch to on if you really want a build in the source directory." OFF)

if(NOT IN_SOURCE_BUILD)
  if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "CMake generation for this library is not allowed within the source directory! 
Remove the CMakeCache.txt file and try again from another folder, e.g.: 

   rm CMakeCache.txt 
   cd <somewhere (preferably a local place on your computer and not a network folder)>
   cmake <source directory>

If you really need an in source build, then run : cmake -DIN_SOURCE_BUILD=ON
")
  endif(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
endif(NOT IN_SOURCE_BUILD)
