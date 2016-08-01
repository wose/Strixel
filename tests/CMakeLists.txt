set(test_SRC
  main.cpp
  )

add_executable(unit_tests ${test_SRC})
target_link_libraries(unit_tests
  gmock
  gtest
  )

add_test(NAME UnitTests COMMAND unit_tests)
