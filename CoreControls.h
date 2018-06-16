#ifndef __CoreControls__
#define __CoreControls__

#define controlMax 5

class CoreControls
{
  static CoreControls *controls[controlMax];
  static int controlNb;

protected:
  void registerControl( CoreControls* control);

public:
  CoreControls(void);
  void setup(void);
};

extern CoreControls coreControl;

#endif
