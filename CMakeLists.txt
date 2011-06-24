cmake_minimum_required(VERSION 2.6)

#-----------------------------------------------------------------------------
# We don't allow in-source builds. This causes no end of troubles because.
# all out-of-source builds will use the CMakeCache.txt file there and even.
# build the libs and objects in it.

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
message(FATAL_ERROR "CMake generation is not allowed within the source directory!
Remove the CMakeCache.txt file and try again from another folder, e.g.:
    rm CMakeCache.txt
    mkdir build
    cd build
    cmake ..
")
endif(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

project(ecsp)

#-----------------------------------------------------------------------------
# Read version

file(READ "${CMAKE_SOURCE_DIR}/VERSION" VERSION)
string(REPLACE "\n" "" VERSION ${VERSION})
string(REPLACE "\r" "" VERSION ${VERSION})
string(REPLACE "." "," VERSION_WIN ${VERSION})
string(REGEX REPLACE "^([0-9]*)\\.([0-9]*)\\.([0-9]*)" "\\1" VERSION_MAJOR ${VERSION})
string(REGEX REPLACE "^([0-9]*)\\.([0-9]*)\\.([0-9]*)" "\\2" VERSION_MINOR ${VERSION})
string(REGEX REPLACE "^([0-9]*)\\.([0-9]*)\\.([0-9]*)" "\\3" VERSION_PATCH ${VERSION})

if (CMAKE_BUILD_TYPE)
    set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} 
            CACHE STRING "Configuration types" FORCE )
else()
    set(CMAKE_BUILD_TYPE "Debug" 
            CACHE STRING "Build type" FORCE )
    set(CMAKE_CONFIGURATION_TYPES "Debug" 
            CACHE STRING "Configuration types" FORCE )
endif(CMAKE_BUILD_TYPE)
message("CMAKE_CONFIGURATION_TYPES=${CMAKE_CONFIGURATION_TYPES}")

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

include_directories("${CMAKE_SOURCE_DIR}/src")

mark_as_advanced(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

IF(WIN32 AND NOT UNIX)
    add_definitions("-D_CRT_SECURE_NO_WARNINGS /W3 /wd4290 /wd4251 /Zc:wchar_t-")
ELSE()
   list(APPEND CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Wno-long-long -ansi -Wno-deprecated")
ENDIF(WIN32 AND NOT UNIX)

#-----------------------------------------------------------------------------
# Set default config options

option(WITH_COVERAGE        "Build with coverage support" OFF)
option(WITH_PROFILER        "Use profiler" OFF)
option(WITH_TESTS           "Enable tests" ON)
option(WITH_DOXYGEN         "Generate doxygen documentation" ON)
option(WITH_PYTHON          "Prepare python libs" ON)
option(WITH_PGSQL           "Build with PostgreSQL support" ON)
option(WITH_MYSQL           "Build with MySQL support" ON)
option(WITH_INCLUDES        "Add additional header files" ON)
option(WITH_3RDPARTY        "Add 3rdparty libraries" ON)


if(WITH_COVERAGE)
    add_definitions( "-fprofile-arcs -ftest-coverage" )
    set(CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage -lgcov")
    set(CMAKE_MODULE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage -lgcov")
    set(CMAKE_SHARED_LINKER_FLAGS "-fprofile-arcs -ftest-coverage -lgcov")
endif(WITH_COVERAGE)


if(WITH_PROFILER)
    add_definitions("-pg")
    set(CMAKE_EXE_LINKER_FLAGS "-pg")
    set(CMAKE_MODULE_LINKER_FLAGS "-pg")
    set(CMAKE_SHARED_LINKER_FLAGS "-pg")
endif(WITH_PROFILER)

if(WITH_TESTS)
    include(CTest)
    enable_testing()
    subdirs(testsuite)
endif(WITH_TESTS)

if(WITH_DOXYGEN)
    subdirs(docs)
endif(WITH_DOXYGEN)


#-----------------------------------------------------------------------------
# Find packages 

include(MacroEnsureVersion)

if(WITH_TESTS)
    find_package(GoogleTest REQUIRED)
    find_package(GoogleMock REQUIRED)
endif(WITH_TESTS)

find_package(PythonLibrary REQUIRED)
set(NEEDED_PYTHON_VERSION "2.6")
MACRO_ENSURE_VERSION_RANGE( ${NEEDED_PYTHON_VERSION} ${PYTHON_SHORT_VERSION} ${NEEDED_PYTHON_VERSION} PYTHON_VERSION_OK)
if(PYTHON_VERSION_OK)
else(PYTHON_VERSION_OK)
    message(FATAL_ERROR " Python version missmatch: you have ${PYTHON_SHORT_VERSION} but you need ${NEEDED_PYTHON_VERSION}")
endif(PYTHON_VERSION_OK)

find_package(LibEvent REQUIRED)

if (NOT UNIX)
   set(Boost_USE_STATIC_LIBS   ON)
   set(Boost_USE_MULTITHREADED ON)
   set(BOOST_ROOT "c:/prj/libs/boost_1_42_0")
endif(NOT UNIX)
find_package(Boost COMPONENTS python REQUIRED)

if(WITH_PGSQL)
    find_package(Postgres)
endif(WITH_PGSQL)

if(WITH_MYSQL)
    find_package(MySQL)
endif(WITH_MYSQL)

include_directories(${CMAKE_SOURCE_DIR}/include 
                    ${CMAKE_BINARY_DIR}/include)
include_directories(SYSTEM ${CMAKE_PREFIX_PATH}/include)

#-----------------------------------------------------------------------------
# Set source dirs
# solutions subdirectory must be processed first to generate plugins list

subdirs(solutions src tools)

#-----------------------------------------------------------------------------
# Test for missing types

include(CheckTypeSize)

set(CMAKE_EXTRA_INCLUDE_FILES stdint.h)
check_type_size(int64_t HAVE_INT64_T)
check_type_size(int32_t INT32_T)
check_type_size("long long" SIZEOF_LONGLONG)
check_type_size("double" SIZEOF_DOUBLE)
check_type_size(long SIZEOF_LONG)
check_type_size(int SIZEOF_INT)
check_type_size(intptr_t INTPTR_T)

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/include/config.h)


#-----------------------------------------------------------------------------
# Packaging support

include(CPackConfig)

