#include "CoreMqtt.h"
#include "myIPAddress.h"

#include <WiFiClient.h>
WiFiClient ethClient;
//Client ethClient;

std::forward_list<Abonnements> CoreMqtt::abonnements;

CoreMqtt::CoreMqtt(void) : client(ethClient)
{

}

Abonnements::Abonnements( String& topic, MQTT_CALLBACK_SIGNATURE)
{
  this->topic = topic;
  this->callback = callback;
}

void CoreMqtt::registerCallback( String topic, MQTT_CALLBACK_SIGNATURE )
{
  // le topic et le callback dans une liste.
  #ifdef LOG_LEVEL_DEBUG
   String log =  "MQTT : Register topic '";
   log += topic;
   log += '\'';
   CoreLog::add(LOG_LEVEL_DEBUG, log);
  #endif

  Serial.print("Abonnement à ");
  Serial.println( topic );
  Abonnements *abo = new Abonnements(topic, callback);
  abonnements.push_front( *abo );
}

void CoreMqtt::callback(char* topic, byte* payload, unsigned int length)
{
  // Parcours la liste des callback si le topic correspond, appelle le callback.
  for (Abonnements& abo : abonnements)
    if (abo.topic == topic)
      abo.callback( topic, payload, length);
}

uint mqttWait = 5;

void CoreMqtt::reconnect()
{
  if (mqttWait)
  {
    Serial.print("Mqtt Wait...");
    Serial.println(mqttWait);
    mqttWait--;
    return;
  }
  
  // Loop until we're reconnected
  Serial.print("Attempting MQTT connection...");
  // Attempt to connect
  if (client.connect("arduinoClient")) {
    Serial.println("connected");
    // Once connected, publish an announcement...
    client.publish("outTopic", "hello world");
    // ... and resubscribe
    for (Abonnements& abo : abonnements)
    {
      Serial.print("Abo ");
      Serial.println( abo.topic );
      client.publish("abo", abo.topic.begin());
      client.subscribe(abo.topic.begin());
    }
       
  } else {
    Serial.print("failed, rc=");
    Serial.print(client.state());
    Serial.println(" try again in 5 seconds");
    // Wait 5 seconds before retrying
    mqttWait=5;
  }
}

myIPAddress server("192.168.1.100");

void CoreMqtt::setup(void)
{
  registerControl("Mqtt", this);

  client.setServer(server, 1883);
  client.setCallback(callback);
}

void CoreMqtt::loopMedium(void)
{
  if (!client.connected()) {
    reconnect();
  }
}

void CoreMqtt::loop(void)
{
  client.loop();
}

CoreMqtt coreMqtt;
