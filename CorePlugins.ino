#include "CorePlugins.h"

//std::vector<CorePlugins*> CorePlugins::pluginList;
uint CorePlugins::_pluginsNb = 0;

CorePlugins::CorePlugins(void) : CoreBase("CorePlugin")
{
  PANIC_DEBUG();

  setPriority( priorityPlugin );
  
  _saved = false;
}

CorePlugins::CorePlugins(const char* name, const char* desc) : CoreBase("CoreDevice")
{
  PANIC_DEBUG();

  setPriority( priorityDevice );

  this->_pluginName = name;
  this->_pluginDesc = desc;
  this->_pluginNumber = _pluginsNb++;

  addFlags( flagPlugin );
};

void CorePlugins::setup(void)
{
  PANIC_DEBUG();

#ifdef LOG_LEVEL_DEBUG
   String log =  "PLGS : Initialization";
   CoreLog::addLog(LOG_LEVEL_DEBUG, log);
#endif

   coreHttp.addUrl( "/plugins", CorePlugins::listWeb );
   CoreCommands::registerCommand("plugins", CorePlugins::listCommand, "List plugins");
}

void CorePlugins::loop(void)
{
//  PANIC_DEBUG();
}

CorePlugins* CorePlugins::factory() 
{
  PANIC_DEBUG();

  return NULL; 
};

void CorePlugins::listCommand(String &res, char **)
{
  PANIC_DEBUG();

  String log;
  log = _pluginsNb;
  log += F(" plugins : ");
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  res += log;
  res += '\n';

  for ( CoreBase* base : list )
    if (base->flags() & flagPlugin)
    {
      CorePlugins *plugin = reinterpret_cast<CorePlugins*>(base);
      log = F("  ");
      log += plugin->toString();
      log += '(';
      log += plugin->pluginNumber();
      log += ')';
      CoreLog::addLog(LOG_LEVEL_INFO, log);
  
      res += log;
      res += '\n';
    }
}

void CorePlugins::listWeb(void)
{
  PANIC_DEBUG();

#ifdef LOG_LEVEL_DEBUG
  String log = F("HTTP : GET /plugins");
  CoreLog::addLog(LOG_LEVEL_DEBUG, log);
#endif

  if (!coreHttp.isLoggedIn())
    return;

  String reply, line, res;

  coreHttp.pageHeader(reply, MENU_PLUGINS);
  reply += res;

  reply += F("<table class='table'>"); // Ca devrait être dans pageHeader si on fait toujours des tables

  line = F("Plugin list");
  coreHttp.tableHeader(reply, line);

  for ( CoreBase* base : list )
    if (base->flags() & flagPlugin)
    {
      CorePlugins* plugin = reinterpret_cast<CorePlugins*>(base);
      coreHttp.tableLine(reply, plugin->pluginName(), plugin->pluginDesc());
    }

  reply += F("</table>");
  coreHttp.pageFooter(reply);
  
  WebServer.send(200, texthtml, reply);
}

void CorePlugins::webMenu(String& html, int activeMenu)
{
  PANIC_DEBUG();

  // Non utilisé pour le moment.
  coreHttp.menuItem( html, "plugins",  "Plugins",     activeMenu);
}

void CorePlugins::webForm(String& html)
{
  PANIC_DEBUG();

  String line, form;

  form = "";
  coreHttp.input( form, F("save"), F("yes"), F("hidden"));
  coreHttp.input( form, F("topic"), _deviceTopic );
  coreHttp.tableLine(html, F("Name"), form);

  form = "";
  coreHttp.input( form, F("comment"), _deviceComment );
  coreHttp.tableLine(html, F("Comment"), form);
}

void CorePlugins::webSubmit( void )
{
  PANIC_DEBUG();

  _saved = WebServer.arg(F("save"));
  _deviceTopic = WebServer.arg(F("topic"));
  _deviceComment = WebServer.arg(F("comment"));
}

CorePlugins corePlugin;

