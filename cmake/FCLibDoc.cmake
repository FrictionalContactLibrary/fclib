#
# FClib documentation with Doxygen 
#

find_package(Doxygen REQUIRED dot)

if(DOXYGEN_FOUND)
  set(DOXYGEN_EXCLUDE_PATTERNS *tests*)
  set(DOXYGEN_USE_MATHJAX YES)
  set(DOXYGEN_EXTRA_PACKAGES amsmath)
  set(DOXYGEN_PROJECT_NAME "FCLIB: a collection  of discrete 3D Frictional Contact (FC) problems")
  set(DOXYGEN_PROJECT_NUMBER "Version ${${PACKAGE_NAME}_version}")
  doxygen_add_docs(
    fclib-doc
    src config/mainpage.doxygen config/additionalpages.doxygen
    )
    
endif() 

