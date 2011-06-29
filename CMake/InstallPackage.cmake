#===========================================================
# Macro to install a package
#
# F. Pérignon, LJK-CNRS, april 2011
#
#===========================================================

macro(install_package _PACK _LIB_NAME _HEADERSLIST)
  
  set(_HEADERS ${${_HEADERSLIST}})
  
  # Offer the user the choice of overriding the installation directories
  set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
  set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
  set(INSTALL_INCLUDE_DIR include CACHE PATH
    "Installation directory for header files")
  set(INSTALL_DATA_DIR share CACHE PATH
    "Installation directory for data files")

  # Make relative paths absolute (needed later on)
  foreach(p LIB BIN INCLUDE DATA)
    set(var INSTALL_${p}_DIR)
    if(NOT IS_ABSOLUTE "${${var}}")
      set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
    endif()
  endforeach()
  
  # The library
  install(TARGETS ${_LIB_NAME} 
    EXPORT ${_PACK}LibraryDepends
    ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"  # static libs
    LIBRARY DESTINATION "${INSTALL_LIB_DIR}"  COMPONENT shlib # shared libs
    PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}" COMPONENT dev
    )
  
  # The headers and modules
  install(FILES ${_HEADERS} DESTINATION "${INSTALL_INCLUDE_DIR}")
  
  install(DIRECTORY ${CMAKE_BINARY_DIR}/Modules DESTINATION "${INSTALL_INCLUDE_DIR}")
  
  export(TARGETS ${_LIB_NAME} FILE "${PROJECT_BINARY_DIR}/InstallFiles/${_PACK}LibraryDepends.cmake")
  
  # Install the export set for use with the install-tree
  install(EXPORT ${_PACK}LibraryDepends DESTINATION
    "${INSTALL_DATA_DIR}/CMake" COMPONENT dev)
  
  set(${_PACK}_INCLUDE_DIRS "${INSTALL_INCLUDE_DIR}")
  set(${_PACK}_LIB_DIR "${INSTALL_LIB_DIR}")
  set(${_PACK}_CMAKE_DIR "${INSTALL_DATA_DIR}/CMake")
  configure_file(${_PACK}Config.cmake.in
    "${PROJECT_BINARY_DIR}/InstallFiles/${_PACK}Config.cmake")
  configure_file(${_PACK}ConfigVersion.cmake.in
    "${PROJECT_BINARY_DIR}/InstallFiles/${_PACK}ConfigVersion.cmake" @ONLY)
  install(FILES
    "${PROJECT_BINARY_DIR}/InstallFiles/${_PACK}Config.cmake"
    "${PROJECT_BINARY_DIR}/InstallFiles/${_PACK}ConfigVersion.cmake"
    DESTINATION "${${_PACK}_CMAKE_DIR}" COMPONENT dev)
  
endmacro()
