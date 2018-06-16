#include "CoreSettings.h"

#define settingMax 50
Setting setting[settingMax];
int settingNb = 0;
int settingNo = 0;

void CoreSettings::init(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  // No log here, as CoreLog not initialized
  // These are important settings needed from the start.
  CoreSettings::add( "log.serialLoglevel",    SET_TYPE_INT,     LOG_LEVEL_DEBUG );
  CoreSettings::add( "log.mqttLoglevel",      SET_TYPE_INT,     LOG_LEVEL_INFO );
  CoreSettings::add( "log.syslogLoglevel",    SET_TYPE_INT,     LOG_LEVEL_INFO );
  CoreSettings::add( "log.httpLoglevel",      SET_TYPE_INT,     LOG_LEVEL_INFO );
}

void CoreSettings::setup(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
#ifdef LOG_LEVEL_DEBUG
  String log = F("SETS : Initialization.");
  CoreLog::add(LOG_LEVEL_DEBUG, log );
#endif

  coreCommand.addCommand( "set",    CoreSettings::set,    "Modify or display settings value"     );
  coreCommand.addCommand( "load",   CoreSettings::load,   "Loads configuration from flash"       );
  coreCommand.addCommand( "save",   CoreSettings::save,   "Saves current configuration to flash" );

  CoreSettings::add( "system.name",        SET_TYPE_STRING,  DEFAULT_NAME     );
  CoreSettings::add( "system.unit",        SET_TYPE_INT,     DEFAULT_UNIT     );
  CoreSettings::add( "system.pass",        SET_TYPE_STRING,  DEFAULT_PASSWORD );

}

void CoreSettings::add(char *name, char type, int value)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  int settingNo = CoreSettings::add(name, type);
  if (settingNo < settingNb)
    setting[ settingNo ].defvalue.i = setting[ settingNo ].value.i = value;
}

void CoreSettings::add(char *name, char type, char *value)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  int settingNo = CoreSettings::add(name, type);
  if (settingNo < settingNb)
  {
    setting[ settingNo ].defvalue.str = new String(value);
    setting[ settingNo ].value.str = new String(value);
  }
}

void CoreSettings::add(char *name, char type, String &value)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  int settingNo = CoreSettings::add(name, type);
  if (settingNo < settingNb)
  {
    setting[ settingNo ].defvalue.str = new String(value);
    setting[ settingNo ].value.str = new String(value);
  }
}


int CoreSettings::add(char *name, char type)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
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
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  settingNo = 0;
  return &setting[ settingNo ];
}

Setting *CoreSettings::next(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  if (++settingNo < settingNb)
    return &setting[ settingNo ];
  return NULL;
}

int CoreSettings::getInt( char *name )
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  Setting *setting = CoreSettings::getSetting( name, SET_TYPE_INT );
  if (setting) return setting->value.i;
  return 0;
}

char *CoreSettings::getChar( char *name )
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  Setting *setting = CoreSettings::getSetting( name, SET_TYPE_STRING );
  if (setting) return setting->value.str->begin();
  return NULL;
}

String *CoreSettings::getString( char *name )
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  Setting *setting = CoreSettings::getSetting( name, SET_TYPE_STRING );
  if (setting) return setting->value.str;
  return NULL;
}


Setting *CoreSettings::getSetting( char *name, char type)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
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
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String log = F("SETS : Loading defaults settings...");
  CoreLog::add(LOG_LEVEL_INFO, log);

  for (int settingNo = 0; settingNo < settingNb; settingNo++)
    setting[ settingNo ].value = setting[ settingNo ].defvalue;
}

void CoreSettings::load(String &res, char **blocks)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
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
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String log;
#ifdef LOG_LEVEL_DEBUG
  log = F("SETS : Saving configuration to flash...");
  CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif
  res += F("Settings saved");
  log = F("SETS : ");
  log += res;
  log += '.';
  CoreLog::add(LOG_LEVEL_INFO, log);
}

int CoreSettings::search(char *str)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  int settingNo;
  for ( settingNo = 0; settingNo < settingNb; settingNo++)
    if (str && !strcmp(str, setting[settingNo].name.begin()))
      break;
  return settingNo;
}

void CoreSettings::display(String &res, int settingNo)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
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

#define settingName   block[1]
#define settingValue  block[2]

void CoreSettings::set(String &res, char **block)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
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
