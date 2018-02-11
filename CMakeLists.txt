cmake_minimum_required(VERSION 3.4)
project(XiboPi)

find_package(PkgConfig REQUIRED)

add_subdirectory("${CMAKE_SOURCE_DIR}/src")
set(SOURCE "main.cpp" ${SOURCE})

add_executable(XiboPi ${SOURCE} ${HEADERS})

pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

pkg_check_modules(WEBKIT2 REQUIRED webkit2gtk-4.0)
include_directories(${WEBKIT2_INCLUDE_DIRS})
link_directories(${WEBKIT2_LIBRARY_DIRS})
add_definitions(${WEBKIT2_CFLAGS_OTHER})

pkg_check_modules(GSOAP REQUIRED gsoapssl++)
include_directories(${GSOAP_INCLUDE_DIRS})
link_directories(${GSOAP_LIBRARY_DIRS})
add_definitions(${GSOAP_CFLAGS_OTHER})

find_package(cppzmq)
if(cppzmq_FOUND)
  include_directories(${cppzmq_INCLUDE_DIR})
  target_link_libraries(XiboPi ${cppzmq_LIBRARY})
endif()
  
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_NO_C_LOCALE")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set_target_properties(XiboPi PROPERTIES LINKER_LANGUAGE "CXX")

target_link_libraries(XiboPi ${GTK3_LIBRARIES})
target_link_libraries(XiboPi ${WEBKIT2_LIBRARIES})
target_link_libraries(XiboPi ${GSOAP_LIBRARIES})
target_link_libraries(XiboPi XiboClient)
target_link_libraries(XiboPi XiboSoap)
target_link_libraries(XiboPi XiboZMQ)
  