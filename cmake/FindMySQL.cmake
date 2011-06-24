# Find MySQL includes and libraries
#
# This module defines
# LIBMYSQL_INCLUDE_DIR
# LIBMYSQL_LIBRARIES
# LIBMYSQL_FOUND
# LIBMYSQL_LIBRARY

#MESSAGE("Searching for libmysqlclien library")

FIND_PATH(LIBMYSQL_INCLUDE_DIR mysql.h
  /usr/local/include
  /usr/include
  /usr/include/mysql
  "C://Program Files//MySQL//MySQL Server 5.1//include" 
)

IF(UNIX)
  set(MYSQL_LIB_FILE "mysqlclient")
ELSE()
  set(MYSQL_LIB_FILE "libmysql")
ENDIF(UNIX) 

FIND_LIBRARY(LIBMYSQL_LIBRARY ${MYSQL_LIB_FILE} 
        "/usr/local/lib"  
        "/usr/lib"  
        "/usr/lib64"
        "C://Program Files//MySQL//MySQL Server 5.1//lib//opt"
 )

IF(LIBMYSQL_INCLUDE_DIR)
  IF(LIBMYSQL_LIBRARY)
    SET(LIBMYSQL_FOUND TRUE)
    SET(LIBMYSQL_LIBRARIES ${LIBMYSQL_LIBRARY})
  ENDIF(LIBMYSQL_LIBRARY)
ENDIF(LIBMYSQL_INCLUDE_DIR)

IF (LIBMYSQL_FOUND)
   IF (NOT LIBMYSQL_FIND_QUIETLY)
      MESSAGE(STATUS "Found libmysqlclient: ${LIBMYSQL_LIBRARIES}")
   ENDIF (NOT LIBMYSQL_FIND_QUIETLY)
ELSE (LIBMYSQL_FOUND)
   message("MySQL client development libraries had not been found") 
   IF (MySQL_FIND_REQUIRED)
      MESSAGE(SEND_ERROR "Could NOT find MySQL libraries")
   ENDIF (MySQL_FIND_REQUIRED)
ENDIF (LIBMYSQL_FOUND)

MARK_AS_ADVANCED(LIBMYSQL_INCLUDE_DIR LIBMYSQL_LIBRARIES)

