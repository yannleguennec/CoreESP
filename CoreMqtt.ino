#include "CoreMqtt.h"
#include "CoreWifi.h"
#include "myIPAddress.h"

std::forward_list<CoreMqttCallback*> CoreMqtt::callbacks;

CoreMqttCallback::CoreMqttCallback( String& topic, MQTT_CALLBACK_SIGNATURE)
{
  this->topic = topic;
  this->callback = callback;
}

// ---------------------------------------------------------------------------------------

//#include <WiFiClient.h>
//WiFiClient ethClient;

CoreMqtt::CoreMqtt(void) : CoreControls("Mqtt")
{
  PANIC_DEBUG();
  
  addFlags( flagLog );
  setPriority( priorityControl );

  CoreSettings::registerSetting( "log.mqttLoglevel",      LOG_LEVEL_INFO );
}

void CoreMqtt::setup(void)
{
  PANIC_DEBUG();

  CoreSettings::registerSetting( "mqtt.server", "192.168.1.190" );
  CoreSettings::registerSetting( "mqtt.port",    1883 );
  CoreSettings::registerSetting( "mqtt.user",    "Yann" );
  CoreSettings::registerSetting( "mqtt.pass",    "Yann" );

  client.setCallback(doCallback);
}

void CoreMqtt::loop(void)
{
//  PANIC_DEBUG();

  client.loop();
  
  schedule( handleMqtt, 1000 );
}

void CoreMqtt::subscribe( String topic, MQTT_CALLBACK_SIGNATURE )
{
  PANIC_DEBUG();

  // le topic et le callback dans une liste.
  #ifdef LOG_LEVEL_DEBUG
   String log =  "MQTT : Register topic '";
   log += topic;
   log += '\'';
   CoreLog::addLog(LOG_LEVEL_DEBUG, log);
  #endif

  Serial.print("Abonnement à ");
  Serial.println( topic );
  
  callbacks.push_front( new CoreMqttCallback(topic, callback) );
}

void CoreMqtt::doCallback(char* topic, byte* payload, unsigned int length)
{
  PANIC_DEBUG();

  // Parcours la liste des callback si le topic correspond, appelle le callback.
  for (CoreMqttCallback* callback : callbacks)
    if (callback->topic == topic)
      callback->callback( topic, payload, length);
}

void CoreMqtt::displayStatus( String& res )
{
  PANIC_DEBUG();

  std::vector<String> clientStates = { "Timeout", "Connection lost", "Connection failed", 
                                       "Disconnected", "Connected", "Bad protocol", "Bad client ID", 
                                       "Unavailable", "Bad credentials", "Unauthorized" };

  if ( (client.state()>=-4) && (client.state()<=5) )
    res += clientStates[ client.state() + 4 ];
  else
  {
    res += F("Unknown reason ");
    res += client.state();
    res += '\n';
  }
}
 
#define MQTT_WAIT_NONE            0
#define MQTT_WAIT_CONNECT         7
#define MQTT_WAIT_RECONNECT       10
uint mqttWait = MQTT_WAIT_NONE;

void CoreMqtt::handleMqtt()
{
  PANIC_DEBUG();

  if (client.connected()) return;
  if (!wifiIsConnected()) return;
  
  if (mqttWait)
  {
    Serial.print("Mqtt Wait...");
    Serial.println(mqttWait);
    mqttWait--;
    return;
  }

  myIPAddress server = CoreSettings::getString( "mqtt.server" );
  int port = CoreSettings::getInt( "mqtt.port");

  String *id = CoreSettings::getString("system.name");
  String *user = CoreSettings::getString( "mqtt.user" );
  String *pass = CoreSettings::getString( "mqtt.pass" );
  
  String log = F("MQTT : Connecting as ");
  log += *id;
  log += F(" to ");
  log += server.toString();
  log += F("...");
  CoreLog::addLog(LOG_LEVEL_INFO, log);
  
  client.setServer( server, port );
  
  if (client.connect( id->begin(), user->begin(), pass->begin() )) 
  {
    log=F("MQTT : Connected to ");
    log += server.toString();
    log += '.';    
    CoreLog::addLog(LOG_LEVEL_INFO, log);
    
    // Once connected, publish an announcement...
    log = F("We are ");
    log += *id;
    log += F(", resistance is futile.");
    client.publish( id->begin() , log.begin());

    // ... and resubscribe all callbacks
    for (CoreMqttCallback* callback : callbacks)
    {
      Serial.print("Subscribe to ");
      Serial.println( callback->topic );
      client.publish("info", callback->topic.begin());
      client.subscribe(callback->topic.begin());
    }  
  } else 
  {
    log=F("MQTT : ");
    displayStatus( log );
    log += '.';
    CoreLog::addLog(LOG_LEVEL_INFO, log);

    // Wait before retrying
    mqttWait=MQTT_WAIT_RECONNECT;
  }
}

void CoreMqtt::log(String &logMsg)
{
  // Send message to server
  Serial.println(logMsg.begin());
}

byte CoreConsole::logLevel(void) 
{
  return CoreSettings::getInt("log.mqttLoglevel");
}

//CoreMqtt coreMqtt;
