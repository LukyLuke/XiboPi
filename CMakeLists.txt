project(xibogtk)

cmake_minimum_required(VERSION 3.3.1)
find_package(PkgConfig REQUIRED)

set(MYPROJECT_SOURCE_FILES
	${CMAKE_SOURCE_DIR}/src/main.cpp
)

pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

pkg_check_modules(WEBKIT2 REQUIRED webkit2gtk-4.0)
include_directories(${WEBKIT2_INCLUDE_DIRS})
link_directories(${WEBKIT2_LIBRARY_DIRS})
add_definitions(${WEBKIT2_CFLAGS_OTHER})

add_executable(xibogtk ${MYPROJECT_SOURCE_FILES})
target_link_libraries(xibogtk ${GTK3_LIBRARIES})
target_link_libraries(xibogtk ${WEBKIT2_LIBRARIES})
