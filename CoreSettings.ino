#include "CoreSettings.h"
#include "CoreDefaults.h"

#define settingMax 50
Setting setting[settingMax];
int settingNb = 0;
int settingNo = 0;

void CoreSettings::init(void)
{
  // No log here, as CoreLog not initialized
  // These are important settings needed from the start.
  CoreSettings::add( "log.serialLoglevel",    SET_TYPE_INT,     LOG_LEVEL_DEBUG );
  CoreSettings::add( "log.mqttLoglevel",      SET_TYPE_INT,     LOG_LEVEL_INFO );
  CoreSettings::add( "log.syslogLoglevel",    SET_TYPE_INT,     LOG_LEVEL_INFO );
  CoreSettings::add( "log.httpLoglevel",      SET_TYPE_INT,     LOG_LEVEL_INFO );
}

void CoreSettings::setup(void)
{
#ifdef LOG_LEVEL_DEBUG
  String log = F("SETS : Initialization.");
  CoreLog::add(LOG_LEVEL_DEBUG, log );
#endif

  CoreCommands::add( "set",    CoreSettings::set,    "Modify or display settings value"     );
  CoreCommands::add( "load",   CoreSettings::load,   "Loads configuration from flash"       );
  CoreCommands::add( "save",   CoreSettings::save,   "Saves current configuration to flash" );

  CoreSettings::add( "system.name",        SET_TYPE_STRING,  DEFAULT_NAME     );
  CoreSettings::add( "system.unit",        SET_TYPE_INT,     DEFAULT_UNIT     );
  CoreSettings::add( "system.pass",        SET_TYPE_STRING,  DEFAULT_PASSWORD );

}

void CoreSettings::add(char *name, char type, int value)
{
  int settingNo = CoreSettings::add(name, type);
  if (settingNo < settingNb)
    setting[ settingNo ].defvalue.i = setting[ settingNo ].value.i = value;
}

void CoreSettings::add(char *name, char type, char *value)
{
  int settingNo = CoreSettings::add(name, type);
  if (settingNo < settingNb)
  {
    setting[ settingNo ].defvalue.str = new String(value);
    setting[ settingNo ].value.str = new String(value);
  }
}

void CoreSettings::add(char *name, char type, String &value)
{
  int settingNo = CoreSettings::add(name, type);
  if (settingNo < settingNb)
  {
    setting[ settingNo ].defvalue.str = new String(value);
    setting[ settingNo ].value.str = new String(value);
  }
}


int CoreSettings::add(char *name, char type)
{
  String log;
  int settingNo;

  if (settingNb < settingMax)
  {
    // Add a setting to the interpreter
#ifdef LOG_LEVEL_DEBUG_MORE
    log = "SETS : Adding setting '";
    log += name;
    log += "'";
    CoreLog::add(LOG_LEVEL_DEBUG_MORE, log);
#endif
    setting[settingNb].name = name;
    setting[settingNb].type = type;
    settingNo = settingNb++;
  }
  else
  {
    // This should be a log error
    Serial.println(F("SETS : Increase settingMax."));
    settingNo = settingMax;
  }

  return settingNo;
}

Setting *CoreSettings::first(void)
{
  settingNo = 0;
  return &setting[ settingNo ];
}

Setting *CoreSettings::next(void)
{
  if (++settingNo < settingNb)
    return &setting[ settingNo ];
  return NULL;
}

int CoreSettings::getInt( char *name )
{
  Setting *setting = CoreSettings::getSetting( name, SET_TYPE_INT );
  if (setting) return setting->value.i;
  return 0;
}

char *CoreSettings::getChar( char *name )
{
  Setting *setting = CoreSettings::getSetting( name, SET_TYPE_STRING );
  if (setting) return setting->value.str->begin();
  return NULL;
}

String *CoreSettings::getString( char *name )
{
  Setting *setting = CoreSettings::getSetting( name, SET_TYPE_STRING );
  if (setting) return setting->value.str;
  return NULL;
}


Setting *CoreSettings::getSetting( char *name, char type)
{
  String log;
  int settingNo = CoreSettings::search( name );
  if (settingNo < settingNb)
  {
    if (setting[ settingNo ].type == type)
      return &setting[ settingNo ];
    else
    {
      log = name;
      log += F(" is not a ");
      switch (type)
      {
      case SET_TYPE_INT:
        log += "int";
        break;
      case SET_TYPE_STRING:
        log += "String";
        break;
      }
      log += '.';
      CoreLog::add(LOG_LEVEL_ERROR, log);
      return NULL;
    }
  }

  log = name;
  log += F(" does not exists.");
  Serial.println( log.begin() );
  return NULL;
}

void CoreSettings::defaults(String &res, char **blocks)
{
  String log = F("SETS : Loading defaults settings...");
  CoreLog::add(LOG_LEVEL_INFO, log);

  for (int settingNo = 0; settingNo < settingNb; settingNo++)
    setting[ settingNo ].value = setting[ settingNo ].defvalue;
}

void CoreSettings::load(String &res, char **blocks)
{
  // Load settings here

  byte version = CoreSettings::getInt( "prefs.version" );
  String log = F("SETS : Settings version ");
  log += version;
  CoreLog::add(LOG_LEVEL_INFO, log);

  if (version != SETTING_VERSION)
  {
    // Settings version mismatch, try to recover
  }
}

void CoreSettings::save(String &res, char **blocks)
{
  String log;
#ifdef LOG_LEVEL_DEBUG
  log = F("SETS : Saving configuration to flash...");
  CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif
  res += "Settings saved";
  log = F("SETS : ");
  log += res;
  log += '.';
  CoreLog::add(LOG_LEVEL_INFO, log);
}

#define settingName   block[1]
#define settingValue  block[2]

int CoreSettings::search(char *str)
{
  int settingNo;
  for ( settingNo = 0; settingNo < settingNb; settingNo++)
    if (str && !strcmp(str, setting[settingNo].name.begin()))
      break;
  return settingNo;
}

void CoreSettings::display(String &res, int settingNo)
{
  res = setting[ settingNo ].name;
  res += F(" = ");
  switch ( setting[settingNo].type )
  {
    case SET_TYPE_INT:
      res += setting[settingNo].value.i;
      break;
    case SET_TYPE_STRING:
      res += "'";
      res += setting[settingNo].value.str->c_str();
      res += "'";
      break;
  }

  String log  = F("SETS :    ");
  log += res;
  CoreLog::add(LOG_LEVEL_INFO, log);
}

void CoreSettings::set(String &res, char **block)
{
  if (!settingName)
  {
    String log = F("SETS : Settings :");
    CoreLog::add(LOG_LEVEL_INFO, log);
    for ( int settingNo = 0; settingNo < settingNb; settingNo++)
      CoreSettings::display(res, settingNo);
  }
  else
  {
    int settingNo = CoreSettings::search(settingName);
    if (settingNo < settingNb)
    {
      if (settingValue)
        switch ( setting[settingNo].type )
        {
          case SET_TYPE_INT:
            setting[ settingNo ].value.i = atoi(settingValue);
            break;
          case SET_TYPE_STRING:
           *setting[ settingNo ].value.str = settingValue;
            break;
        }

      CoreSettings::display(res, settingNo);
    }
  }
}
