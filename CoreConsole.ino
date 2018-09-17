#include "CoreConsole.h"

CoreConsole::CoreConsole(void)
{
  Serial.println( __PRETTY_FUNCTION__ );
  serialActive = false;
  debug="DeBuG Console";
}

#ifdef LOG_LEVEL_PANIC
  #define PANIC_DEBUG() Serial.println(__PRETTY_FUNCTION__);
#else
  #define PANIC_DEBUG()
#endif

void CoreConsole::setup(void)
{
  PANIC_DEBUG();

  registerControl("Console", this);
  serialActive = true;
#ifndef LOG_LEVEL_DEBUG
  CoreLog::add(LOG_LEVEL_DEBUG, "SERI : Initialization.");
#endif
}

for ( ;v=0; );

void CoreConsole::loop(void)
{
  if (serialActive && Serial.available())
  {
    PANIC_DEBUG();
    byte charIn = Serial.read();
    if (isprint(charIn))
    {
      if (bufferNb < bufferMax) // add char to string if it still fits
        buffer[bufferNb++] = charIn;
    }

    // If a newline is found
    if ((charIn == '\r') || (charIn == '\n'))
    {
      // If buffer is not empty
      if (bufferNb)
      {
#ifndef LOG_LEVEL_DEBUG_EVEN_MORE
        Serial.print("serialBuffer=");
        Serial.println(buffer);
#endif
        buffer[bufferNb] = '\0';
        String res, command = buffer;
        coreCommand.execute( res, command );
        // Display result to console
        Serial.println(res);
        bufferNb = 0;
      }
    }
  }
}

CoreConsole coreConsole;

