# Install script for directory: C:/Users/himself65/Desktop/OpenArkCompiler-master-ae976ab813e7243fe8c59a9dd0dc0ddedd773acf/src/third_party/zlib-1.2.11

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Users/himself65/Desktop/OpenArkCompiler-master-ae976ab813e7243fe8c59a9dd0dc0ddedd773acf/src/third_party/zlib-1.2.11/out/install/x64-Debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Users/himself65/Desktop/OpenArkCompiler-master-ae976ab813e7243fe8c59a9dd0dc0ddedd773acf/src/third_party/zlib-1.2.11/out/install/x64-Debug/lib/zlibd.lib")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "C:/Users/himself65/Desktop/OpenArkCompiler-master-ae976ab813e7243fe8c59a9dd0dc0ddedd773acf/src/third_party/zlib-1.2.11/out/install/x64-Debug/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/himself65/Desktop/OpenArkCompiler-master-ae976ab813e7243fe8c59a9dd0dc0ddedd773acf/src/third_party/zlib-1.2.11/out/build/x64-Debug/zlibd.lib")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Users/himself65/Desktop/OpenArkCompiler-master-ae976ab813e7243fe8c59a9dd0dc0ddedd773acf/src/third_party/zlib-1.2.11/out/install/x64-Debug/bin/zlibd.dll")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "C:/Users/himself65/Desktop/OpenArkCompiler-master-ae976ab813e7243fe8c59a9dd0dc0ddedd773acf/src/third_party/zlib-1.2.11/out/install/x64-Debug/bin" TYPE SHARED_LIBRARY FILES "C:/Users/himself65/Desktop/OpenArkCompiler-master-ae976ab813e7243fe8c59a9dd0dc0ddedd773acf/src/third_party/zlib-1.2.11/out/build/x64-Debug/zlibd.dll")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Users/himself65/Desktop/OpenArkCompiler-master-ae976ab813e7243fe8c59a9dd0dc0ddedd773acf/src/third_party/zlib-1.2.11/out/install/x64-Debug/lib/zlibstaticd.lib")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "C:/Users/himself65/Desktop/OpenArkCompiler-master-ae976ab813e7243fe8c59a9dd0dc0ddedd773acf/src/third_party/zlib-1.2.11/out/install/x64-Debug/lib" TYPE STATIC_LIBRARY FILES "C:/Users/himself65/Desktop/OpenArkCompiler-master-ae976ab813e7243fe8c59a9dd0dc0ddedd773acf/src/third_party/zlib-1.2.11/out/build/x64-Debug/zlibstaticd.lib")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Users/himself65/Desktop/OpenArkCompiler-master-ae976ab813e7243fe8c59a9dd0dc0ddedd773acf/src/third_party/zlib-1.2.11/out/install/x64-Debug/include/zconf.h;C:/Users/himself65/Desktop/OpenArkCompiler-master-ae976ab813e7243fe8c59a9dd0dc0ddedd773acf/src/third_party/zlib-1.2.11/out/install/x64-Debug/include/zlib.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "C:/Users/himself65/Desktop/OpenArkCompiler-master-ae976ab813e7243fe8c59a9dd0dc0ddedd773acf/src/third_party/zlib-1.2.11/out/install/x64-Debug/include" TYPE FILE FILES
    "C:/Users/himself65/Desktop/OpenArkCompiler-master-ae976ab813e7243fe8c59a9dd0dc0ddedd773acf/src/third_party/zlib-1.2.11/out/build/x64-Debug/zconf.h"
    "C:/Users/himself65/Desktop/OpenArkCompiler-master-ae976ab813e7243fe8c59a9dd0dc0ddedd773acf/src/third_party/zlib-1.2.11/zlib.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Users/himself65/Desktop/OpenArkCompiler-master-ae976ab813e7243fe8c59a9dd0dc0ddedd773acf/src/third_party/zlib-1.2.11/out/install/x64-Debug/share/man/man3/zlib.3")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "C:/Users/himself65/Desktop/OpenArkCompiler-master-ae976ab813e7243fe8c59a9dd0dc0ddedd773acf/src/third_party/zlib-1.2.11/out/install/x64-Debug/share/man/man3" TYPE FILE FILES "C:/Users/himself65/Desktop/OpenArkCompiler-master-ae976ab813e7243fe8c59a9dd0dc0ddedd773acf/src/third_party/zlib-1.2.11/zlib.3")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Users/himself65/Desktop/OpenArkCompiler-master-ae976ab813e7243fe8c59a9dd0dc0ddedd773acf/src/third_party/zlib-1.2.11/out/install/x64-Debug/share/pkgconfig/zlib.pc")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "C:/Users/himself65/Desktop/OpenArkCompiler-master-ae976ab813e7243fe8c59a9dd0dc0ddedd773acf/src/third_party/zlib-1.2.11/out/install/x64-Debug/share/pkgconfig" TYPE FILE FILES "C:/Users/himself65/Desktop/OpenArkCompiler-master-ae976ab813e7243fe8c59a9dd0dc0ddedd773acf/src/third_party/zlib-1.2.11/out/build/x64-Debug/zlib.pc")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/Users/himself65/Desktop/OpenArkCompiler-master-ae976ab813e7243fe8c59a9dd0dc0ddedd773acf/src/third_party/zlib-1.2.11/out/build/x64-Debug/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
