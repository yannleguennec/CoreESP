#include "CoreSerial.h"
#include "CoreCommands.h"

#define bufferMax 128
static char buffer[bufferMax + 1];
static int  bufferNb=0;
bool serialActive = false;

void CoreSerial::setup(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  serialActive = true;
#ifndef LOG_LEVEL_DEBUG
  CoreLog::add(LOG_LEVEL_DEBUG, "SERI : Initialization.");
#endif
}

void CoreSerial::loop(void)
{
  if (Serial.available())
  {
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
    byte charIn = Serial.read();
    if (isprint(charIn))
    {
      if (bufferNb < bufferMax) // add char to string if it still fits
        buffer[bufferNb++] = charIn;
    }

    if ((charIn == '\r') || (charIn == '\n'))
    {
      if (bufferNb)
      {
#ifndef LOG_LEVEL_DEBUG_EVEN_MORE
        Serial.print("serialBuffer=");
        Serial.println(buffer);
#endif
        buffer[bufferNb] = '\0';
        String res, command = buffer;
        CoreCommands::execute( res, command );
        bufferNb = 0;
      }
    }
  }
}

bool CoreSerial::isActive(void)
{
  return serialActive;
}

