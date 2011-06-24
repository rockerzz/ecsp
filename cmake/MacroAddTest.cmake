include(CMakeMacroParseArguments)

MACRO(ADD_CXX_TEST)

    PARSE_ARGUMENTS(TEST "PATH;LIBRARIES;EXTRA_SOURCES" "" ${ARGN})
    CAR(TEST_NAME ${TEST_DEFAULT_ARGS})

    string(REGEX MATCH "[]plugins[]" MATCH ${TEST_PATH})

    if(MATCH)
        separate_arguments(PLUGINS)
        list(FIND PLUGINS ${TEST_NAME} INDEX)
        string(COMPARE NOTEQUAL ${INDEX} "-1" PLUGIN_DONE)
        if(PLUGIN_DONE)
            ADD_CXX_TEST_BODY(${TEST_NAME} PATH ${TEST_PATH} LIBRARIES ${TEST_LIBRARIES} EXTRA_SOURCES ${TEST_EXTRA_SOURCES})
        endif(PLUGIN_DONE)
    else(MATCH)
            ADD_CXX_TEST_BODY(${TEST_NAME} PATH ${TEST_PATH} LIBRARIES ${TEST_LIBRARIES} EXTRA_SOURCES ${TEST_EXTRA_SOURCES})
    endif(MATCH)

ENDMACRO(ADD_CXX_TEST)

MACRO(ADD_CXX_TEST_BODY)

    PARSE_ARGUMENTS(TEST "PATH;LIBRARIES;EXTRA_SOURCES" "" ${ARGN})
    CAR(TEST_NAME ${TEST_DEFAULT_ARGS})

    set(TEST_DIR "testsuite/xunit/${TEST_PATH}/${TEST_NAME}")
    set(TMP_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${TEST_DIR}")

    include_directories(${GTEST_INCLUDE_DIRS})	
    include_directories(${GMOCK_INCLUDE_DIRS})	
    file(GLOB TEST_SOURCES "${CMAKE_SOURCE_DIR}/${TEST_DIR}/*.cc")
    add_executable("${TEST_NAME}_test" ${CMAKE_SOURCE_DIR}/testsuite/xunit/testbase/gtest_main.cc ${TEST_SOURCES} ${TEST_EXTRA_SOURCES})
    target_link_libraries("${TEST_NAME}_test" ${TEST_LIBRARIES} ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES} platform)
    target_link_libraries("${TEST_NAME}_test" ${TEST_LIBRARIES} ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES} platform)
    add_test("${TEST_NAME}_test" "${CMAKE_BINARY_DIR}/${TEST_DIR}/${TEST_NAME}_test" "--gtest_color=yes")

    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${TEST_DIR}/common")
    file(GLOB RESOURCE_FILES "${CMAKE_SOURCE_DIR}/${TEST_DIR}/common/*")
    foreach(resource ${RESOURCE_FILES})
        string(REPLACE ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} to_resource ${resource})
        configure_file(${resource} ${to_resource} COPYONLY)
    endforeach(resource)

    file(GLOB CMAKELISTS "${CMAKE_SOURCE_DIR}/${TEST_DIR}/CMakeLists.txt")
    if(CMAKELISTS)
        subdirs(${CMAKE_SOURCE_DIR}/${TEST_DIR})
    endif(CMAKELISTS)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TMP_DIR})

ENDMACRO(ADD_CXX_TEST_BODY)

MACRO(ADD_PY_TEST)

    PARSE_ARGUMENTS(TEST "PATH;LIBRARIES" "" ${ARGN})
    CAR(TEST_NAME ${TEST_DEFAULT_ARGS})
    IF(UNIX)	
      SET(ENV{PYTHONPATH} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
    ELSE()
      SET(ENV{PYTHONPATH} "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}\;${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" )
    ENDIF(UNIX)
    file(GLOB TEST_FILES "${CMAKE_SOURCE_DIR}/testsuite/xunit/${TEST_PATH}/${TEST_NAME}/*.py")
    foreach(pyfile ${TEST_FILES})
        string(REPLACE  ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} tofile ${pyfile})
        CONFIGURE_FILE(${pyfile} ${tofile})

        STRING(REPLACE "${CMAKE_SOURCE_DIR}/testsuite/xunit/${TEST_PATH}/${TEST_NAME}/" "" file_name ${pyfile})
        STRING(REPLACE ".py" "" test ${file_name})

        FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${test}_cmake.cmake
        "
            SET(ENV{PYTHONPATH} $ENV{PYTHONPATH})
            EXECUTE_PROCESS(
                COMMAND ${PYTHON_EXECUTABLE} ${tofile}
                RESULT_VARIABLE import_res
                OUTPUT_VARIABLE import_output
                ERROR_VARIABLE  import_output
            )
  
            # Pass the output back to ctest
            IF(import_output)
                MESSAGE("\${import_output}")
            ENDIF(import_output)
            IF(import_res)
                MESSAGE(SEND_ERROR "\${import_res}")
            ENDIF(import_res)
        "
        )
        ADD_TEST(${test} ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/${test}_cmake.cmake)

    endforeach(pyfile)


ENDMACRO(ADD_PY_TEST)


