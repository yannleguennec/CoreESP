#ifndef __PluginLed__
#define __PluginLed__

#include "CorePlugins.h"

class PluginLed : public CorePlugins
{
  typedef CorePlugins __super;
  
public:
  PluginLed(const char* name, const char* desc):CorePlugins(name, desc){};
  
  virtual CorePlugins* factory(void);

  virtual void setup(void);

  virtual String toString(void) { return __super::toString() + " (Pin " + _pin + ")"; };

  virtual void webForm(String& html);
  virtual void webSubmit( void );

private:
  int  _type;
  int  _pin;
  bool _inverse;
  int  _bootState;

  int  _state;
  
  static void callback(char* topic, byte* payload, unsigned int length);

};

#endif
