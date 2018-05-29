#include "CoreLog.h"
#include "CoreSerial.h"

Log logs[logMax];
int logWritePtr      = 0; // Pointer to next write entry
int logReadPtrSerial = 0; // Pointer for read to Serial
int logReadPtrSyslog = 0; // Pointer for read to Syslog
int logReadPtrMqtt   = 0; // Pointer for read to Mqtt
int logIterPtr       = 0; // Pointer for iterating amongst logs (web)

#define logLevelMax 5
char *logLevelMsg[logLevelMax+1] = { "<ERR > ", "<INFO> ", "<DEBG> ", "<DBG+> ", "<DB++> " };

#define logIncr(x) (x)=(((x)       +1)%logMax)
#define logDecr(x) (x)=(((x)+logMax-1)%logMax)

void CoreLog::setup(void)
{
#ifdef LOG_LEVEL_DEBUG
  String log = F( "LOGS : Initialization." );
  CoreLog::add(LOG_LEVEL_DEBUG, log );
#endif
}

void CoreLog::add( byte logLevel, String &msg ) 
{
  if (logLevel > logLevelMax)
  {
    Serial.print( F("Log level overflow (") );
    Serial.print(logLevel);
    Serial.print( '>' );
    Serial.print(logLevelMax);
    Serial.println( F(").") );
    return;
  }

  logs[ logWritePtr ].logTime  = millis();
  logs[ logWritePtr ].logLevel = logLevel;
  logs[ logWritePtr ].logMsg   = msg;
  logIncr(logWritePtr);

  // If read pointers are equal to write pointer, we filled the buffer and just overriden the oldest entry.
  // So push the start and forget the oldest entry...
  if ( logWritePtr == logReadPtrSerial ) logIncr( logReadPtrSerial );
  if ( logWritePtr == logReadPtrSyslog ) logIncr( logReadPtrSyslog );
  if ( logWritePtr == logReadPtrMqtt   ) logIncr( logReadPtrMqtt   );
  if ( logWritePtr == logIterPtr       ) logIncr( logIterPtr       );

  CoreLog::flushSerial();
  //CoreLog::flushSyslog();
  //CoreLog::flushMQTT();
};

void CoreLog::display(String &line, Log *log )
{
  char str[11];
  int logLevel = log->logLevel;
  
  sprintf(str, "%10l", log->logTime);
  line += str;
  line += " : ";
  line += logLevelMsg[ logLevel - 1 ];
  line += " ";
  line += log->logMsg;
}

void CoreLog::flushSerial(void)
{
  if (CoreSerial::isActive())
  {
    // Flush the logBuffer for Serial
    if ( logReadPtrSerial != logWritePtr )
    {
      while ( logReadPtrSerial != logWritePtr )
      {
        int logLevel = logs[ logReadPtrSerial ].logLevel;
        int serialLogLevel = CoreSettings::getInt("log.serialLoglevel");
        
        if (logLevel <= serialLogLevel)
        {
          String line;
          CoreLog::display(line, &logs[ logReadPtrSerial ] );
          Serial.println(line);
        }
        logIncr(logReadPtrSerial);
      }
    }
  }
}

Log *CoreLog::first(void)
{
  logIterPtr = logWritePtr;
  logIncr( logIterPtr );
  return &logs[ logIterPtr ];
}

Log *CoreLog::next(void)
{
  if (logWritePtr == logIterPtr) 
    return NULL;
  
  logIncr(logIterPtr);
  return &logs[ logIterPtr ];
}


