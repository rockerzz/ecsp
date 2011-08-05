#ifndef __MOCK_INIT_TEST_H__
#define __MOCK_INIT_TEST_H__

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <mock_objects.h>

namespace ecsp { namespace test {

using ::testing::_;
using ::testing::Return;
using ::testing::An;
using ::testing::Invoke;
using ::testing::AllOf;
using ::testing::Property;
using ::testing::Eq;


Mock_DBManager* mock_dbmanager;
Mock_DBSession* mock_dbsession;
Mock_PluginManager* mock_pluginmanager;
Mock_ConnManager* mock_connmanager;


class MockInitTest
{
protected:
//    static Fake_DBSessionInternals fake_dbsessioninternals;
//    static Fake_PluginManagerInternals fake_pluginmanagerinternals;
//    static Fake_ConnManagerInternals fake_connmanagerinternals;

public:

    template< typename F_DBM, typename F_DBS >
    static void mock_SetUpDBManager(F_DBM* fake_dbmanager, F_DBS* fake_dbsession)
    {
        mock_dbmanager = new Mock_DBManager;
        mock_dbsession = new Mock_DBSession;

        ////////////////////////////////////////////////////////////////

        fake_dbmanager->setSession(mock_dbsession);

        ON_CALL(*mock_dbmanager, lock_session())
                .WillByDefault(Invoke(fake_dbmanager, &F_DBM::lock_session));
        ON_CALL(*mock_dbmanager, unlock_session(_))
                .WillByDefault(Invoke(fake_dbmanager, &F_DBM::unlock_session));
        ON_CALL(*mock_dbmanager, get_db_type())
                .WillByDefault(Invoke(fake_dbmanager, &F_DBM::get_db_type));
        ON_CALL(*mock_dbmanager, get_generator())
                .WillByDefault(Invoke(fake_dbmanager, &F_DBM::get_generator));
        ON_CALL(*mock_dbmanager, get_verifier())
                .WillByDefault(Invoke(fake_dbmanager, &F_DBM::get_verifier));


        ON_CALL(*mock_dbsession, unlock())
                .WillByDefault(Invoke(fake_dbsession, &F_DBS::unlock));
        ON_CALL(*mock_dbsession, begin_transaction())
                .WillByDefault(Invoke(fake_dbsession, &F_DBS::begin_transaction));
        ON_CALL(*mock_dbsession, commit_transaction())
                .WillByDefault(Invoke(fake_dbsession, &F_DBS::commit_transaction));
        ON_CALL(*mock_dbsession, rollback_transaction())
                .WillByDefault(Invoke(fake_dbsession, &F_DBS::rollback_transaction));
        ON_CALL(*mock_dbsession, exec(An<const db::Query&>()))
                .WillByDefault(Invoke(fake_dbsession, &F_DBS::exec_query_sql));
        ON_CALL(*mock_dbsession, exec(An<const char*>()))
                .WillByDefault(Invoke(fake_dbsession, &F_DBS::exec_str_sql));
        ON_CALL(*mock_dbsession, get_result(_, _))
                .WillByDefault(Invoke(fake_dbsession, &F_DBS::get_result));

        /////////////////////////////////////////////////////////////////

        Engine::instance()->set_db_manager( mock_dbmanager );
    }

    template< typename F_PM>
    static void mock_SetUpPluginManager(F_PM* fake_pluginmanager)
    {
        mock_pluginmanager = new Mock_PluginManager;

        /////////////////////////////////////////////////////////////////

        ON_CALL(*mock_pluginmanager, push_incoming(_))
                .WillByDefault(Invoke(fake_pluginmanager, &F_PM::push_incoming));
        ON_CALL(*mock_pluginmanager, push_outgoing(_))
                .WillByDefault(Invoke(fake_pluginmanager, &F_PM::push_outgoing));
        ON_CALL(*mock_pluginmanager, push_outgoing_from_plugin(_))
                .WillByDefault(Invoke(fake_pluginmanager, &F_PM::push_outgoing_from_plugin));
        ON_CALL(*mock_pluginmanager, call_plugin(_, _))
                .WillByDefault(Invoke(fake_pluginmanager, &F_PM::call_plugin));
        ON_CALL(*mock_pluginmanager, get_plugin_operations(_))
                .WillByDefault(Invoke(fake_pluginmanager, &F_PM::get_plugin_operations));
        ON_CALL(*mock_pluginmanager, is_plugin_loaded(_))
                .WillByDefault(Invoke(fake_pluginmanager, &F_PM::is_plugin_loaded));
        ON_CALL(*mock_pluginmanager, pop_outgoing(_))
                .WillByDefault(Invoke(fake_pluginmanager, &F_PM::pop_outgoing));
        ON_CALL(*mock_pluginmanager, update())
                .WillByDefault(Invoke(fake_pluginmanager, &F_PM::update));

        /////////////////////////////////////////////////////////////////

        Engine::instance()->set_plugin_manager( mock_pluginmanager );
    }

    template< typename F_CM >
    static void mock_SetUpConnManager(F_CM* fake_connmanager)
    {
        mock_connmanager = new Mock_ConnManager;

        /////////////////////////////////////////////////////////////////

        ON_CALL(*mock_connmanager, get_roles_by_sid(_))
                .WillByDefault(Invoke(fake_connmanager, &F_CM::get_roles_by_sid));
        ON_CALL(*mock_connmanager, start_processing())
                .WillByDefault(Invoke(fake_connmanager, &F_CM::start_processing));
        ON_CALL(*mock_connmanager, accept_connection(_, _))
                .WillByDefault(Invoke(fake_connmanager, &F_CM::accept_connection));
        ON_CALL(*mock_connmanager, deny_connection(_))
                .WillByDefault(Invoke(fake_connmanager, &F_CM::deny_connection));
        ON_CALL(*mock_connmanager, update())
                .WillByDefault(Invoke(fake_connmanager, &F_CM::update));

        /////////////////////////////////////////////////////////////////

        Engine::instance()->set_conn_manager( mock_connmanager );
    }

    static void mock_TearDownDBManager()
    {
        Engine::instance()->set_db_manager( NULL );
        delete mock_dbsession;
        delete mock_dbmanager;
    }

    static void mock_TearDownPluginManager()
    {
        Engine::instance()->set_plugin_manager( NULL );
        delete mock_pluginmanager;
    }

    static void mock_TearDownConnManager()
    {
        Engine::instance()->set_conn_manager( NULL );
        delete mock_connmanager;
    }
};

//Fake_DBSessionInternals MockInitTest::fake_dbsessioninternals;
//Fake_PluginManagerInternals MockInitTest::fake_pluginmanagerinternals;
//Fake_ConnManagerInternals MockInitTest::fake_connmanagerinternals;

}} // namespace

#endif

