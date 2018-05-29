#ifndef __CoreDevices__
#define __CoreDevices__

#include "CorePlugins.h"

class CoreDevices
{
public:
  static void setup(void);

  static void loopSlow(void);
  static void loopMedium(void);
  static void loopFast(void);

  static void setupPlugin(int& deviceId, int pluginId);

  static void listCommand(String &res, char **block);
  static void setCommand(String &res, char **block);
  
  static void listWeb(void);
  static void setWeb(void);
};
#endif
