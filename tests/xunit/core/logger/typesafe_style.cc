#include <sys/types.h>
#include <gtest/gtest.h>
#include <string>
#include <toolkit/logger.h>
#include "logger_test.h"

namespace ecsp { namespace test {

    

TEST_F(LoggerTest, typesafe_style)
{
    LogDeviceMemory *memdev = new LogDeviceMemory( "mem_device_typesafe" );
    EXPECT_TRUE( logger.add_device( memdev ) );
    EXPECT_TRUE( logger.create_tag( "MEM_TAG_TYPESAFE", 20 ) );
    EXPECT_TRUE( logger.set_tag_device( "MEM_TAG_TYPESAFE", "mem_device_typesafe" ) );

    memdev->clear_data();    

    std::string str = "string";
    log_print( 20, "%_ %_ %_ %_ %_ %_ %_ %_ %_ %_ %_", "Complex format", str, 3.1415926f, (void*)0, (std::string*)123, 666, -13, "-----", char(1), (unsigned int)-1 );

    std::string logStr = memdev->get_log_data();
#ifdef __GNUC__
    const std::string expectedStr = "< MEM_TAG_TYPESAFE > Complex format string 3.141593 (nil) 0x7b 666 -13 ----- 1 4294967295 %_\n";
#else
    const std::string expectedStr = "< MEM_TAG_TYPESAFE > Complex format string 3.141593 00000000 0000007B 666 -13 ----- 1 4294967295 %_\n";
#endif

//    fprintf(stderr,"logStr=%s\n", logStr.c_str());
//    fprintf(stderr,"expStr=%s\n", expectedStr.c_str());

    EXPECT_TRUE( expectedStr == logStr );
}

}}

