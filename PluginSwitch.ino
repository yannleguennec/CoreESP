#include "PluginSwitch.h"

PluginSwitch::PluginSwitch(bool first) : 
  CorePlugins( "Switch", "handle input switch", first )
{
  Serial.println("This is PluginSwitch");
}

void PluginSwitch::setup(void)
{
  String log = F("PluginSwitch Initialization.");
  CoreLog::add(LOG_LEVEL_INFO, log);
}

CorePlugins* PluginSwitch::factory(void)
{
  CorePlugins* plugin = new PluginSwitch();
  plugin->num(_num); // Set new device number to actuel plugin number
  return plugin;
}

void PluginSwitch::webForm( String &html )
{
  String line;
  
  CorePlugins::webForm(html);
  
  String optionsType[] = { "Switch", "Dimmer" };
  String optionsPin[] = { "1", "3", "5", "7"};
  int optionNo;

  // Select switch type
  line = "";
  CoreHttp::select(line, "type");
  for( int optionNo=0; optionNo< sizeof(optionsType) / sizeof(String); optionNo++ )
    CoreHttp::option(line, optionsType[ optionNo ], optionNo);
  CoreHttp::select(line);
  CoreHttp::tableLine(html, "Type", line);

  // Select switch pin
  CoreHttp::select(line, "pin");
  line = "";
  optionNo=0;
  while ( optionsPin[ optionNo ] != "" )
    CoreHttp::option(line, optionsPin[ optionNo ], optionNo);
  CoreHttp::select(line);
  CoreHttp::tableLine(html, "Pin", line);

  line="";
  CoreHttp::select(line, "state");
  CoreHttp::option(line, "Off", 0);
  CoreHttp::option(line, "On", 1);
  CoreHttp::select(line);
  CoreHttp::tableLine(html, "Boot state", line);
}

void PluginSwitch::webFormSubmit( void )
{
  CorePlugins::webFormSubmit();
  _type = atoi( WebServer.arg("type").begin() );
  _pin = atoi( WebServer.arg("type").begin() );
  _bootState = atoi( WebServer.arg("type").begin() );
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
  String log = F("PluginSwitch top");
  CoreLog::add(LOG_LEVEL_INFO, log);
  
  log = '@';
  log += (int)this;
  CoreLog::add(LOG_LEVEL_INFO, log);
}

PluginSwitch pluginSwitch(true);

