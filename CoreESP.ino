#include "Esp.h"

// Define LOG_LEVEL_PANIC to display almost all functions prototype as they are executed.
// Excluded functions are loop and loopFast as they occur too often.
//#define LOG_LEVEL_PANIC

#ifdef LOG_LEVEL_PANIC
  #define PANIC_DEBUG() { Serial.print("Panic : "); Serial.println(__PRETTY_FUNCTION__); }
#else
  #define PANIC_DEBUG()
#endif

#include "CoreBase.h"

void setup(void)
{
  PANIC_DEBUG();

  CoreBase::doSetup();
}

void loop(void)
{
  CoreBase::doLoop();
  yield();
}

