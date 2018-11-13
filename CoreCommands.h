#ifndef __CoreCommands__
#define __CoreCommands__

#include "CoreBase.h"
#include <forward_list>

#define commandMax  20
#define commandArgs 2

#define commandName     block[0]
#define commandArg(x)   block[x]

#define CoreCommand_Callback(func) void (*func)(String &, char**)

// class CoreCommands;
class CoreCommands : public CoreBase
{
protected:
  static std::forward_list<CoreCommands*> commands;
  static uint commandNb;

  String cmd;
  CoreCommand_Callback(func);
  String desc;

public:
  CoreCommands(void);
  CoreCommands(String& cmd, CoreCommand_Callback(func), String& desc); 

  virtual void setup(void);
 
  static void registerCommand(String cmd, void (*func)(String&, char**), String desc);
  static bool execute(String &res, String &line);
  static bool split(String &line, char **block);
  static bool run(String &res, char **block);
  static bool notFound(String &res, String &line);

  static CoreCommands* search(String str);
  static void display(String &res, CoreCommands* command);

  static void info(String &res, char **block);
  static void restart(String &res, char **block);
  static void reboot(String &res, char **block);
  static void help(String &res, char **block); 
};

#endif
