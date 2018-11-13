#ifndef __CoreLog__
#define __CoreLog__

#include "CoreBase.h"
#include <vector>
//#include <list>

class CoreLog : public CoreBase
{
  public:

    CoreLog(String &name) : CoreBase(name) {};
    
    enum class LogLevel
    {
      None = 0,
      Error,
      Info,
      Debug,
      DebugMore,
      DebugEvenMore,
      All = 255
    };

#define LOG_LEVEL_NONE            0
#define LOG_LEVEL_ERROR           1
#define LOG_LEVEL_INFO            2
#define LOG_LEVEL_DEBUG           3
#define LOG_LEVEL_DEBUG_MORE      4
#define LOG_LEVEL_DEBUG_EVEN_MORE 5
#define LOG_LEVEL_ALL             255

    static void addLog(byte logLevel, String &logMsg);

    // Getters
    virtual byte logLevel(void) = 0;

  protected:
    virtual void log(byte, String&) {};
    String toString(byte logLevel, String& logMsg);

  private:
    static std::vector<String> logLevelMessages;

};

#endif
