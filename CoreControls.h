#ifndef __CoreControls__
#define __CoreControls__

#include <forward_list>

class CoreControls;
class CoreControls 
{
public:
  static std::forward_list<CoreControls*> controls;
  static uint controlsNb;
  
  String name;
  String debug;

  CoreControls(void);
  
  virtual void setup(void);
  virtual void loop(void);
  virtual void loopSlow(void);
  virtual void loopMedium(void);
  virtual void loopFast(void);

  void registerControl( String name, CoreControls *control );
  
  static void listCommand( String &, char** );
//  static Array<CoreControls> &list(void) { return CoreControls::controls; };
};

extern CoreControls coreControls;

#endif
