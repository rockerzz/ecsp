# Find Postgres includes and libraries
#
# This module defines
# LIBPQ_INCLUDE_DIR
# LIBPQ_LIBRARIES
# LIBPQ_FOUND
# LIBPQ_LIBRARY

#MESSAGE("Searching for libpq library")

FIND_PATH(LIBPQ_INCLUDE_DIR libpq-fe.h
  /usr/local/include
  /usr/include
  /usr/include/postgresql
  /usr/include/pgsql
  "c:/Program Files/PostgreSQL/8.4/include" 
)

IF(UNIX)
  set(POSTGRES_LIB_FILE "pq")
ELSE()
  set(POSTGRES_LIB_FILE "libpq")
ENDIF(UNIX) 

FIND_LIBRARY(LIBPQ_LIBRARY ${POSTGRES_LIB_FILE} 
        "/usr/local/lib"  
        "/usr/lib"  
        "/usr/lib64"
	"c:/Program Files/PostgreSQL/8.4/lib" )

IF(LIBPQ_INCLUDE_DIR)
  IF(LIBPQ_LIBRARY)
    SET(LIBPQ_FOUND TRUE)
    SET(LIBPQ_LIBRARIES ${LIBPQ_LIBRARY})
  ENDIF(LIBPQ_LIBRARY)
ENDIF(LIBPQ_INCLUDE_DIR)

IF (LIBPQ_FOUND)
   IF (NOT LIBPQ_FIND_QUIETLY)
      MESSAGE(STATUS "Found libpq: ${LIBPQ_LIBRARIES}")
   ENDIF (NOT LIBPQ_FIND_QUIETLY)
ELSE (LIBPQ_FOUND)
   message("Postgres client development libraries had not been found") 
   IF (Postgres_FIND_REQUIRED)
      MESSAGE(SEND_ERROR "Could NOT find Postgres libraries")
   ENDIF (Postgres_FIND_REQUIRED)
ENDIF (LIBPQ_FOUND)

MARK_AS_ADVANCED(LIBPQ_INCLUDE_DIR LIBPQ_LIBRARIES)

