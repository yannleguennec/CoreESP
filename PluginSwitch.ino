#include "PluginSwitch.h"

void PluginSwitch::setup(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String log = F("PluginSwitch Initialization.");
  CoreLog::add(LOG_LEVEL_INFO, log);
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
  line="";
  coreHttp.select(line, "pin");  
  coreHttp.option(line, "1", 1);
  coreHttp.option(line, "3", 3);
  coreHttp.option(line, "5", 5);
  coreHttp.option(line, "7", 7);
  coreHttp.select(line);
  coreHttp.tableLine(html, "Pin", line);

  line="";
  coreHttp.select(line, "inverse");
  coreHttp.option(line, "No", 0);
  coreHttp.option(line, "Yes", 1);
  coreHttp.select(line);
  coreHttp.tableLine(html, "Inverse", line);
}

void PluginSwitch::webSubmit( void )
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  __super::webSubmit();
  _type = atoi( WebServer.arg("type").begin() );
  _pin = atoi( WebServer.arg("pin").begin() );
  _inverse = atoi( WebServer.arg("inverse").begin() );
}

void PluginSwitch::loopFast(void)
{
  static int lastButtonState = -1;
  static unsigned long lastDebounceTime = 0;
  int reading = digitalRead( this->_pin );

  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  #define debounceDelay 50
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
     if (reading != this->_state)
      this->_state = reading;
      String log = F("Button change : ");
      log += this->_state;
      CoreLog::add(LOG_LEVEL_INFO, log);
  }
}

void PluginSwitch::loopMedium(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String log = F("PluginSwitch top");
  CoreLog::add(LOG_LEVEL_INFO, log);
  
  log = '@';
  log += (int)this;
  CoreLog::add(LOG_LEVEL_INFO, log);
}

PluginSwitch pluginSwitch("Switch", "Handle input switch");

