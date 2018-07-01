
set(SOURCE
  PARENT_SCOPE
)

set(HEADERS
  PARENT_SCOPE
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(XIBO_SOURCE
  ${CMAKE_CURRENT_SOURCE_DIR}/xml/XmlDisplay.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/xml/XmlSchedule.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/xml/XmlFiles.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/xml/XmlLayout.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/XiboConfig.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Event.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/XiboDisplay.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/XiboClient.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/XiboRegion.cpp
)
set(XIBO_HEADER
  ${CMAKE_CURRENT_SOURCE_DIR}/xml/XmlDisplay.h
  ${CMAKE_CURRENT_SOURCE_DIR}/xml/XmlSchedule.h
  ${CMAKE_CURRENT_SOURCE_DIR}/xml/XmlFiles.h
  ${CMAKE_CURRENT_SOURCE_DIR}/xml/XmlLayout.h
  ${CMAKE_CURRENT_SOURCE_DIR}/XiboConfig.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Event.h
  ${CMAKE_CURRENT_SOURCE_DIR}/XiboDisplay.h
  ${CMAKE_CURRENT_SOURCE_DIR}/XiboClient.h
  ${CMAKE_CURRENT_SOURCE_DIR}/XiboRegion.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/api")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/zmq")

add_library(XiboClient STATIC ${XIBO_SOURCE} ${XIBO_HEADER})
