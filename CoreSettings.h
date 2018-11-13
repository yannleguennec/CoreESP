#ifndef __CoreSettings_h__
#define __CoreSettings_h__

/*
Pour sauvegarder dans la Flash, il faut écrire chaque initialisation à faire pour pouvoir l'envoyer directement à l'interpreteur, après décomposition en ligne sur retour-chariot.

Reste à voir comment écrire la configuration des devices...

Peut-être un truc du genre :

device.01.led.value=1
device.02.button.value=1
device.01.led.topic=led
*/

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

class CoreSettings : public CoreBase
{
public:
  String name;
  char type;
  SettingValue value;
  SettingValue defvalue;

  static std::forward_list<CoreSettings*> settings;
  static uint settingNb;
  
  CoreSettings(void);
  CoreSettings(const char *name, char type);

  virtual void setup(void);

  static void registerSetting(const char *name, int value);
  static void registerSetting(const char *name, const char *value);
  static void registerSetting(const char *name, String &value);

  static int        getInt( const char* name );
  static char      *getChar( const char* name );
  static String    *getString( const char* name );

  static CoreSettings *getSetting( const char *name, char type);

  static void defaults(String &res, char **blocks = NULL);
  static void load(String &res, char **blocks = NULL);
  static void save(String &res, char **blocks = NULL);
  static void set (String &res, char **blocks = NULL);

private:
  static CoreSettings* registerSetting(const char *name, char type);

  static CoreSettings* search(const char *name);
  static void display(String &res, CoreSettings* setting);
};

#endif
