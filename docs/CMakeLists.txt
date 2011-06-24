OPTION(USE_DOT "Use dot program for generating graphical class diagrams with doxygen, if available" OFF)

SET(HAVE_DOT "NO")
IF(DOT)
  IF(USE_DOT)
    SET(HAVE_DOT "NO")
  ENDIF(USE_DOT)
ENDIF(DOT)

CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/docs/doxygen.conf.in
               ${PROJECT_BINARY_DIR}/docs/doxygen.conf)

FIND_PACKAGE(Doxygen)

# Doxygen config file requires only the path to DOT, not DOT itself
GET_FILENAME_COMPONENT(DOT_PATH ${DOT} PATH CACHE)

ADD_CUSTOM_TARGET(doxydoc 
                  ${DOXYGEN} 
                  ${PROJECT_BINARY_DIR}/docs/doxygen.conf
                  )



