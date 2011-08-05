#ifndef __FAKE_CLASSES_H__
#define __FAKE_CLASSES_H__

#include <platform/platform.h>
#include <db/db_manager.h>
#include <engine/plugin_manager.h>
#include <map>
#include <vector>
#include <deque>
#include <assert.h>

#include <mock_objects.h>

namespace ecsp { namespace test {

// для имен плагинов выбраны зарегистрированные значения,
// чтоб ф-ция NameIDToStr(pluginName) выдавала корректные значения
//const NameID loadedPlugin = SYS_COLUMN_0;
//const NameID unloadedPlugin = SYS_COLUMN_1;
//const Sid whiteListPlugin = SYS_COLUMN_2;
//
//const Sid testSid = 11;
//const Sid secondSid = 22;
//const Sid unauthSid = 33;
//
//
//const NameID plugin_update = 15; // позиция в маске доступа - 0.
//const NameID plugin_delete = 16; // 1
//const NameID plugin_insert = 17; // 2
//const NameID plugin_error_oper_type = 18; // "несуществующий" тип операции


//////////////////////////////////////////////////////////////////////////////////

class Fake_DBSessionInternals // реализуется только необходимая часть методов из DBSessionInternals
{
public:
    enum ResultMode {BY_QUERY_STR, BY_ORDER};

public:
    Fake_DBSessionInternals();

public:
    bool unlock() { return true; }
    bool begin_transaction() { return true; }
    bool commit_transaction() { return true; }
    bool rollback_transaction() { return true; }

    bool exec_query_sql( const db::Query& query );
    bool exec_str_sql( const char* query);
    bool get_result( Event *res, const db::DBResultLayout *dbrLayout = NULL );

public:
    void setQueryStrAndResult(std::string queryStr, Event res);
    void setResultOrder(std::deque<Event> results);

    void setMode(ResultMode newMode) { mode = newMode; }

private:
    void removeSpaces(std::string& str);

private:
    std::string lastQuery;

    // режим выбора результата по строке запроса
    typedef std::map<std::string, Event> ByQueryStor;
    typedef ByQueryStor::iterator ByQueryStorIter;

    ByQueryStor byQuery;

    // режим возвращения значения по очереди 
    std::deque<Event> byOrder;

private:
    ResultMode mode;
};

//////////////////////////////////////////////////////////////////////////////////

class Fake_DBManagerInternals
{
public:
    Fake_DBManagerInternals() : sess(NULL), index(DBIndex(-1)), gen(NULL), verif(NULL) { }

public:
    db::DBSession* lock_session() { assert(sess); return sess; }
    bool unlock_session( db::DBSession * /*session*/ ) { return true; }
    DBIndex get_db_type() const { return index; }

    db::DbGenerator* get_generator() { assert(gen); return gen; }
    db::Verifier*    get_verifier() { assert(verif); return verif; }

public:
    void set_generator(db::DbGenerator* newGen) { gen = newGen; }
    void set_verifier(db::Verifier* newVerif) { verif = newVerif; }
    void setDBIndex(DBIndex newIndex) { index = newIndex; }

    // В тестах не надо вызывать ее НИКОГДА! Для служебного пользования
    void setSession(Mock_DBSession* newSession) { sess = newSession; }
private:
    Mock_DBSession* sess;
    DBIndex index;

    db::DbGenerator* gen;
    db::Verifier* verif;

};

//////////////////////////////////////////////////////////////////////////////////

class Fake_ConnManagerInternals
{
public:
    const std::vector<std::string>* get_roles_by_sid( const Sid &sid );

    void start_processing() { }
    void accept_connection( const Sid &/*sid*/, const std::vector<std::string> &/*roles*/ ) { }
    void deny_connection( const Sid &/*sid*/ ) { }
    bool update() { return true; }

public:
    void setRolesForSid(const Sid& sid, const std::vector<std::string>& roles);

private:
    std::map<Sid, std::vector<std::string> > sidWithRoles;
    typedef std::map<Sid, std::vector<std::string> >::iterator SidWithRolesIter;

};

//////////////////////////////////////////////////////////////////////////////////

class Fake_PluginManagerInternals
{
public:
    typedef std::deque<Event> QueueType;
    typedef QueueType::iterator QueueIter;

public:
    Fake_PluginManagerInternals()
    {
        setPluginOperation(SYS_AUTHENTICATE, std::vector<PluginOperationDescr>());
    }

public:
    bool push_incoming( const Event &evt );
    bool push_outgoing( const Event &evt );
    bool push_outgoing_from_plugin( const Event &evt );
    bool call_plugin( Event *eventOut, const Event &eventIn );
    std::vector<PluginOperationDescr> get_plugin_operations( const NameID& pluginName );
    bool is_plugin_loaded( const NameID &pluginName );
    bool pop_outgoing( Event *e );

    int32_t update(){ return 1;}


// для задания значений и просто нужные ф-ции
public:
    // помещает во внутреннее хранилище имя плагина и допустимые операции для него.
    void setPluginOperation(NameID pluginName, const std::vector<PluginOperationDescr>& pluginOperations);

    // задает список значений, котрые callPlugin будет возвращать в eventOut по очереди при каждом вызове
    void setCallPluginResult(std::deque<Event> newOutputData);

    //------------------------------------------------------------

    // возвращает входящую очередь
    QueueType& getIncomingQueue() { return incoming; }

    // возвращает исходящую очередь
    QueueType& getOutgoingQueue() { return outgoing; }

    void clearIncoming(){ incoming.clear();}
    void clearOutgoing(){ outgoing.clear();}
    void clearAll(){ clearIncoming(); clearOutgoing(); }

private:
    // типы операции для плагина
    std::map<NameID, std::vector<PluginOperationDescr> > pluginsData;
    typedef std::map<NameID, std::vector<PluginOperationDescr> >::iterator PluginsDataIter;

    std::deque<Event> outputData; // для callPlugin(...)

    // очереди входящих и исходящих эвентов
    QueueType incoming;
    QueueType outgoing;
};



}} //namespace


#endif

