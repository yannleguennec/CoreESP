#ifndef __CoreCommands__
#define __CoreCommands__

#define commandMax  20
#define commandArgs 2

#define commandName     block[0]
#define commandArg(x)   block[x]

class CoreCommands
{
public:
  static void setup(void);

  static void add(String cmd, void (*func)(String&, char**), String desc);
  static bool execute(String &res, String &line);
  static bool split(String &line, char **block);
  static bool run(String &res, char **block);
  static bool notFound(String &res, String &line);

  static int  search(String str);
  static void display(int commandNo);

  static void info(String &res, char **block);
  static void restart(String &res, char **block);
  static void reboot(String &res, char **block);
  static void help(String &res, char **block);
};

#endif
