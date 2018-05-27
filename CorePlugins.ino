#include "CorePlugins.h"

#define pluginMax 20
CorePlugins *plugin[ pluginMax ];
int pluginNb = 0;
int pluginNo = 0;

CorePlugins::CorePlugins(bool first)
{
  if (first)
  {
    // Store the plugin for later use
    this->num( pluginNb + 1 );
    if (pluginNb < pluginMax)
      plugin[ pluginNb++ ] = this;
  }
};

CorePlugins::CorePlugins(const char *name, const char *desc, bool first) :
  CorePlugins(first)
{
  this->_name = name;
  this->_desc = desc;
};

void CorePlugins::loopFast(void)
{
  String log =  "Plugin::loopFast(void)";
  CoreLog::add(LOG_LEVEL_INFO, log);
}

void CorePlugins::loopMedium(void)
{
  String log =  "Plugin::loopMedium(void)";
  CoreLog::add(LOG_LEVEL_INFO, log);
}

void CorePlugins::loopSlow(void)
{
  String log =  "Plugin::loopSlow(void)";
  CoreLog::add(LOG_LEVEL_INFO, log);
}

int CorePlugins::num( int num )
{
  if (num) this->_num = num;
  return this->_num;
};

String* CorePlugins::name( String* name )
{
  if (name) this->_name = *name;
  return &this->_name;
};

String* CorePlugins::desc( String* desc ) 
{
  if (desc) this->_desc = *desc;
  return &this->_desc;
};

void CorePlugins::setup(void)
{
   String log =  "CorePlugins::setup(void)";
   CoreLog::add(LOG_LEVEL_DEBUG, log);

   CoreHttp::add( "/plugins", CorePlugins::webPlugins );
   CoreCommands::add("plugins", CorePlugins::listPlugins, "List plugins");
}

CorePlugins* CorePlugins::factory() { return NULL; };

void CorePlugins::listPlugins(String &res, char **)
{
  String log =  "";
  log += pluginNb;
  log += " plugins : ";
  CoreLog::add(LOG_LEVEL_INFO, log);

  res += log;
  res += "\n";

  CorePlugins *plugin = CorePlugins::first();
  while ( plugin )
  {
    log = "  ";
    log += *plugin->name();
    log += " : ";
    log += *plugin->desc();
    CoreLog::add(LOG_LEVEL_INFO, log);

    res += log;
    res += "\n";

    plugin = CorePlugins::next();
  }
}

void CorePlugins::webPlugins(void)
{
#ifdef LOG_LEVEL_DEBUG
  String log = F("HTTP : GET /plugins");
  CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif

  if (!CoreHttp::isLoggedIn())
    return;

  String reply, line, res;

  CoreHttp::pageHeader(reply, MENU_PLUGINS);
  reply += res;

  reply += F("<table class='table'>"); // Ca devrait être dans pageHeader si on fait toujours des tables

  line = F("Plugin list");
  CoreHttp::tableHeader(reply, line);

  CorePlugins *plugin = CorePlugins::first();
  while (plugin)
  {
    CoreHttp::tableLine(reply, *plugin->name(), *plugin->desc());
    plugin = CorePlugins::next();
  }

  reply += F("</table>");
  CoreHttp::pageFooter(reply);
  
  WebServer.send(200, texthtml, reply);
}

CorePlugins* CorePlugins::first(void)
{
  pluginNo = 0;
  return plugin[ pluginNo ];
}

CorePlugins* CorePlugins::next(void)
{
  if ( ++pluginNo >= pluginNb)
    return NULL;
  return plugin[ pluginNo ];
}

