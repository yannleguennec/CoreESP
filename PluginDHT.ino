#include "PluginDHT.h"

void PluginDHT::setup(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String log = F("PluginDHT Initialization.");
  CoreLog::add(LOG_LEVEL_INFO, log);
}

CorePlugins* PluginDHT::factory(void) 
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  CorePlugins* plugin = new PluginDHT();
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
  CoreHttp::select(line, "degres");
  CoreHttp::option(line, "Centigrade", 0);
  CoreHttp::option(line, "Fahrenheit", 1);
  CoreHttp::select(line);
  CoreHttp::tableLine(html, "Degrés", line);

  // Select switch pin
  line="";
  CoreHttp::select(line, "pin");  
  CoreHttp::option(line, "1", 1);
  CoreHttp::option(line, "3", 3);
  CoreHttp::option(line, "5", 5);
  CoreHttp::option(line, "7", 7);
  CoreHttp::select(line);
  CoreHttp::tableLine(html, "Pin", line);
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

void PluginDHT::loopFast(void)
{
}

void PluginDHT::loopMedium(void)
{
#ifdef LOG_LEVEL_PANIC
//  Serial.println(__PRETTY_FUNCTION__);
#endif
}

PluginDHT pluginDHT("Temp and Moisture (DHT)", "Handle DHT devices");

