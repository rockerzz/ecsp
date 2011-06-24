INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/ 
        ${CMAKE_SOURCE_DIR}/plugins
        ${CMAKE_SOURCE_DIR}/include 
        ${CMAKE_BINARY_DIR}/include 
        ${CMAKE_BINARY_DIR}
        /usr/local/include )

ADD_DEFINITIONS(" -fno-enforce-eh-specs ")

