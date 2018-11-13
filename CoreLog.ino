#include "CoreLog.h"

std::vector<String> CoreLog::logLevelMessages = { "<ERR > ", "<INFO> ", "<DEBG> ", "<DBG+> ", "<DB++> " };

void CoreLog::addLog(byte logLevel, String &logMsg)
{
  PANIC_DEBUG();

  if (logLevel > logLevelMessages.size())
  {
    Serial.print( F("Log level overflow (") );
    Serial.print(logLevel);
    Serial.print( F(" > logLevelMax (") );
    Serial.print(logLevelMessages.size());
    Serial.println( F(")).") );
    return;
  }

  for ( CoreBase* base : list )
  {
    CoreLog* log = reinterpret_cast<CoreLog*>(base);
    if ((log->flags() & flagLog) && (logLevel <= log->logLevel()))
      log->log(logLevel, logMsg);
  }
}

String CoreLog::toString(byte logLevel, String& logMsg)
{
  String res;
  char str[12];
  
  sprintf(str, "%10lu", millis());
  res += str;
  res += " : ";

  res += logLevelMessages[ logLevel - 1];

  res += " ";
  res += logMsg;

  return res;
}

