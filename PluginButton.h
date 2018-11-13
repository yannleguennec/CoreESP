#ifndef __CoreButton__
#define __CoreButton__

#include "CorePlugins.h"

// Debouncing is done with shift register method by default
#define debounceMethodNone 0
#define debounceMethodLockOut 1
#define debounceMethodTimer 2
#define debounceMethodShiftRegister 3

class PluginButton : public CorePlugins
{
  public:
    typedef CorePlugins __super;

    PluginButton(const char* name, const char* desc):CorePlugins(name,desc){};
    virtual CorePlugins* factory(void);

    virtual void setup(void);
    virtual void loop(void);
    
    virtual void webSubmit( void );

    void update(void);
    void setMethod(int method);
    void setPin(int pin);
    void setTimeout(unsigned long timeout);

  protected:
    bool debounce(void);
    void send(void);

    int debounceMethod;
    unsigned long debounceTimeout;

    int buttonPin;
    bool buttonInvert;

    word buttonState;
};
#endif
