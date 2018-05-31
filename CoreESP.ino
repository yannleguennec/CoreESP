#include "Esp.h"

#include "CoreSystem.h"
#include "CoreLog.h"
#include "CoreSerial.h"
#include "CoreCommands.h"
#include "CoreSettings.h"
#include "CoreWifi.h"
#include "CoreHttp.h"
#include "CoreDevices.h"

// Definition des plugins :
#include "PluginSwitch.h"

// Multitasking : Schedule the launch of the func every interval (ms)
#define schedule(func, interval) { static unsigned long func##last=0; if (millis()-(func##last)>=(interval)) { (func##last)=millis(); (func)(); } }

// loops delays in ms
#define loopFastDelay   10
#define loopMediumDelay 1000
#define loopSlowDelay   20000

void setup(void)
{
  CoreSystem::setup();
  CoreSettings::init();
  CoreLog::setup();
  CoreHttp::setup();
  CoreSerial::setup();
  CoreSettings::setup();
  CoreCommands::setup();
  CorePlugins::setup();
  CoreDevices::setup();
  CoreWifi::setup();
//  //CoreMqtt::setup();

  pinMode(LED_BUILTIN, OUTPUT);
}

#define ledPin LED_BUILTIN
#define divisor 100

void ledToggle(void)
{
  static int count = 0;
  int numerator = (wifiIsConnected()) ? divisor - 10 : 5;

  if (!(count % divisor))
    digitalWrite(ledPin, HIGH);
  else if (!((count + divisor - numerator) % divisor))
    digitalWrite(ledPin, LOW);
  
  count = (count+1) % divisor;
}

void loopFast(void)
{
  CoreDevices::loopFast();
  ledToggle();
}

void loopMedium(void)
{
  CoreWifi::loopMedium();
  CoreHttp::loopMedium();
  CoreDevices::loopMedium();
}

void loopSlow(void)
{
  CoreSystem::loopSlow();
  CoreDevices::loopSlow();
}

void loop(void)
{
  CoreSystem::loop();
  CoreSerial::loop();
  CoreHttp::loop();

  schedule( loopFast,    loopFastDelay );
  schedule( loopMedium,  loopMediumDelay  );
  schedule( loopSlow,    loopSlowDelay );

  yield();
}

