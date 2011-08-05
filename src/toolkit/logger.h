#ifndef ECSP_PLATFORM_LOGGER_H_
#define ECSP_PLATFORM_LOGGER_H_

#include <platform/platform.h>
#include <platform/converter.h>

namespace ecsp {

/////////////////////////////////////////////////////////////////////////////////////////

const int32_t LOG_BUF_SIZE = 2000;

typedef int32_t LogTagID;

/////////////////////////////////////////////////////////////////////////////////////////

const LogTagID T_INFO                = 1;    
const LogTagID T_WARN                = 2;
const LogTagID T_ERROR               = 3;
const LogTagID T_FATAL_ERROR         = 4;
const LogTagID T_DEBUG               = 5;
const LogTagID T_DEBUG_L1            = 6;
const LogTagID T_DEBUG_L2            = 7;
const LogTagID T_DEBUG_L3            = 8;

TOOLKIT_API int32_t LOG( LogTagID tag, const char *fmt, ... );

/////////////////////////////////////////////////////////////////////////////////////////

struct TOOLKIT_API LogMetrics
{
    int64_t print_count;
    float   operation_time;
    int64_t bytes_written_total;
    int64_t bytes_written;
    int32_t rotations_count;

    LogMetrics() : print_count(0), operation_time(0),
        bytes_written_total(0), bytes_written(0), rotations_count(0)
    {}
};

class TOOLKIT_API LogDevice
{
public:
    LogDevice();
    LogDevice( const std::string &name );
    virtual ~LogDevice();
    void set_name( const std::string &newName );
    std::string get_name();
    virtual void enable( bool e );
    virtual bool is_enabled();
    virtual bool set_rotation_threshold( int64_t sz );
    virtual int64_t get_rotation_threshold();
    virtual bool rotate();
    virtual bool print( const char *buf, int32_t bufLen ) = 0;
    void get_metrics( LogMetrics *metrics );
    void reset_metrics();
    bool get_need_timestamp();
    void set_need_timestamp( bool y );
    int32_t get_timestamp( char *ts );
private:
    std::string name;
    bool disabled;    
    int64_t rotation_threshold;
    bool need_timestamp;
protected:
    LogMetrics metrics;
};

/////////////////////////////////////////////////////////////////////////////////////////

class TOOLKIT_API LogDeviceNull : public LogDevice
{
public:
    LogDeviceNull( const std::string &name );
    virtual ~LogDeviceNull();
    virtual bool print( const char *buf, int32_t buf_len );
};

/////////////////////////////////////////////////////////////////////////////////////////

class DeviceMemory;
class TOOLKIT_API LogDeviceMemory : public LogDevice
{
public:
    LogDeviceMemory( const std::string &name );
    virtual ~LogDeviceMemory();
    virtual bool print( const char *buf, int32_t bufLen );
    std::string get_log_data();
    bool clear_data();
private:
    DeviceMemory *dev_memory;
};

/////////////////////////////////////////////////////////////////////////////////////////

class TOOLKIT_API LogDeviceConsole : public LogDevice
{
public:
    enum StdIoHandle
    {
        IOH_STDOUT = 1,
        IOH_STDERR = 2
    };

    LogDeviceConsole( const std::string &name );
    virtual ~LogDeviceConsole();
    virtual bool print( const char *buf, int32_t buf_len );
    StdIoHandle get_io_handle();
    void set_io_handle( StdIoHandle io_handle );
private:
    StdIoHandle io_handle;
};

/////////////////////////////////////////////////////////////////////////////////////////

class DeviceFile;

class TOOLKIT_API LogDeviceFile : public LogDevice
{
public:
    LogDeviceFile( const std::string &name );
    virtual ~LogDeviceFile();
    virtual bool print( const char *buf, int32_t buf_len );
    std::string get_log_file_path();
    void setLogFilePath( const std::string &path );
    virtual bool rotate();
private:
    std::string log_file_path;
    DeviceFile *file;
};

/////////////////////////////////////////////////////////////////////////////////////////

class TOOLKIT_API LogDeviceSyslog : public LogDevice
{
public:
    LogDeviceSyslog( const std::string &name );
    virtual ~LogDeviceSyslog();
    virtual bool print( const char *buf, int32_t buf_len );
    int32_t get_facility();
    void set_facility( int32_t new_facility );
private:
    int32_t facility;
};

/////////////////////////////////////////////////////////////////////////////////////////
class LoggerInternals;

class TOOLKIT_API Logger
{
public:
    Logger();
    ~Logger();

    LogDevice* get_device_by_tag( LogTagID tag );
    bool is_tag_enabled( LogTagID tag );
    const char* get_tag_label( int32_t &tag_label_len, LogTagID tag );
    LogDevice* get_device_by_name( const std::string &name );

    bool add_device( LogDevice *dev );
    bool delete_device( const std::string &name );

    bool create_tag( const std::string &tag_name, LogTagID id );
    bool enable_tag( const std::string &tag_name, bool e );
    std::string get_tag_device_name( const std::string &tag_name );
    bool set_tag_device( const std::string &tag_name, const std::string &new_device_name );

    void enable_internal_messages( bool e );

    LogDevice* get_default_device();
    bool set_default_device( LogDevice *dev );

    LogTagID get_max_tag_id();

private:
    LoggerInternals *internals;
};

TOOLKIT_API extern Logger logger;

inline int32_t log_print_format0( int32_t &, char *buf, int32_t /*rest*/, const char *fmt )
{
    int32_t len = strlen(fmt);
    for ( int32_t i = 0; i < len; ++i )
        buf[i] = fmt[i];

    int32_t num_out = len;
    return num_out;
}

template<class A> inline
int32_t log_print_format_a( int32_t &fmt_pos, char *buf, int32_t rest, const char *fmt, A a )
{
    char *p = buf;
    int32_t len = strlen(fmt);
    int32_t i = 0;
    for ( ; i < len; ++i )
    {
        char c0 = fmt[i];
        if ( c0 == '%' && i != len-1 )
        {
            char c1 = fmt[i+1];
            if ( c1 == '_' )
            {
                i += 2;
                rest -= 2;
                int32_t n = CvtToString( p, rest, a );
                p += n;
                rest -= n;
                break;
            }
        }
        else
        {
            *p = c0;
            ++p;
            --rest;
        }        
    }    

    fmt_pos = i;

    return p-buf;
}

#define LOGPRINT_VARS               \
    char buf[LOG_BUF_SIZE];         \
    int32_t rest = sizeof(buf);     \
    int32_t fmt_pos = -1;           \
    int32_t num_out;                \
    const char *fmt_p = fmt;        \
    int32_t tag_label_len = 0;      \
    const char *tag_lab = logger.get_tag_label( tag_label_len, tag ); \
    memcpy( buf, tag_lab, tag_label_len ); \
    char *p = buf + tag_label_len;    \
    rest -= tag_label_len;            \
    int32_t nn = tag_label_len;

#define LOGPRINT_EPILOG                                   \
    num_out = log_print_format0( fmt_pos, p, rest, fmt_p ); \
    nn += num_out;                                        \
                                                          \
    LogDevice *device = logger.get_device_by_tag( tag );     \
    buf[nn]='\n';                                         \
    buf[++nn]=0;                                          \
    if ( !logger.is_tag_enabled( tag ) )                    \
    return nn;                                            \
    device->print( buf, nn );                             \
    return nn;

#define LOGPRINT_PROCESS_ARG(arg)                                \
    num_out  = log_print_format_a( fmt_pos, p, rest, fmt_p, (arg) );\
    fmt_p += fmt_pos;                                            \
    p += num_out;                                                \
    rest -= num_out;                                             \
    nn += num_out;

inline
int32_t log_print( LogTagID tag, const char *fmt )
{
    LOGPRINT_VARS
    LOGPRINT_EPILOG
}

template<class A1> inline
int32_t log_print( LogTagID tag, const char *fmt, A1 a1 )
{
    LOGPRINT_VARS
    LOGPRINT_PROCESS_ARG(a1)
    LOGPRINT_EPILOG
}

template<class A1,class A2> inline
int32_t log_print( LogTagID tag, const char *fmt, A1 a1, A2 a2 )
{
    LOGPRINT_VARS
    LOGPRINT_PROCESS_ARG(a1)
    LOGPRINT_PROCESS_ARG(a2)
    LOGPRINT_EPILOG
}

template<class A1,class A2,class A3> inline
int32_t log_print( LogTagID tag, const char *fmt, A1 a1, A2 a2, A3 a3 )
{
    LOGPRINT_VARS
    LOGPRINT_PROCESS_ARG(a1)
    LOGPRINT_PROCESS_ARG(a2)
    LOGPRINT_PROCESS_ARG(a3)
    LOGPRINT_EPILOG
}

template<class A1,class A2,class A3,class A4> inline
int32_t log_print( LogTagID tag, const char *fmt, A1 a1, A2 a2, A3 a3, A4 a4 )
{
    LOGPRINT_VARS
    LOGPRINT_PROCESS_ARG(a1)
    LOGPRINT_PROCESS_ARG(a2)
    LOGPRINT_PROCESS_ARG(a3)
    LOGPRINT_PROCESS_ARG(a4)
    LOGPRINT_EPILOG
}

template<class A1,class A2,class A3,class A4,class A5> inline
int32_t log_print( LogTagID tag, const char *fmt, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 )
{
    LOGPRINT_VARS
    LOGPRINT_PROCESS_ARG(a1)
    LOGPRINT_PROCESS_ARG(a2)
    LOGPRINT_PROCESS_ARG(a3)
    LOGPRINT_PROCESS_ARG(a4)
    LOGPRINT_PROCESS_ARG(a5)
    LOGPRINT_EPILOG
}

template<class A1,class A2,class A3,class A4,class A5,class A6> inline
int32_t log_print( LogTagID tag, const char *fmt, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6 )
{
    LOGPRINT_VARS
    LOGPRINT_PROCESS_ARG(a1)
    LOGPRINT_PROCESS_ARG(a2)
    LOGPRINT_PROCESS_ARG(a3)
    LOGPRINT_PROCESS_ARG(a4)
    LOGPRINT_PROCESS_ARG(a5)
    LOGPRINT_PROCESS_ARG(a6)
    LOGPRINT_EPILOG
}

template<class A1,class A2,class A3,class A4,class A5,class A6,class A7> inline
int32_t log_print( LogTagID tag, const char *fmt, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7 )
{
    LOGPRINT_VARS
    LOGPRINT_PROCESS_ARG(a1)
    LOGPRINT_PROCESS_ARG(a2)
    LOGPRINT_PROCESS_ARG(a3)
    LOGPRINT_PROCESS_ARG(a4)
    LOGPRINT_PROCESS_ARG(a5)
    LOGPRINT_PROCESS_ARG(a6)
    LOGPRINT_PROCESS_ARG(a7)
    LOGPRINT_EPILOG
}

template<class A1,class A2,class A3,class A4,class A5,class A6,class A7,class A8> inline
int32_t log_print( LogTagID tag, const char *fmt, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8 )
{
    LOGPRINT_VARS
    LOGPRINT_PROCESS_ARG(a1)
    LOGPRINT_PROCESS_ARG(a2)
    LOGPRINT_PROCESS_ARG(a3)
    LOGPRINT_PROCESS_ARG(a4)
    LOGPRINT_PROCESS_ARG(a5)
    LOGPRINT_PROCESS_ARG(a6)
    LOGPRINT_PROCESS_ARG(a7)
    LOGPRINT_PROCESS_ARG(a8)
    LOGPRINT_EPILOG
}

template<class A1,class A2,class A3,class A4,class A5,class A6,class A7,class A8,class A9> inline
int32_t log_print( LogTagID tag, const char *fmt, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9 )
{
    LOGPRINT_VARS
    LOGPRINT_PROCESS_ARG(a1)
    LOGPRINT_PROCESS_ARG(a2)
    LOGPRINT_PROCESS_ARG(a3)
    LOGPRINT_PROCESS_ARG(a4)
    LOGPRINT_PROCESS_ARG(a5)
    LOGPRINT_PROCESS_ARG(a6)
    LOGPRINT_PROCESS_ARG(a7)
    LOGPRINT_PROCESS_ARG(a8)
    LOGPRINT_PROCESS_ARG(a9)
    LOGPRINT_EPILOG
}

template<class A1,class A2,class A3,class A4,class A5,class A6,class A7,class A8,class A9,class A10> inline
int32_t log_print( LogTagID tag, const char *fmt, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10 )
{
    LOGPRINT_VARS
    LOGPRINT_PROCESS_ARG(a1)
    LOGPRINT_PROCESS_ARG(a2)
    LOGPRINT_PROCESS_ARG(a3)
    LOGPRINT_PROCESS_ARG(a4)
    LOGPRINT_PROCESS_ARG(a5)
    LOGPRINT_PROCESS_ARG(a6)
    LOGPRINT_PROCESS_ARG(a7)
    LOGPRINT_PROCESS_ARG(a8)
    LOGPRINT_PROCESS_ARG(a9)
    LOGPRINT_PROCESS_ARG(a10)
    LOGPRINT_EPILOG
}

} // namespace ecsp

#endif // ECSP_PLATFORM_LOGGER_H_


