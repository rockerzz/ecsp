#ifndef __MOCK_OBJECTS_H__
#define __MOCK_OBJECTS_H__

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <engine/engine.h>
#include <db/db_manager.h>
#include <engine/plugin_manager.h>
#include <engine/conn_manager.h>

namespace ecsp { namespace test {

class Mock_DBSession : public db::DBSession
{
public:
    Mock_DBSession() : db::DBSession(NULL){}
public:
    //MOCK_METHOD0(lock, bool());
    MOCK_METHOD0(unlock, bool());
    MOCK_METHOD0(begin_transaction, bool());
    MOCK_METHOD0(commit_transaction, bool());
    MOCK_METHOD0(rollback_transaction, bool());

    MOCK_METHOD1(exec, bool(const db::Query&)); // execSQL( query.toString() )
    MOCK_METHOD1(exec, bool(const char*));
    MOCK_METHOD2(get_result, bool( Event*, const db::DBResultLayout*));
};

////////////////////////////////////////////////////////////////////////////////////////////

class Mock_DBManager : public db::DBManager
{
public:
    Mock_DBManager() : db::DBManager( db::DBManagerSettings() ) 
    { 
    }

    MOCK_METHOD0(lock_session, db::DBSession*());
    MOCK_METHOD1(unlock_session, bool(db::DBSession*));
    MOCK_CONST_METHOD0(get_db_type, DBIndex());

    MOCK_METHOD0(get_generator, db::DbGenerator*());
    MOCK_METHOD0(get_verifier, db::Verifier*());
};

////////////////////////////////////////////////////////////////////////////////////////////

class Mock_PluginManager : public PluginManager
{
public:
    Mock_PluginManager() : PluginManager( PluginManagerSettings() )
    {
    }

    MOCK_METHOD0(update, int32_t());
    MOCK_METHOD1(pop_outgoing, bool(Event*));
    MOCK_METHOD1(push_incoming, bool(const Event&));
    MOCK_METHOD1(push_outgoing, bool(const Event&));
    MOCK_METHOD1(push_outgoing_from_plugin, bool(const Event&));
    MOCK_METHOD2(call_plugin, bool( Event*, const Event&));
    MOCK_METHOD1(get_plugin_operations, std::vector<PluginOperationDescr>(const NameID&));
    MOCK_METHOD1(is_plugin_loaded, bool(const NameID&));
};

class Mock_ConnManager : public ConnManager
{
public:
    Mock_ConnManager() : ConnManager( ConnManagerSettings() )
    {
    }

    MOCK_METHOD0(start_processing, void());
    MOCK_METHOD0(update, bool());

    MOCK_METHOD2(accept_connection, void( const Sid&, const std::vector<std::string>&));
    MOCK_METHOD1(deny_connection, void(const Sid&));
    MOCK_METHOD1(get_roles_by_sid, const std::vector<std::string>*(const Sid&));
};


}} //namespace
#endif

