#ifndef WIN32
#include <syslog.h>
#endif
#include <gtest/gtest.h>
#include <toolkit/logger.h>
#include "logger_test.h"

namespace ecsp { namespace test {


#ifndef WIN32
const int32_t facilities[] = {
    LOG_AUTH,       // security/authorization messages (DEPRECATED Use LOG_AUTHPRIV instead)
    LOG_AUTHPRIV,   // security/authorization messages (private)
    LOG_CRON,       // clock daemon (cron and at)
    LOG_DAEMON,     // system daemons without separate facility value
    LOG_FTP,        // ftp daemon
    LOG_KERN,       // kernel messages (these can’t be generage from user processes)
    LOG_LOCAL0,  
    LOG_LOCAL1,  
    LOG_LOCAL2,  
    LOG_LOCAL3,  
    LOG_LOCAL4,  
    LOG_LOCAL5,  
    LOG_LOCAL6,  
    LOG_LOCAL7,     // reserved for local use
    LOG_LPR,        // line printer subsystem
    LOG_MAIL,       // mail subsystem
    LOG_NEWS,       // USENET news subsystem
    LOG_SYSLOG,     // messages generated internally by syslogd(8)
    LOG_USER,       // (default) generic user-level messages
    LOG_UUCP        // UUCP subsystem
};
#endif

TEST_F(LoggerTest, syslog_test)
{
#ifndef WIN32
    const int32_t numValidFacilities = sizeof(facilities)/sizeof(facilities[0]);

    LogDeviceSyslog *dev = new LogDeviceSyslog( "syslog_device" );
    EXPECT_TRUE( logger.add_device( dev ) );

    const LogTagID tagID = logger.get_max_tag_id()+1;
    EXPECT_TRUE( logger.create_tag( "SYSLOG_TAG", tagID ) );
    EXPECT_TRUE( logger.set_tag_device( "SYSLOG_TAG", "syslog_device" ) );

    for ( int32_t i = 0; i < numValidFacilities; ++i )
    {
        dev->set_facility( facilities[i] );
        LOG( tagID, "syslog message for facility %d", facilities[i] );
    }
#endif
}

}}

