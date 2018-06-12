#include "PluginLed.h"

void PluginLed::setup(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String log = F("PluginLed Initialization.");
  CoreLog::add(LOG_LEVEL_INFO, log);
}

CorePlugins* PluginLed::factory(void) 
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  CorePlugins* plugin = new PluginLed(*this);
  //Serial.print( " setting pluginNumber=");
  //Serial.print( _pluginNumber );
  //plugin->pluginNumber(_pluginNumber);
  return plugin;
}

void PluginLed::webForm( String &html )
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String line;
  
  CorePlugins::webForm(html);
  
  // Select switch type
  line="";
  CoreHttp::select(line, "type");
  CoreHttp::option(line, "OnOff", 0);
  CoreHttp::option(line, "PWM", 1);
  CoreHttp::select(line);
  CoreHttp::tableLine(html, "Type", line);

  // Select switch pin
  line="";
  CoreHttp::select(line, "pin");  
  CoreHttp::option(line, "1", 1);
  CoreHttp::option(line, "3", 3);
  CoreHttp::option(line, "5", 5);
  CoreHttp::option(line, "7", 7);
  CoreHttp::select(line);
  CoreHttp::tableLine(html, "Pin", line);

  line="";
  CoreHttp::select(line, "inverse");
  CoreHttp::option(line, "No", 0);
  CoreHttp::option(line, "Yes", 1);
  CoreHttp::select(line);
  CoreHttp::tableLine(html, "Inverse", line);
  
  line="";
  CoreHttp::select(line, "bootstate");
  CoreHttp::option(line, "Off", 0);
  CoreHttp::option(line, "On", 1);
  CoreHttp::select(line);
  CoreHttp::tableLine(html, "Boot state", line);
}

void PluginLed::webSubmit( void )
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  __super::webSubmit();
  _type = atoi( WebServer.arg("type").begin() );
  _pin = atoi( WebServer.arg("pin").begin() );
  _inverse = atoi( WebServer.arg("inverse").begin() );
  _bootState = atoi( WebServer.arg("type").begin() );
}

void PluginLed::loopFast(void)
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
      String log = "Button change : ";
      log += this->_state;
      CoreLog::add(LOG_LEVEL_INFO, log);
  }
}

void PluginLed::loopMedium(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String log = F("PluginLed top");
  CoreLog::add(LOG_LEVEL_INFO, log);
  log = "@";
  log += (int)this;
  CoreLog::add(LOG_LEVEL_INFO, log);
}

PluginLed pluginLed("Led", "Handle led");

