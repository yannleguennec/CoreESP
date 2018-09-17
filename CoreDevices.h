#ifndef __CoreDevices__
#define __CoreDevices__

#include <array>
#include "CorePlugins.h"

#define deviceMax 10

class CoreDevices
{
public:
  static std::array<CorePlugins*,deviceMax> devices;

  virtual void setup(void);
  virtual void loopSlow(void);
  virtual void loopMedium(void);
  virtual void loopFast(void);

  void setupPlugin(size_t deviceId, size_t pluginId);

  static void listCommand(String &res, char **block);
  static void setCommand(String &res, char **block);
  
  static void listWeb(void);
  static void setWeb(void);
};

extern CoreDevices coreDevices;
#endif
