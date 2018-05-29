#include "CorePlugins.h"

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

void CorePlugins::setup(void)
{
   String log =  "CorePlugins::setup(void)";
   CoreLog::add(LOG_LEVEL_DEBUG, log);

   CoreHttp::add( "/plugins", CorePlugins::listWeb );
   CoreCommands::add("plugins", CorePlugins::listCommand, "List plugins");
}

CorePlugins* CorePlugins::factory() { return NULL; };

void CorePlugins::listCommand(String &res, char **)
{
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
    CoreHttp::tableLine(reply, plugin->name(), plugin->desc());
    plugin = CorePlugins::next();
  }

  reply += F("</table>");
  CoreHttp::pageFooter(reply);
  
  WebServer.send(200, texthtml, reply);
}

void CorePlugins::webForm(String& html)
{
  String line, form;
  line = F("Topic");
  form = F("<input type='text' name='topic' value=\"");
  form += _topic;
  form += F("\">");
  CoreHttp::tableLine(html, line, form);
  
  line = F("Comment");
  form = F("<input type='text' name='comment' value=\"");
  form += _comment;
  form += F("\">");
  CoreHttp::tableLine(html, line, form);
}

void CorePlugins::webFormSubmit( void )
{
  _topic = WebServer.arg(F("topic"));
  _comment = WebServer.arg(F("comment"));
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

