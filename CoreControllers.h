#ifndef __CoreControllers__
#define __CoreControllers__

#define commandMax  20
#define commandArgs 2

#define commandName     block[0]
#define commandArg(x)   block[x]

class Commands
{
public:
  String cmd;
  void (*func)(String &, char**);
  String desc;
};

class CoreControllers
{
protected:
  static Commands command[commandMax];
  static int commandNb;
  
public:
  CoreControllers(void);

  void addCommand(String cmd, void (*func)(String&, char**), String desc);
  bool execute(String &res, String &line);
  bool split(String &line, char **block);
  bool run(String &res, char **block);
  bool notFound(String &res, String &line);

  int  search(String str);
  void display(int commandNo);

};

extern CoreControllers coreController;

#endif
