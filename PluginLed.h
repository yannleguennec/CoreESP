#ifndef __PluginLed__
#define __PluginLed__

#include "CorePlugins.h"

class PluginLed : public CorePlugins
{
private:
  typedef CorePlugins __super;
  
  int  _type;
  int  _pin;
  bool _inverse;
  int  _bootState;

  int  _state;
public:
  PluginLed(void): __super() {};
  PluginLed(String pluginName, String pluginDesc) : __super(pluginName, pluginDesc) {};
  
  virtual CorePlugins* factory(void);
  static void setup(void);

  String toString(void) { return __super::toString() + " (Pin " + _pin + ")"; };

  virtual void webForm(String& html);
  virtual void webSubmit( void );

  void loopFast(void);     // to get switch position
  void loopMedium(void);   // to send infos
};

#endif
