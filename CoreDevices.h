#ifndef __CoreDevices__
#define __CoreDevices__

#include "CorePlugins.h"

class CoreDevices
{
  int    _num;
  String _name;
  String _desc;

  CorePlugins* plugin;

public:
  CoreDevices(void);

  int num( int num = 0);
  String* name( String* name = NULL);
  String* desc( String* desc = NULL);
  friend class CoreDevices;

  static void setup(void);

  static void loopSlow(void);
  static void loopMedium(void);
  static void loopFast(void);

  static void devicesCommand(String &res, char **block);
  static void deviceCommand(String &res, char **block);
  static void devicesWeb(void);
  static void deviceWeb(void);

  static CoreDevices* first(void);
  static CoreDevices* next(void);
};
#endif
