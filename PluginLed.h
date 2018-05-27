#ifndef __PluginLed__
#define __PluginLed__

#include "CorePlugins.h"

class PluginLed : public CorePlugins
{
  int  pin;
  bool inverse;
  int  state;
public:
  PluginLed(bool first = false);
  
  CorePlugins* factory(void);
  static void setup();

  void webForm( String &res );
  
  void loopFast();     // to get switch position
  void loopMedium();   // to send infos
};

#endif
