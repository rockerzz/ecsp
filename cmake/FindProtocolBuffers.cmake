if(PROTOBUF_INCLUDE_DIRS AND PROTOBUF_LIBRARIES)
  set(PROTOBUF_FOUND true)
else(PROTOBUF_INCLUDE_DIRS AND PROTOBUF_LIBRARIES)
  set(PROTOBUF_PREFIX "" CACHE PATH "Installation prefix for Google Protocol Buffers")
  if(PROTOBUF_PREFIX)
    find_path(_PROTOBUF_INCLUDE_DIR stubs/common.h
      PATHS "${PROTOBUF_PREFIX}/include"
      PATH_SUFFIXES "google/protobuf"
      NO_DEFAULT_PATH)
    find_library(_PROTOBUF_LIBRARY protobuf
      PATHS "${PROTOBUF_PREFIX}/lib"
      NO_DEFAULT_PATH)
  else(PROTOBUF_PREFIX)
    find_path(_PROTOBUF_INCLUDE_DIR stubs/common.h
      PATH_SUFFIXES "google/protobuf")
    find_library(_PROTOBUF_LIBRARY protobuf)
  endif(PROTOBUF_PREFIX)
  if(_PROTOBUF_INCLUDE_DIR AND _PROTOBUF_LIBRARY)
    set(PROTOBUF_FOUND true)
    set(PROTOBUF_INCLUDE_DIRS ${_PROTOBUF_INCLUDE_DIR} CACHE PATH
      "Include directories for Google Protocol Buffers framework")
    set(PROTOBUF_LIBRARIES ${_PROTOBUF_LIBRARY} CACHE FILEPATH
      "Libraries to link for Google Protocol Buffers framework")
    mark_as_advanced(PROTOBUF_INCLUDE_DIRS PROTOBUF_LIBRARIES)
    if(NOT PROTOBUF_FIND_QUIETLY)
      message(STATUS "Found Google Protocol Buffers: ${PROTOBUF_LIBRARIES}")
    endif(NOT PROTOBUF_FIND_QUIETLY)
  else(_PROTOBUF_INCLUDE_DIR AND _PROTOBUF_LIBRARY)
    if(PROTOBUF_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find the Google Protocol Buffers framework")
    endif(PROTOBUF_FIND_REQUIRED)
  endif(_PROTOBUF_INCLUDE_DIR AND _PROTOBUF_LIBRARY)
endif(PROTOBUF_INCLUDE_DIRS AND PROTOBUF_LIBRARIES)

mark_as_advanced(_PROTOBUF_LIBRARY)
