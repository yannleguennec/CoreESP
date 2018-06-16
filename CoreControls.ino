#include "CoreControls.h"
#include "CoreLog.h"

CoreControls *CoreControls::controls[controlMax];
int CoreControls::controlNb;

CoreControls::CoreControls(void)
{
    controlNb = 0;
}

void CoreControls::registerControl( CoreControls* control)
{
    if (controlNb < controlMax)
        controls[controlNb++] = control;
    else
    {
        String log = F("CMDS : Increase controlMax constant.");
        CoreLog::add(LOG_LEVEL_ERROR, log);
    }
}

void CoreControls::setup(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
#ifdef LOG_LEVEL_DEBUG
  String log = F("CTLS : Initialization.");
  CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif
}

CoreControls coreControl;
