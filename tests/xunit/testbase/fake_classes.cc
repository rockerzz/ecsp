#include <platform/hashmap.h>
#include <events/events.h>
#include <acl/acl_types.h>
#include <dict/sys_names.h>

#include <algorithm>
#include <functional>
#include <ostream>

#include "fake_classes.h"

namespace ecsp { namespace test {

Fake_DBSessionInternals::Fake_DBSessionInternals() : mode (BY_QUERY_STR)
{
}

bool Fake_DBSessionInternals::exec_query_sql( const db::Query& query )
{
    return exec_str_sql(query.to_string().c_str());
}

bool Fake_DBSessionInternals::exec_str_sql(const char* query)
{
    lastQuery = query;
    removeSpaces(lastQuery);
    return true;
}

bool Fake_DBSessionInternals::get_result( Event *res, const db::DBResultLayout * /*dbrLayout*/ )
{
    if(mode == BY_QUERY_STR)
    {
        ByQueryStorIter findIter = byQuery.find(lastQuery);
        if(findIter != byQuery.end())
        {
            *res = findIter->second;
            return true;
        }
    }

    else if(mode == BY_ORDER)
    {
        if(!byOrder.empty())
        {
            *res = byOrder.front();
            byOrder.pop_front();
            return true;
        }
    }

    return false;
}

void Fake_DBSessionInternals::setQueryStrAndResult(std::string queryStr, Event res)
{
    removeSpaces(queryStr);

    byQuery[queryStr] = Event(); // очистка
    byQuery[queryStr] = res;
}

void Fake_DBSessionInternals::setResultOrder(std::deque<Event> results)
{
    byOrder.swap(results);
}

void Fake_DBSessionInternals::removeSpaces(std::string& str)
{
    std::string result;

    std::string::iterator spaceRangeBegin = str.begin();
    std::string::iterator spaceRangeEnd = spaceRangeBegin;
    while(spaceRangeBegin != str.end())
    {
        spaceRangeEnd = std::find(spaceRangeBegin, str.end(), ' ');
        if(spaceRangeEnd != str.end())
            ++spaceRangeEnd; // + символ ' ';

        result.insert(result.end(), spaceRangeBegin, spaceRangeEnd);

        // пропускаем пробелы, идущие подряд
        spaceRangeBegin = std::find_if(spaceRangeEnd, str.end(), std::bind2nd(std::not_equal_to<char>(), ' '));
    }

    str = result;
}
//////////////////////////////////////////////////////////////////////////////////////

const std::vector<std::string>* Fake_ConnManagerInternals::get_roles_by_sid( const Sid &sid )
{
    SidWithRolesIter findIter = sidWithRoles.find(sid);
    if(findIter != sidWithRoles.end())
        return &findIter->second;
    return NULL;
}

void Fake_ConnManagerInternals::setRolesForSid(const Sid& sid, const std::vector<std::string>& roles)
{
    sidWithRoles[sid] = roles;
}

//////////////////////////////////////////////////////////////////////////////////////

bool Fake_PluginManagerInternals::push_incoming( const Event &evt )
{
    incoming.push_back(evt);
    return true;
}

bool Fake_PluginManagerInternals::push_outgoing( const Event &evt )
{
    outgoing.push_back(evt);
    return true;
}

bool Fake_PluginManagerInternals::push_outgoing_from_plugin( const Event &evt )
{
    outgoing.push_back(evt);
    return true;
}

bool Fake_PluginManagerInternals::call_plugin(Event* event_out, const Event& /*event_in*/)
{
    if(outputData.empty())
        return false;
    *event_out = outputData.front();
    outputData.pop_front();
    return true;
}

std::vector<PluginOperationDescr> Fake_PluginManagerInternals::get_plugin_operations( const NameID& pluginName )
{
    PluginsDataIter findIter = pluginsData.find(pluginName);
    if(findIter != pluginsData.end())
        return findIter->second;
    return std::vector<PluginOperationDescr>();
}

bool Fake_PluginManagerInternals::is_plugin_loaded( const NameID &pluginName )
{
    return pluginsData.find(pluginName) != pluginsData.end();
}

bool Fake_PluginManagerInternals::pop_outgoing( Event *e )
{
    if ( outgoing.empty() )
        return false;
    *e = outgoing.front();
    outgoing.pop_front();
    return true;
}

//----------------------------------------

void Fake_PluginManagerInternals::setPluginOperation(NameID pluginName, const std::vector<PluginOperationDescr>& pluginOperations)
{
    pluginsData[pluginName] = pluginOperations;
}

void Fake_PluginManagerInternals::setCallPluginResult(std::deque<Event> newOutputData)
{
    outputData.swap(newOutputData);
}






}} //namespace
