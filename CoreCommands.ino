#include "CoreCommands.h"

Commands CoreCommands::command[commandMax];
int CoreCommands::commandNb;

CoreCommands::CoreCommands(void)
{
  commandNb = 0; 
}

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

void CoreCommands::addCommand(String cmd, void(*func)(String &, char**), String desc)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String log;

  // Check if command already exists, if so, ignore with log.
  for(int commandNo=0; commandNo < commandNb; commandNo++)
  {
    if (command[commandNo].cmd == cmd) 
    {
      log = F("CMDS : Command ");
      log += cmd;
      log += F(" already added.");
      CoreLog::add(LOG_LEVEL_ERROR, log);
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
    CoreLog::add(LOG_LEVEL_DEBUG_MORE, log);
#endif

    command[commandNb].cmd = cmd;
    command[commandNb].func = func;
    command[commandNb].desc = desc;
    commandNb++;
  }
  else
  {
    log = F("CMDS : Increase commandMax constant.");
    CoreLog::add(LOG_LEVEL_ERROR, log);
  }
}

bool CoreCommands::execute(String &res, String &str)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
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
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
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
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String log;

#ifdef LOG_LEVEL_DEBUG
    log  = F("CMDS : search command '");
    log += commandName;
    log += "'";
    CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif

  int commandNo = search( commandName );
  if (commandNo < commandNb)
  {
#ifdef LOG_LEVEL_DEBUG
    log  = F("CMDS : Command ");
    log += commandNo;
    log += F(" (");
    log += command[commandNo].cmd;
    log += F(") Found.");
    CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif

    if (command[commandNo].func)
    {
      command[commandNo].func(res, block);
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

  CoreLog::add(LOG_LEVEL_ERROR, log);
  return false;
}

bool CoreCommands::notFound(String &, String &)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  return false;
}

int CoreCommands::search(String str)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  int commandNo;
  for (commandNo = 0; commandNo < commandNb; commandNo++)
    if (str == command[ commandNo ].cmd)
//    if (str && !strcmp( str.begin, command[commandNo].cmd))
      break;
  return commandNo;
}

void CoreCommands::display(int commandNo)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String log = F("CMDS :    ");
  log += command[commandNo].cmd;
  log += F(" : ");
  log += command[commandNo].desc;
  CoreLog::add(LOG_LEVEL_INFO, log);
}


void CoreCommands::info(String &, char **)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String head = F("CMDS :    ");
  String log = F("CMDS : Informations :");
  CoreLog::add(LOG_LEVEL_INFO, log);
 
  log = head;
  log += F("loadAverage : ");
  log = CoreSystem::getLoadAverage();
  log += F("% ( ");
  log += CoreSystem::getLoopCounterLast();
  log += F(" / ");
  log += CoreSystem::getLoopCounterMax();
  log += F(" )");  
  CoreLog::add(LOG_LEVEL_INFO, log);
  
  log = head;
  log += F("getCycleCount : ");
  log += ESP.getCycleCount();
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

void CoreCommands::restart(String &, char **)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String log;
  log = F("SYST : Restarting the device...");
  CoreLog::add(LOG_LEVEL_INFO, log);

  ESP.reset();
}

void CoreCommands::reboot(String &, char **)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String log;
  log = F("SYST : Rebooting the device...");
  CoreLog::add(LOG_LEVEL_INFO, log);

  ESP.reset();
}

void CoreCommands::help(String &, char **block)
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
    for (commandNo = 0; commandNo < commandNb; commandNo++)
      coreCommand.display(commandNo);
  }
  else
  {
    commandNo = coreCommand.search( commandArg(1) );
    if (commandNo < commandNb)
      coreCommand.display(commandNo);
  }
}

CoreCommands coreCommand;


