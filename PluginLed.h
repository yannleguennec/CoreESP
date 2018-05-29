#ifndef __PluginLed__
#define __PluginLed__

#include "CorePlugins.h"

class PluginLed : public CorePlugins
{
  int  _pin;
  int  _bootState;
  int  _type;
  bool _inverse;
  int  _state;
public:
  PluginLed(bool first = false);
  
  CorePlugins* factory(void);
  static void setup();

  String& toString(void) { return CorePlugins::toString() + " (Pin " + _pin + ")"; };

  void webForm(String& html);
  void webFormSubmit( void );

  void loopFast();     // to get switch position
  void loopMedium();   // to send infos
};

#endif
