file(GLOB TOOLKIT_HEADERS 
        "${CMAKE_SOURCE_DIR}/include/toolkit/*.h"
        "*.h"
)

if (UNIX)
	set(IMPL_SOURCES daemon_unix.cc)
else()               
	set(IMPL_SOURCES daemon_windows.cc)
endif(UNIX)	

set(TOOLKIT_SOURCES 
        tcpsocket.cc  
        ${IMPL_SOURCES}
        cpu.cc
        http_event.cc
        puuid.cc
	logger.cc
   )

if(NOT WIN32)
    set(DL_LIB "dl")
    set(UUID_LIB "uuid")
else()
    set(SOCKETS_LIB "wsock32.lib")
    set(UUID_LIB "Rpcrt4.lib")
endif(NOT WIN32)

include_directories(${LIBEVENT_INCLUDE_DIR})
if(NOT UNIX)
	include_directories(${LIBEVENT_INCLUDE_WIN32_DIR})
endif(NOT UNIX)

include_directories("${CMAKE_SOURCE_DIR}/src/3rdparty/json_cpp/include")

add_library(toolkit SHARED ${TOOLKIT_SOURCES} ${TOOLKIT_HEADERS})
target_link_libraries(toolkit ${SOCKETS_LIB} ${LIBEVENT_LIBRARIES} ${UUID_LIB} platform)

if(INSTALL_INCLUDE_DIR)
    install(FILES "${CMAKE_SOURCE_DIR}/src/toolkit/logger.h" DESTINATION "${INSTALL_INCLUDE_DIR}/toolkit")
endif(INSTALL_INCLUDE_DIR)

install(TARGETS toolkit DESTINATION ${INSTALL_LIB_DIR})
      
