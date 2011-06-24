## Binary package install paths ###########

set(INSTALL_BIN_DIR "bin" CACHE STRING "Path for binaries installation")
mark_as_advanced(INSTALL_BIN_DIR)

set(INSTALL_LIB_DIR "lib/${CMAKE_PROJECT_NAME}" CACHE STRING "Path for libraries installation")
mark_as_advanced(INSTALL_LIB_DIR)

set(INSTALL_INIT_DIR "etc/init.d" CACHE STRING "Path for init installation")
mark_as_advanced(INSTALL_INIT_DIR)

set(INSTALL_INCLUDE_DIR "include/${CMAKE_PROJECT_NAME}" CACHE STRING "Path for include headers installation")
mark_as_advanced(INSTALL_INCLUDE_DIR)

set(INSTALL_PLUGIN_DIR "lib/${CMAKE_PROJECT_NAME}/plugins" CACHE STRING "Path for plugins installation")
mark_as_advanced(INSTALL_INCLUDE_DIR)

set(INSTALL_CONFIG_DIR "etc/${CMAKE_PROJECT_NAME}" CACHE STRING "Path for configuration insallation")
mark_as_advanced(INSTALL_CONFIG_DIR)

set(INSTALL_DOC_DIR "share/doc/${CMAKE_PROJECT_NAME}" CACHE STRING "Path for doc installation")
mark_as_advanced(INSTALL_DOC_DIR)

set(INSTALL_EXAMPLE_DIR "share/${CMAKE_PROJECT_NAME}/examples" CACHE STRING "Path for examples installation")
mark_as_advanced(INSTALL_EXAMPLE_DIR)

set(INSTALL_TOOL_DIR "share/${CMAKE_PROJECT_NAME}/tools" CACHE STRING "Path for developer toolkit installation")
mark_as_advanced(INSTALL_TOOL_DIR)


# skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
#
# # when building, use the install RPATH already
# # (so it doesn't need to relink when installing)
#SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

# # the RPATH to be used when installing
SET(CMAKE_INSTALL_RPATH "../${INSTALL_LIB_DIR}")
#
# # add the automatically determined parts of the RPATH
# # which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

##########################################

##############################
# Packaging support
##############################

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION "${VERSION}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} ${VERSION}")
set(CPACK_PACKAGE_VENDOR "Alexey Gelyadov")
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
if(NOT UNIX)
    set(CPACK_GENERATOR NSIS)
else()
    set(CPACK_GENERATOR TGZ DEB)
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libboost-python-dev, libevent-dev, python")

    if(WITH_PGSQL)
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, postgresql")
    endif(WITH_PGSQL)

    if(WITH_MYSQL)
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, mysql-server")
    endif(WITH_MYSQL)

endif(NOT UNIX)
set(CPACK_SOURCE_GENERATOR TGZ)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER " ")
set(CPACK_SOURCE_IGNORE_FILES "/build/;/.hg/")
set(CPACK_SET_DESTDIR "ON")

include(CPack)


##############################
#uninstall
##############################

configure_file(
        "${CMAKE_MODULE_PATH}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

add_custom_target(uninstall
        C_HEADER"${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

