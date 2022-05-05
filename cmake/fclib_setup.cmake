# Set directory used to save cmake config files
# required to use fclib (e.g. to call find_package(fclib) )
set(ConfigPackageLocation lib/cmake/fclib-${PACKAGE_VERSION})

# Provides install directory variables as defined by the GNU Coding Standards.
include(GNUInstallDirs)  # It defines CMAKE_INSTALL_LIBDIR
# RPATH stuff
# Warning: RPATH settings must be defined before install(...) settings.
# Source : https://gitlab.kitware.com/cmake/community/wikis/doc/cmake/RPATH-handling

if(FORCE_SKIP_RPATH) # Build with no RPATH. Do we really need this option??
  set(CMAKE_SKIP_BUILD_RPATH TRUE)
else()
  set(CMAKE_SKIP_BUILD_RPATH FALSE)
endif()

# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

# when building a binary package, it makes no sense to add this rpath
if(NOT FORCE_SKIP_RPATH)
  # the RPATH to be used when installing
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
endif(NOT FORCE_SKIP_RPATH)

# don't add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# the RPATH to be used when installing, but only if it's not a system directory
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
endif()


