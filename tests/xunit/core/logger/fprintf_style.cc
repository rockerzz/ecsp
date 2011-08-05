#include <gtest/gtest.h>
#include <toolkit/logger.h>
#include "logger_test.h"

namespace ecsp { namespace test {

TEST_F(LoggerTest, fprintf_style)
{
    LogDeviceMemory *memdev = new LogDeviceMemory( "mem_device_fprintf" );
    EXPECT_TRUE( logger.add_device( memdev ) );
    EXPECT_TRUE( logger.create_tag( "MEM_TAG_FPRINTF", 15 ) );
    EXPECT_TRUE( logger.set_tag_device( "MEM_TAG_FPRINTF", "mem_device_fprintf" ) );

    memdev->clear_data();    

    LOG( 15, "$\n" 
            "%s\n" 
            "%d %.1f %s", "$$", 10, 13.14, "$$$" );

    std::string logStr = memdev->get_log_data();
    const std::string expectedStr = "< MEM_TAG_FPRINTF > $\n$$\n10 13.1 $$$\n";

//    fprintf(stderr,"logStr=%s\n", logStr.c_str());
//    fprintf(stderr,"expStr=%s\n", expectedStr.c_str());


    EXPECT_TRUE( expectedStr == logStr );
}

}}

