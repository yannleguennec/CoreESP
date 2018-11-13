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
  PluginDHT(const char* name, const char* desc) : CorePlugins(name, desc) {};  
  virtual CorePlugins* factory(void);

  virtual void setup(void);
  virtual void loop(void);

  virtual String toString(void) { return __super::toString() + " (Degrés " + _degree + ")"; };

  virtual void webForm(String& html);
  virtual void webSubmit( void );
};

#endif
