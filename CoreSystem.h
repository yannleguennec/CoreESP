#ifndef __CoreSystem__
#define __CoreSystem__

#include <IPAddress.h>

class CoreSystem : public CoreBase
{
  static ulong loopCounter;
  static ulong loopCounterLast;
  static ulong loopCounterMax;

public:
  CoreSystem(void);
  
  // Helper functions
  static void format_time(String &str, unsigned long now);
  static String IPMaskGW(int ip_if, bool mask, bool gw);
  static uint8_t cidr(IPAddress ip);

  static ulong getVersion(void);
  static ulong getBuild(void);
  static ulong getLoadAverage(void);
  static ulong getLoopCounterLast(void);
  static ulong getLoopCounterMax(void);

  virtual void setup(void);
  virtual void loop(void);

  void handleLoadAverage(void);

  void ledSetup(void);
  void ledLoop(void);
};

extern CoreSystem coreSystem;

#endif
