#ifndef __CoreMqtt__
#define __CoreMqtt__

#include "CoreControls.h"
#include <PubSubClient.h>

class Abonnements
{
public:
  String topic;
  MQTT_CALLBACK_SIGNATURE;
  
  Abonnements( String& topic, MQTT_CALLBACK_SIGNATURE );
};

class CoreMqtt : protected CoreControls
{  
  static std::forward_list<Abonnements> abonnements;

protected:
  typedef CoreControls __super;
  PubSubClient client;

  static void callback(char* topic, byte* payload, unsigned int length);
  void reconnect( void );

public:
  CoreMqtt(void);
  
  void setup(void);
  void loop(void);
  void loopMedium(void);
  
  void registerCallback( String topic, MQTT_CALLBACK_SIGNATURE );
};

extern CoreMqtt coreMqtt;

#endif
