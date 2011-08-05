#ifndef LOGGER_TEST 

namespace ecsp { namespace test {

class LoggerTest: public testing::Test
{
protected:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp();
    virtual void TearDown();
};

}}

#endif


