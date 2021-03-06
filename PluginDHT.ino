#include "PluginDHT.h"

void PluginDHT::setup(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String log = F("PluginDHT Initialization.");
  CoreLog::addLog(LOG_LEVEL_INFO, log);
}

void PluginDHT::loop(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
}

CorePlugins* PluginDHT::factory(void) 
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  CorePlugins* plugin = new PluginDHT(*this);
  Serial.print( " setting pluginNumber=");
  Serial.print( _pluginNumber );
  plugin->pluginNumber(_pluginNumber);
  return plugin;
}

void PluginDHT::webForm( String &html )
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String line;
  
  CorePlugins::webForm(html);
  
  // Select switch type
  line="";
  coreHttp.select(line, "degres");
  coreHttp.option(line, "Centigrade", 0);
  coreHttp.option(line, "Fahrenheit", 1);
  coreHttp.select(line);
  coreHttp.tableLine(html, "Degrés", line);

  // Select switch pin
  line="";
  coreHttp.select(line, "pin");  
  coreHttp.option(line, "1", 1);
  coreHttp.option(line, "3", 3);
  coreHttp.option(line, "5", 5);
  coreHttp.option(line, "7", 7);
  coreHttp.select(line);
  coreHttp.tableLine(html, "Pin", line);
}

void PluginDHT::webSubmit( void )
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  __super::webSubmit();
  _degree = atoi( WebServer.arg("degree").begin() );
  _pin = atoi( WebServer.arg("pin").begin() );
}

PluginDHT pluginDHT("Temp and Moisture (DHT)", "Handle DHT devices");

