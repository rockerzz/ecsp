#ifndef TESTBASE_H
#define TESTBASE_H

#include <gtest/gtest.h>
#include <stdio.h>
#include <platform/platform.h>

namespace ecsp { namespace test {

class TestBase: public testing::Test
{
public:
    static std::string currentDir()
    {
        return get_executable_dir();
    }    
};

}} //namespace

#endif //TESTBASE_H
