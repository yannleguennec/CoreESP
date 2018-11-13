#include "CoreControls.h"

uint CoreControls::controlsNb = 0;

CoreControls::CoreControls(String name) : CoreLog(name)
{
  PANIC_DEBUG();
  setPriority( priorityControl );
  addFlags( flagControl );

  controlsNb++;
  controlActive = false;
};

void CoreControls::setup(void)
{
  PANIC_DEBUG();

#ifdef LOG_LEVEL_DEBUG
  String log = F("CTLS : Initialization.");
  CoreLog::addLog(LOG_LEVEL_DEBUG, log);
#endif
  CoreCommands::registerCommand( "controls",    CoreControls::listControls,    "Display list of controls" );
}

void CoreControls::listControls( String &res, char** )
{
  PANIC_DEBUG();

  res += controlsNb;
  res += F(" controls :\n");

  uint controlNo = 1;
  for (CoreBase* base : list)
    if (base->flags() & flagControl)
    {
      res += F("  ");
      res += controlNo++;
      res += F(" : ");
      res += base->name();
      res += '\n';
    }
}

