cmake_minimum_required(VERSION 3.4)
project(XiboPi)

find_package(PkgConfig REQUIRED)

option(COVERAGE "Enable running with coverage" OFF)
option(TESTS "Compile and make tests for the code?" OFF)
option(TESTS_DETAILED "Run each test separately instead of grouped?" OFF)

pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

pkg_check_modules(WEBKIT2 REQUIRED webkit2gtk-4.0)
include_directories(${WEBKIT2_INCLUDE_DIRS})
link_directories(${WEBKIT2_LIBRARY_DIRS})
add_definitions(${WEBKIT2_CFLAGS_OTHER})

pkg_check_modules(GSOAP REQUIRED gsoap++)
include_directories(${GSOAP_INCLUDE_DIRS})
link_directories(${GSOAP_LIBRARY_DIRS})
add_definitions(${GSOAP_CFLAGS_OTHER})

pkg_check_modules(EXPAT REQUIRED expat)
include_directories(${EXPAT_INCLUDE_DIRS})
link_directories(${EXPAT_LIBRARY_DIRS})
add_definitions(${EXPAT_CFLAGS_OTHER})

pkg_check_modules(SSL REQUIRED openssl)
include_directories(${SSL_INCLUDE_DIRS})
link_directories(${SSL_LIBRARY_DIRS})
add_definitions(${SSL_CFLAGS_OTHER})

find_package(Threads REQUIRED)

find_package(cppzmq REQUIRED)
include_directories(${cppzmq_INCLUDE_DIR})

find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})

add_subdirectory("${CMAKE_SOURCE_DIR}/src")
set(SOURCE "main.cpp" ${SOURCE})

if (TESTS)
  add_subdirectory(test)
endif (TESTS)

add_executable(XiboPi ${SOURCE} ${HEADERS})
  
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_NO_C_LOCALE")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set_target_properties(XiboPi PROPERTIES LINKER_LANGUAGE "CXX")

target_link_libraries(XiboPi ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(XiboPi ${cppzmq_LIBRARY})
target_link_libraries(XiboPi ${WEBKIT2_LIBRARIES})
target_link_libraries(XiboPi ${GTK3_LIBRARIES})
target_link_libraries(XiboPi ${GSOAP_LIBRARIES})
target_link_libraries(XiboPi ${EXPAT_LIBRARIES})
target_link_libraries(XiboPi ${ZLIB_LIBRARIES})
target_link_libraries(XiboPi ${SSL_LIBRARIES})
target_link_libraries(XiboPi XiboClient)
target_link_libraries(XiboPi XiboSoap)
target_link_libraries(XiboPi XiboZMQ)
