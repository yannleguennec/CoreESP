#ifndef __PluginSwitch__
#define __PluginSwitch__

#include "CorePlugins.h"

class PluginSwitch : public PluginButton
{
  typedef PluginButton __super;

protected:
  int switchState;
  int switchStates;
  
public:
  PluginSwitch(const char* name, const char* desc):PluginButton(name,desc){};
  virtual CorePlugins* factory(void);

  virtual void setup();
  virtual void loop();

  virtual String toString(void) { return __super::toString() + " (Pin " + buttonPin + ")"; };

  virtual void webForm(String& html);
  virtual void webSubmit( void );
};

#endif
