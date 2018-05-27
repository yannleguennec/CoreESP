#include "PluginLed.h"

PluginLed::PluginLed(bool first) : 
  CorePlugins( "Led", "handle led", first )
{
  Serial.println("This is PluginSwitch");
}

void PluginLed::setup(void)
{
  String log = F("PluginSwitch Initialization.");
  CoreLog::add(LOG_LEVEL_INFO, log);
}

CorePlugins* PluginLed::factory(void) 
{
  return new PluginLed();
}

void PluginLed::webForm( String &res )
{
  String optionsType[] = { F("Switch"), F("Dimmer"), "" };
  String optionsPin[] = { "1", "3", "5", "7", "" };
  String html;
  int optionNo;

  // Choose name
  html = "";
  CoreHttp::input(res, "name", "thename");
  CoreHttp::tableLine(res, "Name", html);

  // Select switch type
  CoreHttp::select(html, "type");
  html = "";
  optionNo=0;
  while ( optionsType[ optionNo ] != "" )
    CoreHttp::option(html, optionsType[ optionNo ], optionNo);
  CoreHttp::select(html);
  CoreHttp::tableLine(res, "Type", html);

  // Select switch pin
  CoreHttp::select(html, "pin");
  html = "";
  optionNo=0;
  while ( optionsPin[ optionNo ] != "" )
    CoreHttp::option(html, optionsPin[ optionNo ], optionNo);
  CoreHttp::select(html);
  CoreHttp::tableLine(res, "Pin", html);
}

void PluginLed::loopFast(void)
{
  static int lastButtonState = -1;
  static unsigned long lastDebounceTime = 0;
  int reading = digitalRead( this->pin );

  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  #define debounceDelay 50
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
     if (reading != this->state)
      this->state = reading;
      String log = "Button change : ";
      log += this->state;
      CoreLog::add(LOG_LEVEL_INFO, log);
  }
}

void PluginLed::loopMedium(void)
{
  String log = F("PluginSwitch top");
  CoreLog::add(LOG_LEVEL_INFO, log);
  log = "@";
  log += (int)this;
  CoreLog::add(LOG_LEVEL_INFO, log);
}

PluginLed pluginLed(true);

