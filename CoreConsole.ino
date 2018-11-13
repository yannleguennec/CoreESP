#include "CoreConsole.h"
#include "CoreSettings.h"
CoreConsole::CoreConsole(void) : CoreControls(String("Serial"))
{
  PANIC_DEBUG();
  setPriority( priorityLog );
  addFlags( flagLog );;

  // Serial.begin(115200);  // This is done in CoreSystem !!!
  CoreSettings::registerSetting( "log.serialLoglevel",    LOG_LEVEL_DEBUG );

  serialActive = false;
}

void CoreConsole::setup(void)
{
  PANIC_DEBUG();

  serialActive = true;

#ifndef LOG_LEVEL_DEBUG
  CoreLog::addLog(LOG_LEVEL_DEBUG, "SERI : Initialization.");
#endif
}

void CoreConsole::log(byte level, String& msg)
{
  PANIC_DEBUG();
  Serial.println( toString( level, msg ) );
}

byte CoreConsole::logLevel(void) 
{
  return CoreSettings::getInt("log.serialLoglevel");
}

void CoreConsole::loop(void)
{
  if (serialActive && Serial.available())
  {
    PANIC_DEBUG();
    byte charIn = Serial.read();
    if (isprint(charIn))
      if (bufferNb < bufferMax) // add char to string if it still fits
        buffer[bufferNb++] = charIn;

    // If a newline is found
    if ((charIn == '\r') || (charIn == '\n'))
    {
      // If buffer is not empty
      if (bufferNb)
      {
        buffer[bufferNb] = '\0';
#ifndef LOG_LEVEL_DEBUG_EVEN_MORE
        Serial.print("serialBuffer=");
        Serial.println(buffer);
#endif
        String res, command = buffer;
        CoreCommands::execute( res, command );

        // Display result to console
        char *msg, *line = res.begin();

        // Split line in blocks and prints them (separated by \n)
        while (*line)
        {

          msg = line;
          while ( *line && *line != '\n' ) line++;
          if (*line) *line++ = '\0';

          String log = F("CONS :   ");
          log += msg;
          CoreLog::addLog(LOG_LEVEL_INFO, log);
        }

        bufferNb = 0;
      }
    }
  }
}

CoreConsole coreConsole;

