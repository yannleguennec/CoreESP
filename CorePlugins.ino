#include "CorePlugins.h"

CorePlugins *plugin[ pluginMax ];
int pluginNb = 0;
int pluginNo = 0;

CorePlugins::CorePlugins(String pluginName, String pluginDesc)
{
    Serial.begin(74880);
  //#ifdef LOG_LEVEL_PANIC
    Serial.println(__PRETTY_FUNCTION__);
  //#endif
  this->_pluginName = pluginName;
  this->_pluginDesc = pluginDesc;
  this->registerPlugin();
};

void CorePlugins::registerPlugin(void)
{
//#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
//#endif
  // Store the plugin for later use
  this->pluginNumber( pluginNb + 1 );
  if (pluginNb < pluginMax)
    plugin[ pluginNb++ ] = this;
  else
  {
    String log = F("PLGS : Too much plugins (increase pluginMax)");
    CoreLog::add(LOG_LEVEL_ERROR, log);
  }
};

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

   CoreHttp::add( "/plugins", CorePlugins::listWeb );
   CoreCommands::add("plugins", CorePlugins::listCommand, "List plugins");
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
  log += pluginNb;
  log += F(" plugins : ");
  CoreLog::add(LOG_LEVEL_INFO, log);

  res += log;
  res += '\n';

  CorePlugins *plugin = CorePlugins::first();
  while ( plugin )
  {
    log = F("  ");
    log += plugin->toString();
    CoreLog::add(LOG_LEVEL_INFO, log);

    res += log;
    res += '\n';

    plugin = CorePlugins::next();
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
    CoreHttp::tableLine(reply, plugin->pluginName(), plugin->pluginDesc());
    plugin = CorePlugins::next();
  }

  reply += F("</table>");
  CoreHttp::pageFooter(reply);
  
  WebServer.send(200, texthtml, reply);
}

void CorePlugins::webMenu(String& html, int activeMenu)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  // Non utilisé pour le moment.
  CoreHttp::menuItem( html, "plugins",  "Plugins",     activeMenu);
}

void CorePlugins::webForm(String& html)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String line, form;

  form = "";
  CoreHttp::input( form, F("save"), F("yes"), F("hidden"));
  CoreHttp::input( form, F("topic"), _deviceTopic );
  CoreHttp::tableLine(html, F("Name"), form);

  form = "";
  CoreHttp::input( form, F("comment"), _deviceComment );
  CoreHttp::tableLine(html, F("Comment"), form);
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

CorePlugins* CorePlugins::first(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  pluginNo = 0;
  return plugin[ pluginNo ];
}

CorePlugins* CorePlugins::next(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  if ( ++pluginNo >= pluginNb)
    return NULL;
  return plugin[ pluginNo ];
}

