#ifndef __PluginSwitch__
#define __PluginSwitch__

#include "CorePlugins.h"

class PluginSwitch : public CorePlugins
{
private:
  typedef CorePlugins __super;
  
  int  _type;
  int  _pin;
  bool _inverse;
  
  int  _state;
public:
  PluginSwitch(void): __super() {};  
  PluginSwitch(String pluginName, String pluginDesc) : __super(pluginName, pluginDesc) {};
  
  virtual CorePlugins* factory(void);
  virtual void setup();

  virtual String toString(void) { return __super::toString() + " (Pin " + _pin + ")"; };

  virtual void webForm(String& html);
  virtual void webSubmit( void );
  
  virtual void loopFast();     // to get switch position
  virtual void loopMedium();   // to send infos
};

#endif
