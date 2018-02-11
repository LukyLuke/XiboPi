
set(SOAP_SOURCE
  ${CMAKE_CURRENT_SOURCE_DIR}/stdsoap2.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/xmdsBinding.nsmap
  ${CMAKE_CURRENT_SOURCE_DIR}/soapC.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/soapxmdsBindingProxy.cpp
)

set(SOAP_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/soapH.h
  ${CMAKE_CURRENT_SOURCE_DIR}/soapxmdsBindingProxy.h
)

add_library(XiboSoap STATIC ${SOAP_SOURCE} ${SOAP_HEADERS})
