#ifndef __CoreMqtt__
#define __CoreMqtt__

#include "CoreControls.h"
#include <PubSubClient.h>

class CoreMqttCallback
{
public:
  String topic;
  MQTT_CALLBACK_SIGNATURE; // this defines a callback attribute with right prototype (see PibSubClient.h)
  
  CoreMqttCallback( String& topic, MQTT_CALLBACK_SIGNATURE );
};

class CoreMqtt : protected CoreControls
{  
  typedef CoreControls __super;

public:
  CoreMqtt(void);
  
  virtual void setup(void);
  virtual void loop(void);

  virtual void log(String&);

  void subscribe( String topic, MQTT_CALLBACK_SIGNATURE );
  
protected:
  static std::forward_list<CoreMqttCallback*> callbacks;
  PubSubClient client;

  static void doCallback(char* topic, byte* payload, unsigned int length);
  void handleMqtt( void );
  void displayStatus( String& res );

};

#endif
