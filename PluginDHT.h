#ifndef __PluginDHT__
#define __PluginDHT__

#include "CorePlugins.h"

class PluginDHT : public CorePlugins
{
private:
  typedef CorePlugins __super;
  
  int  _pin;
  int  _degree;
 
public:
  PluginDHT(void): __super() {};
  PluginDHT(String pluginName, String pluginDesc) : __super(pluginName, pluginDesc) {};
  
  virtual CorePlugins* factory(void);
  virtual void setup(void);

  virtual String toString(void) { return __super::toString() + " (Degrés " + _degree + ")"; };

  virtual void webForm(String& html);
  virtual void webSubmit( void );

  virtual void loopFast(void);     // to get switch position
  virtual void loopMedium(void);   // to send infos
};

#endif
