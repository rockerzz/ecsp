INCLUDE(CMakeMacroParseArguments)

set(PYTHON_PLUGIN_DIR "${CMAKE_SOURCE_DIR}/src/plugins/python_plugins")
set(PYTHON_BUILD_DIR "${CMAKE_BINARY_DIR}/plugins/python_plugins")
set(PYTHON_OUTPUT_DIR "${CMAKE_BINARY_DIR}/lib/plugins")

if (UNIX)
           set(BATCH_SUFFIX ".sh")
else()
           set(BATCH_SUFFIX ".bat")
endif(UNIX)

if (UNIX)
    set(TOOL_LIB_DIR "${CMAKE_BINARY_DIR}/lib")
else()
    if (MSVC_IDE)
            # devenv.exe builds
        #set(TOOL_LIB_DIR "${CMAKE_BINARY_DIR}/bin/Release")
        set(TOOL_LIB_DIR "${CMAKE_BINARY_DIR}/bin/Debug")
    else()
            # nmake.exe builds
            set(TOOL_LIB_DIR "${CMAKE_BINARY_DIR}/bin")
    endif(MSVC_IDE)
endif (UNIX)


MACRO(ADD_PY_PLUGIN PLUGIN_NAME)

        separate_arguments(PLUGINS)
        list(FIND PLUGINS ${PLUGIN_NAME} INDEX)
        string(COMPARE NOTEQUAL ${INDEX} "-1" NEED_PLUGIN)
        if (NEED_PLUGIN)

        message(STATUS "ADD_PY_PLUGIN ${PLUGIN_NAME}")

        set(MAKE_PLUGIN_TOOL_FILES
            "${CMAKE_SOURCE_DIR}/tools/make_plugin/make_plugin_executor${BATCH_SUFFIX}"
            "${CMAKE_SOURCE_DIR}/tools/make_plugin/make_plugin.py"
            "${CMAKE_SOURCE_DIR}/tools/make_plugin/templates/base_plugin.py"
            "${CMAKE_SOURCE_DIR}/tools/make_plugin/templates/init_plugin.py"
            "${CMAKE_SOURCE_DIR}/tools/make_plugin/templates/setup.py"
            pylibconfig)

        set(BASE_SOURCE_FILES 
#                              "${PYTHON_PLUGIN_DIR}/${PLUGIN_NAME}/src/base_plugin.py"
#                              "${PYTHON_PLUGIN_DIR}/${PLUGIN_NAME}/src/init_plugin.py"
                              "${PYTHON_PLUGIN_DIR}/${PLUGIN_NAME}/src/__init__.py"
                              "${PYTHON_PLUGIN_DIR}/${PLUGIN_NAME}/setup.py")
        add_custom_command(
            OUTPUT ${BASE_SOURCE_FILES}
            COMMAND "${CMAKE_SOURCE_DIR}/tools/make_plugin/make_plugin_executor${BATCH_SUFFIX}"
            ARGS
                ${TOOL_LIB_DIR}
                ${TOOL_LIB_DIR}
                "${CMAKE_SOURCE_DIR}/tools/make_plugin/make_plugin.py"
                ${CMAKE_BINARY_DIR}
                "${PYTHON_PLUGIN_DIR}/${PLUGIN_NAME}/${PLUGIN_NAME}.conf"
            DEPENDS "${PYTHON_PLUGIN_DIR}/${PLUGIN_NAME}/${PLUGIN_NAME}.conf"
                    ${MAKE_PLUGIN_TOOL_FILES}
        )

        # CREATE FOLDERS
        file(MAKE_DIRECTORY ${PYTHON_BUILD_DIR}/${PLUGIN_NAME}/src) 
        file(MAKE_DIRECTORY ${PYTHON_OUTPUT_DIR})

        file(GLOB_RECURSE PLUGIN_FILES "${PYTHON_PLUGIN_DIR}/${PLUGIN_NAME}/*")
        list(APPEND PLUGIN_FILES ${BASE_SOURCE_FILES})
        set(BUILD_PLUGIN_FILES)
        foreach(pyfile ${PLUGIN_FILES})
            string(REPLACE ${PYTHON_PLUGIN_DIR} ${PYTHON_BUILD_DIR} tofile ${pyfile})
            add_custom_command(
                OUTPUT ${tofile}
                COMMAND ${CMAKE_COMMAND}
                ARGS -E copy ${pyfile} ${tofile}
                DEPENDS ${BASE_SOURCE_FILES}
            )
            list(APPEND BUILD_PLUGIN_FILES ${tofile})
        endforeach(pyfile)

        set(EGG_FILE_NAME "${PLUGIN_NAME}-0.0.0-py${PYTHON_SHORT_VERSION}.egg")

        add_custom_command(
            OUTPUT "${PYTHON_BUILD_DIR}/${PLUGIN_NAME}/dist/${EGG_FILE_NAME}"
            COMMAND ${PYTHON_EXECUTABLE} 
            ARGS setup.py bdist_egg ${PYTHON_EGG_PARAM} WORKING_DIRECTORY ${PYTHON_BUILD_DIR}/${PLUGIN_NAME}
            DEPENDS ${BUILD_PLUGIN_FILES}
        )

        add_custom_command(
            OUTPUT "${PYTHON_OUTPUT_DIR}/${EGG_FILE_NAME}"
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy "${PYTHON_BUILD_DIR}/${PLUGIN_NAME}/dist/${EGG_FILE_NAME}" "${PYTHON_OUTPUT_DIR}/${EGG_FILE_NAME}"
            DEPENDS "${PYTHON_BUILD_DIR}/${PLUGIN_NAME}/dist/${EGG_FILE_NAME}"
        )

        install(FILES "${PYTHON_OUTPUT_DIR}/${EGG_FILE_NAME}" DESTINATION ${INSTALL_PLUGIN_DIR})

        add_dependencies(${PROJECT_NAME} ${PLUGIN_NAME}_build_egg)

        add_custom_target(${PLUGIN_NAME}_build_egg 
            DEPENDS "${PYTHON_OUTPUT_DIR}/${EGG_FILE_NAME}"
        )

    endif(NEED_PLUGIN)

ENDMACRO(ADD_PY_PLUGIN)

MACRO(ADD_CXX_PLUGIN)

    PARSE_ARGUMENTS(PLUGIN "LIBRARIES" "" ${ARGN})
    CAR(PLUGIN_NAME ${PLUGIN_DEFAULT_ARGS})
    separate_arguments(PLUGINS)
    list(FIND PLUGINS ${PLUGIN_NAME} INDEX)
    string(COMPARE NOTEQUAL ${INDEX} "-1" NEED_PLUGIN)
    if (NEED_PLUGIN)
        file(GLOB SOURCE_FILES "${CMAKE_SOURCE_DIR}/src/plugins/${PLUGIN_NAME}/*.cc")
        file(GLOB PLUGIN_HEADER_FILES "${CMAKE_SOURCE_DIR}/src/plugins/${PLUGIN_NAME}/*.h")
        file(GLOB CONF_FILES "${CMAKE_SOURCE_DIR}/src/plugins/${PLUGIN_NAME}/*.conf")
        set(BASE_SOURCE_FILES "${CMAKE_BINARY_DIR}/src/plugins/${PLUGIN_NAME}/${PLUGIN_NAME}_base.cc")
        set(BASE_INCLUDE_FILES "${CMAKE_BINARY_DIR}/include/plugins/${PLUGIN_NAME}_base.h")
        add_library(${PLUGIN_NAME} SHARED ${SOURCE_FILES} 
										  ${BASE_SOURCE_FILES} 
										  ${BASE_INCLUDE_FILES}
										  ${PLUGIN_HEADER_FILES}
										  ${CONF_FILES} )    
        target_link_libraries(${PLUGIN_NAME} ${PLUGIN_LIBRARIES})

        set(MAKE_PLUGIN_TOOL_FILES 
            "${CMAKE_SOURCE_DIR}/tools/make_plugin/make_plugin_executor${BATCH_SUFFIX}"
            "${CMAKE_SOURCE_DIR}/tools/make_plugin/make_plugin.py"
            "${CMAKE_SOURCE_DIR}/tools/make_plugin/templates/base.h"
            "${CMAKE_SOURCE_DIR}/tools/make_plugin/templates/base.cc"
            "${CMAKE_SOURCE_DIR}/tools/make_plugin/templates/impl.h"
            "${CMAKE_SOURCE_DIR}/tools/make_plugin/templates/impl.cc"
            pylibconfig)

        if (UNIX)
            set(TOOL_LIB_DIR "${CMAKE_BINARY_DIR}/lib")
        else()
            if (MSVC_IDE)
	            # devenv.exe builds
                set(TOOL_LIB_DIR "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}")
            else()
	            # nmake.exe builds
	            set(TOOL_LIB_DIR "${CMAKE_BINARY_DIR}/bin")
            endif(MSVC_IDE)
        endif (UNIX)

        add_custom_command(
            OUTPUT ${BASE_SOURCE_FILES} ${BASE_INCLUDE_FILES}
            COMMAND "${CMAKE_SOURCE_DIR}/tools/make_plugin/make_plugin_executor${BATCH_SUFFIX}"
            ARGS 
                 ${TOOL_LIB_DIR}
                 ${TOOL_LIB_DIR}
                 "${CMAKE_SOURCE_DIR}/tools/make_plugin/make_plugin.py"
                 ${CMAKE_BINARY_DIR}
                 "${CMAKE_SOURCE_DIR}/src/plugins/${PLUGIN_NAME}/${PLUGIN_NAME}.conf"
            DEPENDS "${CMAKE_SOURCE_DIR}/src/plugins/${PLUGIN_NAME}/${PLUGIN_NAME}.conf"
                    ${MAKE_PLUGIN_TOOL_FILES}
         )


        # Consider CMakeLists if exist
        file(GLOB CMAKELISTS "${CMAKE_SOURCE_DIR}/src/plugins/${PLUGIN_NAME}/CMakeLists.txt")
        if(CMAKELISTS)
            subdirs(${CMAKE_SOURCE_DIR}/src/plugins/${PLUGIN_NAME})
        endif(CMAKELISTS)

        install(TARGETS ${PLUGIN_NAME} DESTINATION ${INSTALL_PLUGIN_DIR})
               
    endif(NEED_PLUGIN)

ENDMACRO(ADD_CXX_PLUGIN)
  
