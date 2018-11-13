#include "PluginLed.h"

void PluginLed::setup(void)
{
  PANIC_DEBUG();

  String log = F("PluginLed Initialization.");
  CoreLog::addLog(LOG_LEVEL_INFO, log);

// note :  a-t-on besoin de faire un subscribe ?
// Il suffirait que le moteur Mqtt recherche dans tous les objets celui qui a le bon nom
// et qu'il appelle alors une methode précise pour le prévenir de l'arrivée d'un message.
// Cela permettrait d'utiliser de la même faćon CoreCommands !!!

  //coreMqtt.subscribe( "led1", this );
}

CorePlugins* PluginLed::factory(void) 
{
  PANIC_DEBUG();
  PluginLed *led = new PluginLed(*this);
//  coreMqtt.subscribe( this->deviceName, this );

  return led;
}

void PluginLed::callback(char* topic, byte* payload, unsigned int length)
{
  PANIC_DEBUG();
  
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
  PANIC_DEBUG();

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
  PANIC_DEBUG();

  __super::webSubmit();
  _type = atoi( WebServer.arg("type").begin() );
  _pin = atoi( WebServer.arg("pin").begin() );
  _inverse = atoi( WebServer.arg("inverse").begin() );
  _bootState = atoi( WebServer.arg("type").begin() );
}

PluginLed pluginLed("Led", "Handle led");

