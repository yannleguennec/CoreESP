#ifndef __CoreSystem__
#define __CoreSystem__

#include <IPAddress.h>

class CoreSystem
{
public:
  // Helper functions
  static void format_time(String &str, unsigned long now);
  static String IPMaskGW(int ip_if, bool mask, bool gw);
  static uint8_t cidr(IPAddress ip);

  static unsigned long getVersion(void);
  static unsigned long getBuild(void);
  static unsigned long getLoadAverage(void);
  static unsigned long getLoopCounterLast(void);
  static unsigned long getLoopCounterMax(void);

  static void setup(void);
  static void loop(void);
  static void loopSlow(void);

};

#endif
