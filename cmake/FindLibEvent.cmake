
# Find the CUnit includes and library
#
# This module defines
# LIBEVENT_INCLUDE_DIR, where to find cppunit include files, etc.
# LIBEVENT_LIBRARIES, the libraries to link against to use CppUnit.
# LIBEVENT_FOUND, If false, do not try to use CppUnit.

# also defined, but not for general use are
# LIBEVENT_LIBRARY, where to find the CUnit library.

#MESSAGE("Searching for cunit library ")

if(NOT UNIX)
# confilict with event.h from Platform SDK
FIND_PATH(LIBEVENT_INCLUDE_DIR event.h
  /usr/local/include
  /usr/include
  "c:/prj/libs/libevent-1.4.13-stable"  
  NO_SYSTEM_ENVIRONMENT_PATH
  )
else()
FIND_PATH(LIBEVENT_INCLUDE_DIR event.h
  /usr/local/include
  /usr/include
  "C:/prj/libs/libevent-1.4.13-stable"  
  )
endif(NOT UNIX)

if(NOT UNIX)
FIND_PATH(LIBEVENT_INCLUDE_WIN32_DIR event-config.h
  /usr/local/include
  /usr/include
  "C:/prj/libs/libevent-1.4.13-stable/WIN32-Code"  
)
endif(NOT UNIX)

if (NOT WIN32)
            FIND_LIBRARY(LIBEVENT_LIBRARY event
              /usr/local/lib
              /usr/lib
            )
else()
    FIND_LIBRARY(LIBEVENT_LIBRARY libevent
                 "C:/prj/libs/libevent-1.4.13-stable/lib/${CMAKE_BUILD_TYPE}"
                 )
endif(NOT WIN32)



IF(LIBEVENT_INCLUDE_DIR)
  IF(LIBEVENT_LIBRARY)
    SET(LIBEVENT_FOUND TRUE)
    SET(LIBEVENT_LIBRARIES ${LIBEVENT_LIBRARY} ${CMAKE_DL_LIBS})
  ENDIF(LIBEVENT_LIBRARY)
ENDIF(LIBEVENT_INCLUDE_DIR)

IF (LIBEVENT_FOUND)
   IF (NOT LibEvent_FIND_QUIETLY)
      MESSAGE(STATUS "Found LibEvent: ${LIBEVENT_LIBRARIES}")
   ENDIF (NOT LibEvent_FIND_QUIETLY)
ELSE (LIBEVENT_FOUND)
   IF (LibEvent_FIND_REQUIRED)
      MESSAGE(SEND_ERROR "Could NOT find LibEvent")
   ENDIF (LibEvent_FIND_REQUIRED)
ENDIF (LIBEVENT_FOUND)

MARK_AS_ADVANCED(LIBEVENT_INCLUDE_DIR LIBEVENT_LIBRARIES)

