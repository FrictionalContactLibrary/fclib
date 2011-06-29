# -*- cmake -*-
# This is the test cmake configuration
# built from @CMAKE_SOURCE_DIR@/cmake/CMakeListsForTests.cmake.in 

# scan the list of test executables
foreach(_EXE ${_EXE_LIST_${_CURRENT_TEST_DIRECTORY}})
 
  message(STATUS "Adding test suite ${_CURRENT_TEST_DIRECTORY}/${_EXE}")
  
  # Create an executable for the current test 
  add_executable(${_EXE} ${${_EXE}_FSOURCES})

  # Add a dependency between current test and the main library target
  add_dependencies(${_EXE} ${PROJECT_LIBRARY_NAME})
  # link current target test with the same lib as for main project lib
  target_link_libraries(${_EXE} ${PROJECT_LIBRARY_NAME})
  target_link_libraries(${_EXE} ${LIBS})
  
  # add test for ctest
  add_test(${_EXE} ${_EXE})

  set_tests_properties(${_EXE} PROPERTIES FAIL_REGULAR_EXPRESSION "FAILURE;Exception;failed;ERROR")
  
endforeach(_EXE ${_EXE_LIST_${_CURRENT_TEST_DIRECTORY}})
