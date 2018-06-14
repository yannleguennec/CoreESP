#include "CoreCommands.h"


void CoreCommands::setup(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
#ifdef LOG_LEVEL_DEBUG
  String log = F("CMDS : Initialization.");
  CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif

  addCommand( "help",    CoreCommands::help,    "Display help on commands (HELP <cmd>)" );
  addCommand( "info",    CoreCommands::info,    "Display info on chip" );
  addCommand( "restart", CoreCommands::restart, "Restarts the ESP" );
  addCommand( "reboot",  CoreCommands::reboot,  "Reboots the ESP" );
  // idée : Ajouter ici une page web pour envoyer des commandes à partir du web
};

void CoreCommands::info(String &res, char **block)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String head = F("CMDS :    ");
  String log = F("CMDS : Informations :");
  CoreLog::add(LOG_LEVEL_INFO, log);

  log = head;
  log += F("VCC : ");
  log += ESP.getVcc();
  CoreLog::add(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Free Heap : ");
  log += ESP.getFreeHeap();
  CoreLog::add(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Chip ID : ");
  log += ESP.getChipId();
  CoreLog::add(LOG_LEVEL_INFO, log);

  log = head;
  log += F("SDK version : ");
  log += ESP.getSdkVersion();
  CoreLog::add(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Core version : ");
  log += ESP.getCoreVersion();
  CoreLog::add(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Full version : ");
  log += ESP.getFullVersion();
  CoreLog::add(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Boot version : ");
  log += ESP.getBootVersion();
  CoreLog::add(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Boot mode : ");
  log += ESP.getBootMode();
  CoreLog::add(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Frequency : ");
  log += ESP.getCpuFreqMHz();
  log += F(" MHz");
  CoreLog::add(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Flash Chip ID : ");
  log += ESP.getFlashChipId();
  CoreLog::add(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Flash real size : ");
  log += ESP.getFlashChipRealSize();
  CoreLog::add(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Flash set size : ");
  log += ESP.getFlashChipSize();
  CoreLog::add(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Flash speed : ");
  log += ESP.getFlashChipSpeed();
  CoreLog::add(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Flash mode : ");
  log += ESP.getFlashChipMode();
  CoreLog::add(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Flash Chip Size by ID : ");
  log += ESP.getFlashChipSizeByChipId();
  CoreLog::add(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Sketch size : ");
  log += ESP.getSketchSize();
  CoreLog::add(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Sketch MD5 : ");
  log += ESP.getSketchMD5();
  CoreLog::add(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Sketch Free Space : ");
  log += ESP.getFreeSketchSpace();
  CoreLog::add(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Reset reason : ");
  log += ESP.getResetReason();
  CoreLog::add(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Reset Info : ");
  log += ESP.getResetInfo();
  CoreLog::add(LOG_LEVEL_INFO, log);
}

void CoreCommands::restart(String &res, char **block)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String log;
  log = F("SYST : Restarting the device...");
  CoreLog::add(LOG_LEVEL_INFO, log);

  ESP.reset();
}

void CoreCommands::reboot(String &res, char **block)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String log;
  log = F("SYST : Rebooting the device...");
  CoreLog::add(LOG_LEVEL_INFO, log);

  ESP.reset();
}

void CoreCommands::help(String &res, char **block)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  int commandNo;
  String log;

  if (!commandArg(1))
  {
    log = F("CMDS : Commands :");
    CoreLog::add(LOG_LEVEL_INFO, log);
    for (commandNo = 0; commandNo < coreController.commandNb; commandNo++)
      coreCommands.display(commandNo);
  }
  else
  {
    commandNo = coreController.search( commandArg(1) );
    if (commandNo < coreController.commandNb)
      coreController.display(commandNo);
  }
}

CoreCommands coreCommands;


