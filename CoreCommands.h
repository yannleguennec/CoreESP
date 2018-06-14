#ifndef __CoreCommands__
#define __CoreCommands__

class CoreCommands : public CoreControllers
{
public:
  void setup(void);
  
  static void info(String &res, char **block);
  static void restart(String &res, char **block);
  static void reboot(String &res, char **block);
  static void help(String &res, char **block); 
};

extern CoreCommands coreCommands;

#endif
