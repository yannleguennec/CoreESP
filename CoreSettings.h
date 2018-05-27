#ifndef __CoreSettings_h__
#define __CoreSettings_h__

#include "myIPAddress.h"

#define SET_VERSION      1

#define SET_TYPE_INT     'i'
#define SET_TYPE_STRING  's'
//#define settingMax 50

typedef union
{
    int i;
    String *str;
} SettingValue;

class Setting
{
public:
  String name;
  char type;
  SettingValue value;
  SettingValue defvalue;
};

class CoreSettings
{
public:
  static void init(void);
  static void setup(void);

  static void add(char *name, char type, int value);
  static void add(char *name, char type, char *value);
  static void add(char *name, char type, String &value);

  static int        getInt( char* name );
  static char      *getChar( char* name );
  static String    *getString( char* name );

  static Setting   *getSetting( char *name, char type);

  static void defaults(String &res, char **blocks = NULL);
  static void load(String &res, char **blocks = NULL);
  static void save(String &res, char **blocks = NULL);
  static void set (String &res, char **blocks = NULL);

  static Setting *first(void);
  static Setting *next(void);
private:
  static int add(char *name, char type);

  static int search(char *str);
  static void display(String &res, int settingNo);

};

#endif
