#include "PluginSwitch.h"

//int PluginSwitch::states( void )
//{
//#ifdef LOG_LEVEL_PANIC
//  Serial.println(__PRETTY_FUNCTION__);
//#endif
//  return pluginStates;
//}

//void PluginSwitch::states( int states )
//{
//#ifdef LOG_LEVEL_PANIC
//  Serial.println(__PRETTY_FUNCTION__);
//#endif
//  pluginStates = states;
//}

void PluginSwitch::setup(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String log = F("PluginSwitch Initialization.");
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  __super::setup();
}

void PluginSwitch::loop(void)
{
  debounce();
  if ( buttonPressed() )
  {
    switchState++;
    switchState %= switchStates;

    String msg = deviceName();
    msg += '=';
    msg += switchState;
  }
}

CorePlugins* PluginSwitch::factory(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  CorePlugins* plugin = new PluginSwitch(*this);
  Serial.print( " setting pluginNumber=");
  Serial.print( _pluginNumber );

  plugin->pluginNumber(_pluginNumber); // Set the plugin number to the new device
  return plugin;
}

void PluginSwitch::webForm( String &html )
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String line;

  CorePlugins::webForm(html);

  // Select switch type
  line = "";
  coreHttp.select(line, "type");
  coreHttp.option(line, "Switch", 0);
  coreHttp.option(line, "Dimmer", 1);
  coreHttp.select(line);
  coreHttp.tableLine(html, "Type", line);

  // Select switch pin
  line = "";
  coreHttp.select(line, "pin");
  coreHttp.option(line, "1", 1);
  coreHttp.option(line, "3", 3);
  coreHttp.option(line, "5", 5);
  coreHttp.option(line, "7", 7);
  coreHttp.select(line);
  coreHttp.tableLine(html, "Pin", line);

  line = "";
  coreHttp.select(line, "inverse");
  coreHttp.option(line, "No", 0);
  coreHttp.option(line, "Yes", 1);
  coreHttp.select(line);
  coreHttp.tableLine(html, "Inverse", line);
}

void PluginSwitch::webSubmit( void )
{
  PANIC_DEBUG();

  __super::webSubmit();

  switchStates = atoi( WebServer.arg("states").begin() );
}

PluginSwitch pluginSwitch("Switch", "Handle input switch");

