#include "PluginLed.h"

void PluginLed::setup(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String log = F("PluginLed Initialization.");
  CoreLog::add(LOG_LEVEL_INFO, log);

  coreMqtt.registerCallback( "led1", PluginLed::callback );
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

void PluginLed::callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Led received a message [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
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
  coreHttp.select(line, "type");
  coreHttp.option(line, "OnOff", 0);
  coreHttp.option(line, "PWM", 1);
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
  
  line="";
  coreHttp.select(line, "bootstate");
  coreHttp.option(line, "Off", 0);
  coreHttp.option(line, "On", 1);
  coreHttp.select(line);
  coreHttp.tableLine(html, "Boot state", line);
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

