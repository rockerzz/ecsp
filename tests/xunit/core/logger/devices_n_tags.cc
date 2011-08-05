#include  <gtest/gtest.h>
#include  <toolkit/logger.h>
#include "logger_test.h"

namespace ecsp { namespace test {

TEST_F(LoggerTest, two_tags)
{
    logger.enable_internal_messages( false );

    LogDeviceFile *dev_0 = new LogDeviceFile( "default_device" );
    dev_0->setLogFilePath( "default_unkn.log" );
    logger.set_default_device( dev_0 );

    LogDeviceFile *dev_1 = new LogDeviceFile( "file_device_1" );
    dev_1->setLogFilePath( "file_1.log" );

    LogDeviceFile *dev_2 = new LogDeviceFile( "file_device_2" );
    dev_2->setLogFilePath( "file_2.log" );

    EXPECT_TRUE( logger.add_device( dev_1 ) );
    EXPECT_TRUE( logger.add_device( dev_2 ) );
    EXPECT_FALSE( logger.delete_device( "file_device_100" ) );

    EXPECT_TRUE( logger.create_tag( "TAG_1", 100 ) );
    EXPECT_TRUE( logger.create_tag( "TAG_2", 110) );
    EXPECT_FALSE( logger.create_tag( "TAG_2", 110) );
    EXPECT_FALSE( logger.create_tag( "TAG_3", 100 ) );

    EXPECT_TRUE( logger.set_tag_device( "TAG_2", "file_device_2" ) );
    EXPECT_TRUE( logger.set_tag_device( "TAG_1", "file_device_1" ) );
    EXPECT_FALSE( logger.set_tag_device( "TAG_2", "file_device_100" ) );

    log_print( 100, "test of TAG_1" );
    log_print( 110, "test of TAG_2" );
    log_print( 110, "test of TAG_2" );
    log_print( 100, "test of TAG_1" );

    log_print( 100, "Yes, this is log message with invalid tag\n" );

    EXPECT_TRUE( logger.delete_device( "file_device_1" ) );
    log_print( 100, "file_1.log is closed now\n" );

    logger.enable_internal_messages( true );
}

}} //namespace

