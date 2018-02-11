#!/bin/sh 

# Parsing the WSDL and creates the Header-File
wsdl2h -c++11 -o xibo.h service_v5.wsdl

# Creates the real C++11 Code based on the Header-File
soapcpp2 -j -C -c++11 xibo.h
