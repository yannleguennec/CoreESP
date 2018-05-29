#include "PluginLed.h"

PluginLed::PluginLed(bool first) : 
  CorePlugins( "Led", "handle led", first )
{
  Serial.println("This is PluginSwitch");
}

void PluginLed::setup(void)
{
  String log = F("PluginLed Initialization.");
  CoreLog::add(LOG_LEVEL_INFO, log);
}

CorePlugins* PluginLed::factory(void) 
{
  CorePlugins* plugin = new PluginLed();
  plugin->num(_num);
  return plugin;
}

void PluginLed::webForm( String &html )
{
  String line;
  
  CorePlugins::webForm(html);
  
  String optionsType[] = { "OnOff", "PWM" };
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

void PluginLed::webFormSubmit( void )
{
  CorePlugins::webFormSubmit();
  _type = atoi( WebServer.arg("type").begin() );
  _pin = atoi( WebServer.arg("type").begin() );
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
  String log = F("PluginLed top");
  CoreLog::add(LOG_LEVEL_INFO, log);
  log = "@";
  log += (int)this;
  CoreLog::add(LOG_LEVEL_INFO, log);
}

PluginLed pluginLed(true);

