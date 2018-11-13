#ifndef __CoreControls__
#define __CoreControls__

#include <forward_list>
#include "CoreBase.h"
#include "CoreLog.h"

class CoreControls : public CoreLog
{
protected:
  static unsigned int controlsNb;
  bool controlActive;
  
public:
  CoreControls(String name);
  
  virtual void setup(void);

  static void listControls( String &, char** );
};

#endif
