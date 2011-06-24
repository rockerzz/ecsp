#ifndef ECSP_DEFINES_H_
#define ECSP_DEFINES_H_

#include "config.h"


// Opens definition of the ecsp namespace with sub-namespace
#define ECSP_NAMESPACE_BEGIN(sub_space) namespace ecsp { namespace sub_space {

// Closes definition of the ecsp namespace with sub-namespace
#define ECSP_NAMESPACE_END }}


#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
                      TypeName(const TypeName&);\
                      void operator=(const TypeName&)

#define DISALLOW_CREATE_AND_DELETE(TypeName) \
                      TypeName(); \
                      ~TypeName()

#define UNUSED(x) (void)(x)

//DLL export/import for windows platform

#ifndef ECSP_DECL_EXPORT
#   if defined(WIN32) 
#       define ECSP_DECL_EXPORT __declspec(dllexport)
#   endif
#   ifndef ECSP_DECL_EXPORT
#       define ECSP_DECL_EXPORT
#   endif
#endif
#ifndef ECSP_DECL_IMPORT
#   if defined(WIN32)
#       define ECSP_DECL_IMPORT __declspec(dllimport)
#   else
#       define ECSP_DECL_IMPORT
#   endif
#endif

#ifdef dbmanager_EXPORTS
#   define DB_MANAGER_API ECSP_DECL_EXPORT
#else
#   define DB_MANAGER_API ECSP_DECL_IMPORT
#endif
#ifdef acl_EXPORTS
#   define ACL_API ECSP_DECL_EXPORT
#else
#   define ACL_API ECSP_DECL_IMPORT
#endif
#ifdef api_EXPORTS
#   define PLUGIN_SDK_API ECSP_DECL_EXPORT
#else
#   define PLUGIN_SDK_API ECSP_DECL_IMPORT
#endif
//TODO ifndef DICT_GENERATOR_BUILD ??
#ifdef dict_EXPORTS
#   define DICT_API ECSP_DECL_EXPORT
#else
#   define DICT_API ECSP_DECL_IMPORT
#endif
#ifdef engine_EXPORTS
#   define ENGINE_API ECSP_DECL_EXPORT
#else
#   define ENGINE_API ECSP_DECL_IMPORT
#endif
#ifdef events_EXPORTS
#   define EVENTS_API ECSP_DECL_EXPORT
#else
#   define EVENTS_API ECSP_DECL_IMPORT
#endif
#ifdef platform_EXPORTS
#   define PLATFORM_API ECSP_DECL_EXPORT
#else
#   define PLATFORM_API ECSP_DECL_IMPORT
#endif
#ifdef toolkit_EXPORTS
#   define TOOLKIT_API ECSP_DECL_EXPORT
#else
#   define TOOLKIT_API ECSP_DECL_IMPORT
#endif

/// ecsp root namespace
namespace ecsp
{
    /// Namespace for ecsp interfaces
    namespace ecsp_interface {}

    /// Namespace for ecsp modules and parts of modules
    namespace module {}

    /// Namespace for ecsp plugins
    namespace plugin {}

    /// Namespace for plugin SDK API
    namespace api {}

    /// Namespace for ecsp tests
    namespace test {}

    /// Namespace for ecsp database subsystem
    namespace db {}

    /// Namespace for ACL subsystem
    namespace acl {}

}



#endif // ECSP_DEFINES_H_
