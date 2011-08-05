#ifndef WIN32
#include <syslog.h>
#endif
#include <gtest/gtest.h>
#include <toolkit/logger.h>
#include "logger_test.h"

namespace ecsp { namespace test {

TEST_F(LoggerTest, rotation_test)
{
    LogDeviceFile *dev = new LogDeviceFile( "file_device_rtest" );
    EXPECT_TRUE( logger.add_device( dev ) );
    dev->setLogFilePath( "rtest.log" );

    const LogTagID tagID = logger.get_max_tag_id()+1;
    EXPECT_TRUE( logger.create_tag( "RTEST_TAG", tagID ) );
    EXPECT_TRUE( logger.set_tag_device( "RTEST_TAG", "file_device_rtest" ) );

    dev->set_rotation_threshold( 10*1024*1024 );
    for ( int32_t i = 0 ; i < 1000000; ++i )
    {
        LOG( tagID, "log rotation test string #%d", i );
    }
}

}}

