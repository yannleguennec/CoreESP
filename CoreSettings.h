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

  static void add(const char *name, char type, int value);
  static void add(const char *name, char type, const char *value);
  static void add(const char *name, char type, String &value);

  static int        getInt( const char* name );
  static char      *getChar( const char* name );
  static String    *getString( const char* name );

  static Setting   *getSetting( const char *name, char type);

  static void defaults(String &res, char **blocks = NULL);
  static void load(String &res, char **blocks = NULL);
  static void save(String &res, char **blocks = NULL);
  static void set (String &res, char **blocks = NULL);

  static Setting *first(void);
  static Setting *next(void);
private:
  static int add(const char *name, char type);

  static int search(const char *str);
  static void display(String &res, int settingNo);

};

#endif
