#include "CoreSettings.h"

#define settingMax 50
std::forward_list<CoreSettings*> CoreSettings::settings;
uint CoreSettings::settingNb = 0;

CoreSettings::CoreSettings(void) : CoreBase("CoreSettings")
{
  PANIC_DEBUG();

  // No log here, as CoreLog not initialized
  // These are important settings needed from the start.
//  CoreSettings::registerSetting( "log.serialLoglevel",    LOG_LEVEL_DEBUG );
//  CoreSettings::registerSetting( "log.mqttLoglevel",      LOG_LEVEL_INFO );
//  CoreSettings::registerSetting( "log.syslogLoglevel",    LOG_LEVEL_INFO );
//  CoreSettings::registerSetting( "log.httpLoglevel",      LOG_LEVEL_INFO );
}

CoreSettings::CoreSettings(const char *name, char type) : CoreBase("CoreSettings")
{
  PANIC_DEBUG();

  this->name = name;
  this->type = type;
  settings.push_front(this); 
  this->settingNb++;

}

void CoreSettings::setup(void)
{
  PANIC_DEBUG();

#ifdef LOG_LEVEL_DEBUG
  String log = F("SETS : Initialization.");
  CoreLog::addLog(LOG_LEVEL_DEBUG, log );
#endif

  CoreCommands::registerCommand( "set",    CoreSettings::set,    "Modify or display settings value"     );
  CoreCommands::registerCommand( "load",   CoreSettings::load,   "Loads configuration from flash"       );
  CoreCommands::registerCommand( "save",   CoreSettings::save,   "Saves current configuration to flash" );


}

void CoreSettings::registerSetting(const char *name, int value)
{
  PANIC_DEBUG();

  CoreSettings* setting = CoreSettings::registerSetting(name, SET_TYPE_INT);
  if (setting)
  {
    setting->defvalue.i = value;
    setting->value.i = value;
  }
}

void CoreSettings::registerSetting(const char *name, const char *value)
{
  PANIC_DEBUG();

  CoreSettings* setting = CoreSettings::registerSetting(name, SET_TYPE_STRING);
  if (setting)
  {
    setting->defvalue.str = new String(value);
    setting->value.str = new String(value);
  }
}

void CoreSettings::registerSetting(const char *name, String &value)
{
  PANIC_DEBUG();

  CoreSettings* setting = CoreSettings::registerSetting(name, SET_TYPE_STRING);
  if (setting)
  {
    setting->defvalue.str = new String(value);
    setting->value.str = new String(value);
  }
}

// Create a généric setting
CoreSettings* CoreSettings::registerSetting(const char *name, char type)
{
  PANIC_DEBUG();

  String log;

  for(CoreSettings* setting : settings)
  {
    if (setting->name == name) 
    {
      log = F("CMDS : Setting ");
      log += name;
      log += F(" already added.");
      CoreLog::addLog(LOG_LEVEL_ERROR, log);
      
      return NULL;
    }
  }

  if (settingNb < settingMax)
  {
    // Add a command to the interpreter
#ifdef LOG_LEVEL_DEBUG_MORE
    log = F("CMDS : Adding setting '");
    log += name;
    log += '\'';
    CoreLog::addLog(LOG_LEVEL_DEBUG_MORE, log);
#endif

    return new CoreSettings(name, type);
  }

  // This should be a log error
  Serial.println(F("SETS : Increase settingMax."));

  return NULL;
}

int CoreSettings::getInt( const char *name )
{
  PANIC_DEBUG();

  CoreSettings *setting = CoreSettings::getSetting( name, SET_TYPE_INT );
  if (setting) return setting->value.i;
  return 0;
}

char *CoreSettings::getChar( const char *name )
{
  PANIC_DEBUG();

  CoreSettings *setting = CoreSettings::getSetting( name, SET_TYPE_STRING );
  if (setting) return setting->value.str->begin();
  return NULL;
}

String *CoreSettings::getString( const char *name )
{
  PANIC_DEBUG();

  CoreSettings *setting = CoreSettings::getSetting( name, SET_TYPE_STRING );
  if (setting) return setting->value.str;
  return NULL;
}


CoreSettings *CoreSettings::getSetting( const char *name, char type)
{
  PANIC_DEBUG();

  String log;
  CoreSettings* setting = CoreSettings::search( name );
  if (setting)
  {
    if (setting->type == type)
      return setting;
    else
    {
      log = setting->name;
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
      CoreLog::addLog(LOG_LEVEL_ERROR, log);
      return NULL;
    }
  }

  log = name;
  log += F(" does not exists.");
  Serial.println( log.begin() );
  return NULL;
}

void CoreSettings::defaults(String &res, char **)
{
  PANIC_DEBUG();

  res += F("Defaults settings loaded.");
  String log = F("SETS : Loading defaults settings...");
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  for (CoreSettings* setting : settings)
    setting->value = setting->defvalue;
}

void CoreSettings::load(String &, char **)
{
  PANIC_DEBUG();

  // Load settings here

  byte version = CoreSettings::getInt( "prefs.version" );
  String log = F("SETS : Settings version ");
  log += version;
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  if (version != SETTING_VERSION)
  {
    // Settings version mismatch, try to recover
  }
}

void CoreSettings::save(String &res, char **)
{
  PANIC_DEBUG();

  String log;
#ifdef LOG_LEVEL_DEBUG
  log = F("SETS : Saving configuration to flash...");
  CoreLog::addLog(LOG_LEVEL_DEBUG, log);
#endif
  res += F("Settings saved");
  log = F("SETS : ");
  log += res;
  log += '.';
  CoreLog::addLog(LOG_LEVEL_INFO, log);
}

CoreSettings* CoreSettings::search( const char *name)
{
  PANIC_DEBUG();

  for( CoreSettings* setting : settings )
    if (setting->name == name)
      return setting;
  return NULL;
}

void CoreSettings::display(String &res, CoreSettings* setting)
{
  PANIC_DEBUG();

  String log = setting->name;
  log += F(" = ");
  switch ( setting->type )
  {
    case SET_TYPE_INT:
      log += setting->value.i;
      break;
    case SET_TYPE_STRING:
      log += "'";
      log += setting->value.str->c_str();
      log += "'";
      break;
  }
  res += log + '\n';
  
//  log  = String("SETS :    ") + log;
//  CoreLog::add(LOG_LEVEL_INFO, log);
}

#define settingName   block[1]
#define settingValue  block[2]

void CoreSettings::set(String &res, char **block)
{
  PANIC_DEBUG();

  if (!settingName)
  {
    for ( CoreSettings* setting : settings )
      CoreSettings::display(res, setting);
  }
  else
  {
    CoreSettings* setting = CoreSettings::search(settingName);
    if (setting)
    {
      if (settingValue)
        switch ( setting->type )
        {
          case SET_TYPE_INT:
            setting->value.i = atoi(settingValue);
            break;
          case SET_TYPE_STRING:
           *setting->value.str = settingValue;
            break;
        }

      CoreSettings::display(res, setting);
    }
  }
}
