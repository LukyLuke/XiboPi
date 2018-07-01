enable_testing()

set(CPP_UNITTEST
  AllTests.cpp
  xml/XmlDisplayTest.cpp
#  ${CMAKE_CURRENT_SOURCE_DIR}/xml/XmlScheduleTest.cpp
#  ${CMAKE_CURRENT_SOURCE_DIR}/xml/XmlFilesTest.cpp
#  ${CMAKE_CURRENT_SOURCE_DIR}/xml/XmlLayoutTest.cpp
#  ${CMAKE_CURRENT_SOURCE_DIR}/XiboDisplayTest.cpp
#  ${CMAKE_CURRENT_SOURCE_DIR}/XiboClientTest.cpp
#  ${CMAKE_CURRENT_SOURCE_DIR}/XiboRegionTest.cpp
)

add_executable(XiboPiTests ${CPP_UNITTEST})
#cpputest_normalize_test_output_location(XiboPiTests)

target_link_libraries(XiboPiTests CppUTest ${THREAD_LIB})
#cpputest_buildtime_discover_tests(XiboPiTests)
