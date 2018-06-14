#include "CoreControllers.h"
#include "CoreLog.h"

Commands CoreControllers::command[commandMax];
int CoreControllers::commandNb;
  
CoreControllers::CoreControllers(void)
{
  coreController.commandNb = 0; 
}


void CoreControllers::addCommand(String cmd, void(*func)(String &, char**), String desc)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String log;

  // Check if command already exists, if so, ignore with log.
  for(int commandNo=0; commandNo < coreController.commandNb; commandNo++)
  {
    if (coreController.command[commandNo].cmd == cmd) 
    {
      log = F("CMDS : Command ");
      log += cmd;
      log += F(" already added.");
      CoreLog::add(LOG_LEVEL_ERROR, log);
      return;
    }
  }
      
  if (coreController.commandNb < commandMax)
  {
    // Add a command to the interpreter
#ifdef LOG_LEVEL_DEBUG_MORE
    log = F("CMDS : Adding command '");
    log += cmd;
    log += '\'';
    CoreLog::add(LOG_LEVEL_DEBUG_MORE, log);
#endif

    coreController.command[coreController.commandNb].cmd = cmd;
    coreController.command[coreController.commandNb].func = func;
    coreController.command[coreController.commandNb].desc = desc;
    coreController.commandNb++;
  }
  else
  {
    log = F("CMDS : Increase commandMax constant.");
    CoreLog::add(LOG_LEVEL_ERROR, log);
  }
}

bool CoreControllers::execute(String &res, String &str)
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

bool CoreControllers::split(String &str, char **block)
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

bool CoreControllers::run(String &res, char **block)
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
  if (commandNo < coreController.commandNb)
  {
#ifdef LOG_LEVEL_DEBUG
    log  = F("CMDS : Command ");
    log += commandNo;
    log += F(" (");
    log += this->command[commandNo].cmd;
    log += F(") Found.");
    CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif

    if (this->command[commandNo].func)
    {
      this->command[commandNo].func(res, block);
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

bool CoreControllers::notFound(String &res, String &line)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  return false;
}

int CoreControllers::search(String str)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  int commandNo;
  for (commandNo = 0; commandNo < coreController.commandNb; commandNo++)
    if (str == this->command[ commandNo ].cmd)
//    if (str && !strcmp( str.begin, command[commandNo].cmd))
      break;
  return commandNo;
}

void CoreControllers::display(int commandNo)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String log = F("CMDS :    ");
  log += this->command[commandNo].cmd;
  log += F(" : ");
  log += this->command[commandNo].desc;
  CoreLog::add(LOG_LEVEL_INFO, log);
}

CoreControllers coreController;

