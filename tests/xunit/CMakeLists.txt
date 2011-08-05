include(MacroAddTest)

include_directories(${CMAKE_SOURCE_DIR}/testsuite/xunit/testbase )
include_directories(${CMAKE_SOURCE_DIR}/src/plugins )

subdirs(testbase)

ADD_CXX_TEST(logger PATH core LIBRARIES platform toolkit)
