
set(SOURCE
  PARENT_SCOPE
)

set(HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/config.h
  PARENT_SCOPE
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/api")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/zmq")

set(XIBO_SOURCE
  ${CMAKE_CURRENT_SOURCE_DIR}/XiboClient.cpp
)
set(XIBO_HEADER
  ${CMAKE_CURRENT_SOURCE_DIR}/config.h
  ${CMAKE_CURRENT_SOURCE_DIR}/XiboClient.h
)

add_library(XiboClient STATIC ${XIBO_SOURCE} ${XIBO_HEADER})
