#include "CoreControls.h"
#include "CoreLog.h"

std::forward_list<CoreControls*> CoreControls::controls;
uint CoreControls::controlsNb = 0;

#define forEachControl(func) { \
    static bool doLoop=true; \
    if (doLoop) { Serial.print("ctrl"); Serial.println(doLoop);\
      doLoop=false; for(CoreControls &control : controls) { control.func(); } doLoop=true; \
    } \
}

CoreControls::CoreControls(void)
{
}

void CoreControls::registerControl( String name, CoreControls *control ) 
{ 
//#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
//#endif

  if (controls.empty())
  {
    controlsNb=0;
  }

  this->name = name;
  controls.push_front(control); 
  controlsNb++; 
};

void CoreControls::setup(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
#ifdef LOG_LEVEL_DEBUG
  String log = F("CTLS : Initialization.");
  CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif
  coreCommand.addCommand( "controls",    CoreControls::listCommand,    "Display list of controls" );

  //forEachDevice(setup);
}

void CoreControls::loop(void) 
{
  //forEachDevice(loop);
}

void CoreControls::loopSlow(void)
{
  //forEachDevice(loopSlow);
}

void CoreControls::loopMedium(void)
{
  //forEachDevice(loopMedium);
}

void CoreControls::loopFast(void)
{
  //forEachDevice(loopFast);
}

void CoreControls::listCommand( String &res, char** )
{
  res += controlsNb;
  res += F(" controls :\n");

  uint controlNo = 1;
  for (CoreControls* control : controls)
  {
    res += F("  ");
    res += controlNo++;
    res += F(" : ");
    res += control->name;
    res += '\n';
  }
}

CoreControls coreControls;

