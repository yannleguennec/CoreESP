#ifndef __CoreLog__
#define __CoreLog__

#define logMax 20

#define LOG_LEVEL_NONE            0
#define LOG_LEVEL_ERROR           1
#define LOG_LEVEL_INFO            2
#define LOG_LEVEL_DEBUG           3
#define LOG_LEVEL_DEBUG_MORE      4
#define LOG_LEVEL_DEBUG_EVEN_MORE 5
#define LOG_LEVEL_ALL             255

class Log
{
public:
  unsigned long logTime;
  byte          logLevel;
  String        logMsg;
};

class CoreLog
{
public:
  static void setup();

  static void add( byte logLevel, String &msg );
  static void display(String &line, Log *log );

  static Log *first(void);
  static Log *next(void);

protected:
  static void flushSerial(void);
  static void flushSyslog(void);
  static void flushMQTT(void);

};

#endif
