#include "CorePlugins.h"

std::array<CorePlugins*, pluginsMax> CorePlugins::plugins;
uint CorePlugins::pluginNb = 0;

CorePlugins::CorePlugins(String pluginName, String pluginDesc)
{
  //#ifdef LOG_LEVEL_PANIC
    Serial.println(__PRETTY_FUNCTION__);
  //#endif
  
  registerPlugin( pluginName, pluginDesc );
};

void CorePlugins::registerPlugin( String &pluginName, String &pluginDesc )
{
  _pluginName = pluginName;
  _pluginDesc = pluginDesc;
  _pluginNumber = pluginNb;
  plugins[pluginNb++]=this;
}

void CorePlugins::loopFast(void)
{
#ifdef LOG_LEVEL_PANIC
//  Serial.println(__PRETTY_FUNCTION__);
#endif
}

void CorePlugins::loopMedium(void)
{
#ifdef LOG_LEVEL_PANIC
//  Serial.println(__PRETTY_FUNCTION__);
#endif
}

void CorePlugins::loopSlow(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
}

void CorePlugins::setup(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
#ifdef LOG_LEVEL_DEBUG
   String log =  "PLGS : Initialization";
   CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif

   coreHttp.addUrl( "/plugins", CorePlugins::listWeb );
   coreCommand.addCommand("plugins", CorePlugins::listCommand, "List plugins");
}

CorePlugins* CorePlugins::factory() 
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  return NULL; 
};

void CorePlugins::listCommand(String &res, char **)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String log;
  log = corePlugins.size();
  log += F(" plugins : ");
  CoreLog::add(LOG_LEVEL_INFO, log);

  res += log;
  res += '\n';

  for (CorePlugins* plugin : corePlugins.plugins)
  {
    log = F("  ");
    log += plugin->toString();
    log += '(';
    log += plugin->pluginNumber();
    log += ')';
    CoreLog::add(LOG_LEVEL_INFO, log);

    res += log;
    res += '\n';
  }
}

void CorePlugins::listWeb(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
#ifdef LOG_LEVEL_DEBUG
  String log = F("HTTP : GET /plugins");
  CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif

  if (!coreHttp.isLoggedIn())
    return;

  String reply, line, res;

  coreHttp.pageHeader(reply, MENU_PLUGINS);
  reply += res;

  reply += F("<table class='table'>"); // Ca devrait être dans pageHeader si on fait toujours des tables

  line = F("Plugin list");
  coreHttp.tableHeader(reply, line);

  for (CorePlugins* plugin : plugins)
    coreHttp.tableLine(reply, plugin->pluginName(), plugin->pluginDesc());

  reply += F("</table>");
  coreHttp.pageFooter(reply);
  
  WebServer.send(200, texthtml, reply);
}

void CorePlugins::webMenu(String& html, int activeMenu)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  // Non utilisé pour le moment.
  coreHttp.menuItem( html, "plugins",  "Plugins",     activeMenu);
}

void CorePlugins::webForm(String& html)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
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
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  _saved = WebServer.arg(F("save"));
  _deviceTopic = WebServer.arg(F("topic"));
  _deviceComment = WebServer.arg(F("comment"));
}

CorePlugins corePlugins;
