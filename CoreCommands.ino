#include "CoreCommands.h"
#include "CoreSystem.h"
#include "CoreLog.h"

std::forward_list<CoreCommands*> CoreCommands::commands;
uint CoreCommands::commandNb = 0;

CoreCommands::CoreCommands(void) : CoreBase("CoreCommands")
{
  Serial.begin( 115200 );
  PANIC_DEBUG();

  setPriority( priorityCommand );
}

CoreCommands::CoreCommands(String& cmd, CoreCommand_Callback(func), String& desc) : CoreBase("CoreCommand")
{
  PANIC_DEBUG();

  this->cmd = cmd;
  this->func = func;
  this->desc = desc;
  
  commands.push_front(this); 
  this->commandNb++;
}

void CoreCommands::setup(void)
{
  PANIC_DEBUG();

#ifdef LOG_LEVEL_DEBUG
  String log = F("CMDS : Initialization.");
  CoreLog::addLog(LOG_LEVEL_DEBUG, log);
#endif

  registerCommand( "help",    CoreCommands::help,    "Display help on commands (HELP <cmd>)" );
  registerCommand( "info",    CoreCommands::info,    "Display info on chip" );
  registerCommand( "restart", CoreCommands::restart, "Restarts the ESP" );
  registerCommand( "reboot",  CoreCommands::reboot,  "Reboots the ESP" );
  // idée : Ajouter ici une page web pour envoyer des commandes à partir du web
};

void CoreCommands::registerCommand(String cmd, CoreCommand_Callback(func), String desc)
{
  PANIC_DEBUG();

  String log;

  // Check if command already exists, if so, ignore with log.
  for( CoreCommands* command : commands)
  {
    if (command->cmd == cmd) 
    {
      log = F("CMDS : Command ");
      log += cmd;
      log += F(" already added.");
      CoreLog::addLog(LOG_LEVEL_ERROR, log);
      return;
    }
  }
      
  if (commandNb < commandMax)
  {
    // Add a command to the interpreter
#ifdef LOG_LEVEL_DEBUG_MORE
    log = F("CMDS : Adding command '");
    log += cmd;
    log += '\'';
    CoreLog::addLog(LOG_LEVEL_DEBUG_MORE, log);
#endif

    new CoreCommands(cmd, func, desc);
    return;
  }

  log = F("CMDS : Increase commandMax constant.");
  CoreLog::addLog(LOG_LEVEL_ERROR, log);

  return;
}

bool CoreCommands::execute(String &res, String &str)
{
  PANIC_DEBUG();

  #define blockMax (commandArgs + 1)
  char *block[blockMax];

  // Command grammar :
  // -----------------
  //    line  := sep* command listargs* sep* ;;
  //    listargs := (sep+ arg)* ;;
  //    command := "help"
  //            |  "reboot"
  //            |  "save"
  //            |  ...
  //            ;;
  //
  //    sep := space | , | tab | = | + ;;
  //    arg := quotedstring | integer ;;
  //    quotedstring := string
  //           |  \" string \"
  //           |  \' string \' ;;
  //    integer := [num]+ ;;
  //    string := [char]* ;;
  //
  // TODO : Backslash escapement is not functionnal yet

  if (split(str, block))
  {
    if (!run(res, block))
      return notFound(res, str);
    else return true;
  }
  return false;
}

bool CoreCommands::split(String &str, char **block)
{
  PANIC_DEBUG();

  int blockNb = 0;
  char *line = str.begin();

  // Initialize blocks.
  // block[0] = command, block[1..blockMax-1] = args
  for (int blockNo = 0; blockNo < blockMax; blockNo++)
    block[blockNo] = nullptr;

  // Split line in blocks (ignore extra blocks > blockMax)
  while ( *line && (blockNb < blockMax) )
  {
    // Skip separator(s) at beginning of block
    while ( *line && (*line == '\t' || *line == ' ' || *line == ',' || *line == '=') ) line++;

    if (*line)
    {
      // Check for quotes
      char quote = '\0';
      if (*line == '"' || *line == '\'') quote = *line++;
  
      // Store block start addr
      block[blockNb++] = line;
  
      if (quote)
      {
        // Search for closing quote ignoring all separators in between
        while (*line && (*line != quote)) line++;

        // Whatever it's a quote or a \0, we close the block with \0.
        *line++ = '\0';
      }
      else
      {
        // Search for next separator
        while ( *line && (*line != '\t' && *line != ' ' && *line != ',' && *line != '=') ) line++;
  
        // if *line is not null, close string with \0 and go to next char as there may be more after.
        if (*line) *line++ = '\0';
      }
    }
  }
  return true;
}

bool CoreCommands::run(String &res, char **block)
{
  PANIC_DEBUG();

  String log;

#ifdef LOG_LEVEL_DEBUG
    log  = F("CMDS : search command '");
    log += commandName;
    log += "'";
    CoreLog::addLog(LOG_LEVEL_DEBUG, log);
#endif

  CoreCommands* command = search( commandName );
  if (command)
  {
#ifdef LOG_LEVEL_DEBUG
    log  = F("CMDS : Command ");
    log += command->cmd;
    log += F(" (");
    log += command->desc;
    log += F(") Found.");
    CoreLog::addLog(LOG_LEVEL_DEBUG, log);
#endif

    if (command->func)
    {
      command->func(res, block);
      return true;
    }
    else
    {
      res = "Unknown function for command '";
      res += commandName;
      res += "'";
    }
  }
  else
  {
    res = "Unknown command '";
    res += commandName;
    res += "'";
  }
  
  log = "CMDS : ";
  log += res;
  log += ", try 'help'.";

  CoreLog::addLog(LOG_LEVEL_ERROR, log);
  return false;
}

bool CoreCommands::notFound(String &, String &)
{
  PANIC_DEBUG();

  return false;
}

void CoreCommands::info(String &, char **)
{
  PANIC_DEBUG();

  String head = F("CMDS :    ");
  String log = F("CMDS : Informations :");
  CoreLog::addLog(LOG_LEVEL_INFO, log);
 
  log = head;
  log += F("loadAverage : ");
  log = CoreSystem::getLoadAverage();
  log += F("% ( ");
  log += CoreSystem::getLoopCounterLast();
  log += F(" / ");
  log += CoreSystem::getLoopCounterMax();
  log += F(" )");  
  CoreLog::addLog(LOG_LEVEL_INFO, log);
  
  log = head;
  log += F("getCycleCount : ");
  log += ESP.getCycleCount();
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  log = head;
  log += F("VCC : ");
  log += ESP.getVcc();
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Free Heap : ");
  log += ESP.getFreeHeap();
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Chip ID : ");
  log += ESP.getChipId();
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  log = head;
  log += F("SDK version : ");
  log += ESP.getSdkVersion();
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Core version : ");
  log += ESP.getCoreVersion();
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Full version : ");
  log += ESP.getFullVersion();
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Boot version : ");
  log += ESP.getBootVersion();
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Boot mode : ");
  log += ESP.getBootMode();
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Frequency : ");
  log += ESP.getCpuFreqMHz();
  log += F(" MHz");
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Flash Chip ID : ");
  log += ESP.getFlashChipId();
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Flash real size : ");
  log += ESP.getFlashChipRealSize();
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Flash set size : ");
  log += ESP.getFlashChipSize();
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Flash speed : ");
  log += ESP.getFlashChipSpeed();
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Flash mode : ");
  log += ESP.getFlashChipMode();
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Flash Chip Size by ID : ");
  log += ESP.getFlashChipSizeByChipId();
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Sketch size : ");
  log += ESP.getSketchSize();
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Sketch MD5 : ");
  log += ESP.getSketchMD5();
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Sketch Free Space : ");
  log += ESP.getFreeSketchSpace();
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Reset reason : ");
  log += ESP.getResetReason();
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  log = head;
  log += F("Reset Info : ");
  log += ESP.getResetInfo();
  CoreLog::addLog(LOG_LEVEL_INFO, log);
}

void CoreCommands::restart(String &, char **)
{
  PANIC_DEBUG();

  String log;
  log = F("SYST : Restarting the device...");
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  ESP.reset();
}

void CoreCommands::reboot(String &, char **)
{
  PANIC_DEBUG();

  String log;
  log = F("SYST : Rebooting the device...");
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  ESP.reset();
}

CoreCommands* CoreCommands::search(String str)
{
  PANIC_DEBUG();

  for (CoreCommands* command : commands)
    if (str == command->cmd)
      return command;
  return NULL;
}

void CoreCommands::display(String &res, CoreCommands* command)
{
  PANIC_DEBUG();

  res += command->cmd;
  res += F(" : ");
  res += command->desc;
}

void CoreCommands::help(String &res, char **block)
{
  PANIC_DEBUG();

  if (!commandArg(1))
  {
    res += commandNb;
    res += F(" commands :\n");
    for (CoreCommands* command : commands)
    {
      CoreCommands::display(res, command);
      res += '\n';
    }
  }
  else
  {
    CoreCommands* command = CoreCommands::search( commandArg(1) );
    if (command) CoreCommands::display(res, command);
  }
}

CoreCommands coreCommands;

