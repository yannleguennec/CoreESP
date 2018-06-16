#ifndef __CoreConsole__
#define __CoreConsole__

#define bufferMax 128

#include "CoreControls.h"

// This object should inherit from a common object for log activities
class CoreConsole : protected CoreControls
{
  bool serialActive;
  char buffer[bufferMax + 1];
  int  bufferNb=0;
protected:
  typedef CoreControls __super;

public:
  CoreConsole(void);

  void setup(void);
  void loop(void);

  bool isActive(void) { return serialActive; };
};

CoreConsole coreConsole;

#endif
