#include <iostream>
#include <string>
#include <sstream>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <platform/platform.h>
#include <platform/hashmap.h>
#include <platform/threads.h>
#include <toolkit/logger.h>

#ifdef WIN32
#include <io.h>
#else
#include <syslog.h>
#endif

#ifndef WIN32
#define _open open
#define _close close
#define _write write
#define _O_CREAT O_CREAT
#define _O_BINARY 0x0
#define _O_RDWR O_RDWR
#define _O_TRUNC O_TRUNC
#define _S_IREAD S_IREAD 
#define _S_IWRITE S_IWRITE
#endif

namespace ecsp {

Logger logger;    

/////////////////////////////////////////////////////////////////////////////////////////    

int LOG( LogTagID tag, const char *fmt, ... )
{
    if ( !logger.is_tag_enabled( tag ) )
        return 0;

    va_list va;
    va_start(va,fmt);
    char buf[4096];

    int32_t tagLabelLen = 0;
    const char *tagLab = logger.get_tag_label( tagLabelLen, tag ); 
    memcpy( buf, tagLab, tagLabelLen ); 
    char *p = buf + tagLabelLen;    

    int32_t n = vsnprintf( p, sizeof(buf), fmt, va );
    buf[tagLabelLen+n]='\n';
    buf[tagLabelLen+n+1]=0;
    va_end(va);

    LogDevice *device = logger.get_device_by_tag( tag );

    device->print( buf, tagLabelLen+n+1 );

    return n;
}

/////////////////////////////////////////////////////////////////////////////////////////

LogDevice::LogDevice() :
    disabled(true),
    rotation_threshold(0),
    need_timestamp(false)        
{
}

LogDevice::LogDevice( const std::string &_name ) :
    name(_name),
    disabled(true),
    rotation_threshold(0),
    need_timestamp(false) 
{
}

LogDevice::~LogDevice()
{
}

void LogDevice::set_name( const std::string &newName )
{
    name = newName;
}

std::string LogDevice::get_name()
{
    return name;
}

void LogDevice::enable( bool e )
{
    disabled = !e;
}

bool LogDevice::is_enabled()
{
    return !disabled;
}

bool LogDevice::set_rotation_threshold( int64_t sz )
{
    rotation_threshold = sz;
    return true;
}

int64_t LogDevice::get_rotation_threshold()
{
    return rotation_threshold;
}

bool LogDevice::rotate()
{
    return false;
}

void LogDevice::get_metrics( LogMetrics *_metrics )
{
    *_metrics = metrics;
}

void LogDevice::reset_metrics()
{
    metrics = LogMetrics();
}

bool LogDevice::get_need_timestamp()
{
    return need_timestamp;
}

void LogDevice::set_need_timestamp( bool y )
{
    need_timestamp = y;
}

int32_t LogDevice::get_timestamp( char *ts )
{
    if ( ts )
        FormatTimestamp( ts, GetMachineTime() );
    
    return TIME_STAMP_CHARS_NUM;
}

/////////////////////////////////////////////////////////////////////////////////////////    

LogDeviceNull::LogDeviceNull( const std::string &name ) :
    LogDevice( name )
{
}

LogDeviceNull::~LogDeviceNull()
{
}

bool LogDeviceNull::print( const char * /*buf*/, int32_t bufLen )
{
    int32_t tsLen = get_need_timestamp() ? get_timestamp(NULL) : 0;
    metrics.bytes_written += bufLen + tsLen;
    metrics.bytes_written_total += bufLen + tsLen;
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

class DeviceMemory
{
public:
    std::ostringstream os;
};

LogDeviceMemory::LogDeviceMemory( const std::string &name ) :
    LogDevice( name )
{
    dev_memory = new DeviceMemory;
}
   
LogDeviceMemory::~LogDeviceMemory()
{
    if ( dev_memory )
        delete dev_memory;
}

bool LogDeviceMemory::print( const char *buf, int32_t bufLen )
{
    int32_t tsLen = 0;
    if ( get_need_timestamp() )
    {
        char tsBuf[TIME_STAMP_CHARS_NUM+1];
        tsLen = get_timestamp( tsBuf );
        dev_memory->os.write( tsBuf, tsLen );
    }
    dev_memory->os.write( buf, bufLen );
    metrics.bytes_written += bufLen + tsLen;
    metrics.bytes_written_total += bufLen + tsLen;
    return true;
}

std::string LogDeviceMemory::get_log_data()
{
    return dev_memory->os.str();
}

bool LogDeviceMemory::clear_data()
{
    dev_memory->os.str("");
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

LogDeviceConsole::LogDeviceConsole( const std::string &name ) :
    LogDevice( name )
{
    io_handle = IOH_STDERR;
}

LogDeviceConsole::~LogDeviceConsole()
{
}

bool LogDeviceConsole::print( const char *buf, int32_t bufLen )
{
    int32_t tsLen = 0;
    if ( get_need_timestamp() )
    {
        char tsBuf[TIME_STAMP_CHARS_NUM+1];
        tsLen = get_timestamp( tsBuf );
        _write( static_cast<int>(io_handle), tsBuf, tsLen );
    }
 
    _write( static_cast<int>(io_handle), buf, bufLen );
    metrics.bytes_written += bufLen + tsLen;
    metrics.bytes_written_total += bufLen + tsLen;
    return true;
}

LogDeviceConsole::StdIoHandle LogDeviceConsole::get_io_handle()
{
    return io_handle;
}

void LogDeviceConsole::set_io_handle( StdIoHandle _ioHandle )
{
    io_handle = _ioHandle;
}

/////////////////////////////////////////////////////////////////////////////////////////

class DeviceFile
{
    static const int32_t file_mode = _O_CREAT | _O_BINARY | _O_RDWR | _O_TRUNC;
    static const int32_t file_perm = _S_IREAD | _S_IWRITE;
public:    
    DeviceFile()
    {
        f = -1;
    }

    DeviceFile( const std::string &file_name )
    {
        f = _open( file_name.c_str(), file_mode, file_perm );
        if ( f == -1 )
        {
            fprintf( stderr, "error opening log-file: %s\n", file_name.c_str() );
            ecsp_exit( EXIT_FAILURE );
        }
    }

    ~DeviceFile()
    {
        if ( f != -1 )
            _close(f);
    }

    bool print( const char *buf, uint32_t sz, bool need_ts )
    {
        if ( f == -1 )
            return false;

        MtxLock lock( file_mutex );

        if ( need_ts )
        {
            char ts_buf[TIME_STAMP_CHARS_NUM+1];
            FormatTimestamp( ts_buf, GetMachineTime() );
            _write( f, ts_buf, TIME_STAMP_CHARS_NUM );
        }
 
        _write( f, buf, sz );

        return true;
    }

    void set_new_file( const std::string &new_file_name )
    {
        MtxLock lock( file_mutex );

        if ( f != -1 )
        {
            _close(f);
        }
        f = _open( new_file_name.c_str(), file_mode, file_perm );
    }

private:
    int32_t f;
    Mutex file_mutex;
};

LogDeviceFile::LogDeviceFile( const std::string &name ) :
    LogDevice( name )
{
    file = new DeviceFile();
}

LogDeviceFile::~LogDeviceFile()
{
    if ( file )
        delete file;
}

bool LogDeviceFile::print( const char *buf, int32_t bufLen )
{
    if ( (metrics.bytes_written + bufLen) > LogDevice::get_rotation_threshold() )
    {
        rotate();
        metrics.bytes_written = 0;
    }        
 
    if ( file->print( buf, bufLen, get_need_timestamp() ) )
    {
        ++metrics.print_count;
        metrics.bytes_written += bufLen + ( get_need_timestamp() ? TIME_STAMP_CHARS_NUM : 0 );
        metrics.bytes_written_total += bufLen + ( get_need_timestamp() ? TIME_STAMP_CHARS_NUM : 0 );
        return true;
    }
    else
    {
        return false;
    }
}

std::string LogDeviceFile::get_log_file_path()
{
    return log_file_path;
}

void LogDeviceFile::setLogFilePath( const std::string &path )
{
    if ( file )
        file->set_new_file( path );
    log_file_path = path;
}

bool LogDeviceFile::rotate()
{
    if ( LogDevice::get_rotation_threshold() == 0 )
        return false;

    ++metrics.rotations_count;

    char buf[100];
    sprintf( buf, ".%d", metrics.rotations_count );

    std::string fn = log_file_path;

    if ( metrics.rotations_count == 1 )
    {
        fn += buf; // abc.log --> abc.log.1 
    }
    else
    {
        size_t pidx = fn.rfind( '.' );
        assert( pidx > 0 || pidx < fn.length() );
        fn.replace( pidx, fn.length()-pidx, buf );
    }

    // fprintf( stderr, "Log rotation: %s --> %s\n", log_file_path.c_str(), fn.c_str() );
    log_file_path = fn;
    file->set_new_file( fn );

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

LogDeviceSyslog::LogDeviceSyslog( const std::string &name ) :
    LogDevice( name )
{
#ifndef WIN32
    set_facility( LOG_USER );
#endif
}

LogDeviceSyslog::~LogDeviceSyslog()
{
#ifndef WIN32
    closelog();
#endif
}

bool LogDeviceSyslog::print( const char *buf, int32_t bufLen )
{
#ifndef WIN32
    int32_t tsLen = 0;
    if ( get_need_timestamp() )
    {
        char tsBuf[TIME_STAMP_CHARS_NUM+1];
        tsLen = get_timestamp( tsBuf );
        syslog( 0, "%s", tsBuf );
    }
 
    syslog( 0, "%s", buf );
    metrics.bytes_written += bufLen;
    metrics.bytes_written_total += bufLen;
#endif
    return true;
}

int32_t LogDeviceSyslog::get_facility()
{
    return facility;
}

void LogDeviceSyslog::set_facility( int32_t newFacility )
{
#ifndef WIN32
    closelog();

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

    bool invalidFacility = true;
    for ( uint32_t i = 0; i < sizeof(facilities)/sizeof(facilities[0]); ++i )
    {
        if ( newFacility == facilities[i] )
        {
            invalidFacility = false;
            break;
        } 
    }

    if ( invalidFacility )
    {
        fprintf( stderr, "LogDeviceSyslog -- invalid facility %d\n", newFacility );
        closelog();
        return;
    }
    else
    {
        facility = newFacility;
        openlog( "ecsp", 0, facility );
    }
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////

class LoggerInternals 
{
public:
    LogDevice* defaultDevice;
    hash_map<std::string,LogDevice*> devices;

    enum { MIN_TAG_LENGTH=2, MAX_TAG_LENGTH=30 };

    struct TagDescr
    {
        std::string name;
        int32_t index;
        bool disabled;
        LogDevice *bindedDevice;
        bool inUse;
        std::string label;

        TagDescr() { index = -1; disabled = true; bindedDevice = 0; inUse = false; }
    };  
    std::vector<TagDescr> tags;
    hash_map<std::string,int32_t> tagsMap;

    bool sysMessages;
};

/////////////////////////////////////////////////////////////////////////////////////////

Logger::Logger()
{
    internals = new LoggerInternals;
    internals->sysMessages = false;
    LogDevice *def_dev = new LogDeviceConsole( "default_console" );
    add_device( def_dev );
    internals->defaultDevice = def_dev;

    create_tag( "INFO", T_INFO );
    create_tag( "WARN", T_WARN );
    create_tag( "ERROR", T_ERROR );
    create_tag( "FATAL_ERROR", T_FATAL_ERROR );
    create_tag( "DEBUG", T_DEBUG );
    create_tag( "DEBUG_L1", T_DEBUG_L1 );
    create_tag( "DEBUG_L2", T_DEBUG_L2 );
    create_tag( "DEBUG_L3", T_DEBUG_L3 );
}

Logger::~Logger()
{
    if ( internals->defaultDevice )
        internals->defaultDevice = NULL;

    hash_map<std::string,LogDevice*>::iterator it = internals->devices.begin();
    for ( ; it != internals->devices.end(); ++it )
    {
        delete it->second;
    }
    internals->devices.clear();

    delete internals;
}

LogDevice* Logger::get_device_by_tag( LogTagID tag )
{
    uint32_t idx = static_cast<uint32_t>(tag);
    if ( idx >= internals->tags.size() )
        return internals->defaultDevice;

    if ( LogDevice *device = internals->tags[idx].bindedDevice )
        return device;
    else
        return internals->defaultDevice;
}

bool Logger::is_tag_enabled( LogTagID tag )
{
    uint32_t idx = static_cast<uint32_t>(tag);
    if ( idx >= internals->tags.size() )
    {
        return false;
    }
    else
    {
        return !internals->tags[idx].disabled;
    }
}

const char* Logger::get_tag_label( int32_t &tagLabelLen, LogTagID tag )
{
    uint32_t idx = static_cast<uint32_t>(tag);
    if ( idx >= internals->tags.size() )
    {
        static const char unkn[] = "<UNKN> ";
        tagLabelLen = sizeof(unkn)-1;
        return unkn;
    }

    tagLabelLen = internals->tags[idx].label.length();
    return internals->tags[idx].label.c_str();
}

LogDevice* Logger::get_device_by_name( const std::string &name )
{
    hash_map<std::string,LogDevice*>::iterator it = internals->devices.find(name);
    if ( it == internals->devices.end() )
        return 0;
    else
        return it->second;
}

bool Logger::add_device( LogDevice *dev )
{
    if ( !dev )
        return false;

    std::string newName = dev->get_name();

    hash_map<std::string,LogDevice*>::iterator it = internals->devices.find( newName );
    if ( it != internals->devices.end() )
    {
        if ( internals->sysMessages )
        {
            fprintf( stderr, "Logger: can't add device with name \'%s\' - name is already in use\n", newName.c_str() );
        }
        return false;
    }
    else
    {
        internals->devices[newName] = dev;
        return true;
    }
}

bool Logger::delete_device( const std::string &name )
{
    hash_map<std::string,LogDevice*>::iterator it = internals->devices.find(name);
    if ( it == internals->devices.end() )
    {
        if ( internals->sysMessages )
        {
            fprintf( stderr, "Logger: can't delete device with name \'%s\' - name not found\n", name.c_str() );
        }
        return false;
    }

    LogDevice *dev = it->second;
    
    // для тегов которые используют данное устройство утановить устройство по-умолчанию
    for ( uint32_t i = 0; i < internals->tags.size(); ++i )
    {
        LoggerInternals::TagDescr &td = internals->tags[i];
        if ( td.bindedDevice == dev )
            td.bindedDevice = internals->defaultDevice;
    }

    // удалить устройство 
    internals->devices.erase( it );
    delete dev;

    return true;
}

bool Logger::create_tag( const std::string &tagName, LogTagID id )
{
    if (    tagName.length() < LoggerInternals::MIN_TAG_LENGTH || 
            tagName.length() > LoggerInternals::MAX_TAG_LENGTH )
    {
        if ( internals->sysMessages )
        {
            fprintf( stderr, "Logger: Invalid tag name\n" );
        }
        return false;
    }

    hash_map<std::string,int32_t>::iterator it = internals->tagsMap.find( tagName );
    if ( it != internals->tagsMap.end() )
    {
        if ( internals->sysMessages )
        {
            fprintf( stderr, "Logger: can't create new tag - name in use\n" );
        }
        return false;
    }

    uint32_t idx = static_cast<uint32_t>(id);
    if ( idx < internals->tags.size() )
    {
        if ( internals->tags[idx].inUse )
        {
            if ( internals->sysMessages )
            {
                fprintf( stderr, "Logger: can't create new tag - invalid ID\n" );
            }
            return false;
        }
    }
    else
    {
        internals->tags.resize( idx+1 );
    }

    LoggerInternals::TagDescr &td = internals->tags[idx];
    td.inUse = true;
    td.name = tagName;
    td.label = "< " + tagName + " > ";
    td.index = idx;
    td.disabled = false;
    td.bindedDevice = NULL;

    internals->tagsMap[tagName] = idx;

    return true;
}

bool Logger::enable_tag( const std::string &tagName, bool e )
{
    hash_map<std::string,int32_t>::iterator it = internals->tagsMap.find( tagName );
    if ( it == internals->tagsMap.end() )
    {
        if ( internals->sysMessages )
        {
            fprintf( stderr, "Logger: can't enable/disable tag - tag name not found\n" );
        }
        return false;
    }
    else
    {
        int32_t idx = it->second;
        internals->tags[idx].disabled = !e;
        return true;
    }
}

std::string Logger::get_tag_device_name( const std::string &tagName )
{
    hash_map<std::string,int32_t>::iterator it = internals->tagsMap.find( tagName );
    if ( it == internals->tagsMap.end() )
    {
        if ( internals->sysMessages )
        {
            fprintf( stderr, "Logger: can't find tag by name\n" );
        }
        return "";
    }
    else
    {
        int32_t idx = it->second;
        if ( internals->tags[idx].bindedDevice == 0 )
        {
            if ( internals->sysMessages )
            {
                fprintf( stderr, "Logger: tag device is NULL\n" );
            }
            return "";
        }
        else
        {
            return internals->tags[idx].bindedDevice->get_name();
        }
    }
}

bool Logger::set_tag_device( const std::string &tagName, const std::string &newDeviceName )
{
    LogDevice* dev = get_device_by_name( newDeviceName );
    if ( !dev )
    {
        if ( internals->sysMessages )
        {
            fprintf( stderr, "Logger: device \'%s\' not found\n", newDeviceName.c_str() );
        }
        return false;
    }

    hash_map<std::string,int32_t>::iterator it = internals->tagsMap.find( tagName );
    if ( it == internals->tagsMap.end() )
    {
        if ( internals->sysMessages )
        {
            fprintf( stderr, "Logger: can't find tag by name\n" );
        }
        return false;
    }
    else
    {
        int idx = it->second;
        internals->tags[idx].bindedDevice = dev;
        return true;
    }
}

LogDevice* Logger::get_default_device()
{
    return internals->defaultDevice;
}

bool Logger::set_default_device( LogDevice *dev )
{
    if ( internals->sysMessages )
    {
        fprintf( stderr, "Logger: set new default device '%s'\n", dev->get_name().c_str() );
    }
    internals->defaultDevice = dev;
    return true;
}

LogTagID Logger::get_max_tag_id()
{
    return internals->tags.size();
}

/////////////////////////////////////////////////////////////////////////////////////////

void Logger::enable_internal_messages( bool e )
{
    internals->sysMessages = e;
}

/////////////////////////////////////////////////////////////////////////////////////////

} // namespace ecsp

