#===========================================================
# Macro to install a package
#
# F. Pérignon, LJK-CNRS, april 2011
#
#===========================================================

macro(install_package _PACK _LIB_NAME _HEADERSLIST)
  
  set(_HEADERS ${${_HEADERSLIST}})
  
  # Offer the user the choice of overriding the installation directories
  ASSERT(CMAKE_INSTALL_LIBDIR)
  set(INSTALL_LIB_DIR ${CMAKE_INSTALL_LIBDIR} CACHE PATH "Installation directory for libraries")
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
  
  
  export(TARGETS ${_LIB_NAME} FILE "${PROJECT_BINARY_DIR}/InstallFiles/${_PACK}LibraryDepends.cmake")
  
  # Install the export set for use with the install-tree
  install(EXPORT ${_PACK}LibraryDepends DESTINATION
    "${CMAKE_INSTALL_LIBDIR}/cmake/${_PACK}" COMPONENT dev)
  
   include(CMakePackageConfigHelpers)
  configure_package_config_file(${_PACK}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${_PACK}Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${_PACK}
    PATH_VARS INCLUDE_INSTALL_DIR)
  write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${_PACK}ConfigVersion.cmake
    VERSION ${${_PACK}_version}
    COMPATIBILITY SameMajorVersion )
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${_PACK}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${_PACK}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${_PACK} )
   
endmacro()
