#ifndef __CoreSerial__
#define __CoreSerial__

// This object should inherit from a common object for log activities
class CoreSerial
{
public:
  static void setup(void);
  static void loop(void);

  static bool isActive(void);
};

#endif
