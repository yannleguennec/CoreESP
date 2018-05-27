#ifndef __PluginSwitch__
#define __PluginSwitch__

#include "CorePlugins.h"

class PluginSwitch : public CorePlugins
{
  int  pin;
  bool inverse;
  int  state;
public:
  PluginSwitch(bool first = false);

  CorePlugins* factory(void);
  
  static void setup();

  void webForm( String &res );
  
  void loopFast();     // to get switch position
  void loopMedium();   // to send infos
};

#endif
