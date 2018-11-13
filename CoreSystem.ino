#include "CoreSystem.h"
#include "CoreWifi.h"

// Load average vars
ulong CoreSystem::loopCounter = 0;
ulong CoreSystem::loopCounterLast = 0;
ulong CoreSystem::loopCounterMax  = 1;

CoreSystem::CoreSystem(void) : CoreBase("CoreSystem")
{
  PANIC_DEBUG();

  setPriority( prioritySystem );
}

void CoreSystem::setup(void)
{
  Serial.begin( 115200 );
  Serial.println();
  PANIC_DEBUG();
  Serial.print("Booting... ");
  delay(500);
  Serial.println("Ok.");
  
  CoreSettings::registerSetting( "system.name",  DEFAULT_NAME     );
  CoreSettings::registerSetting( "system.unit",  DEFAULT_UNIT     );
  CoreSettings::registerSetting( "system.pass",  DEFAULT_PASSWORD );
}

void CoreSystem::loop(void)
{
//  PANIC_DEBUG();
  { // handle load average
    loopCounter++;
  }

  schedule( handleLoadAverage, 30000 );
}

void CoreSystem::handleLoadAverage(void)
{
  PANIC_DEBUG();

  { // Handle load everage
    loopCounterLast = loopCounter;
    if (loopCounterLast > loopCounterMax)
      loopCounterMax = loopCounterLast;
//    else
//      loopCounterMax -= (loopCounterMax - loopCounterLast) / 10;

    loopCounter = 0;
  }
  
  { // Print Uptime to know everything works
    String log = "";

    log += "SYST : Uptime ";
    CoreSystem::format_time(log, millis() / 1000 );

    log += F(", Load avg ");
    log += CoreSystem::getLoadAverage();
    log += '%';

    log += F(", FreeMem ");
    log += ESP.getFreeHeap();

    CoreLog::addLog(LOG_LEVEL_INFO, log);
  }
  
  ledLoop();
}

#define ledPin LED_BUILTIN
#define divisor 100

void CoreSystem::ledSetup(void)
{
  PANIC_DEBUG();

  pinMode(ledPin, OUTPUT);
}

void CoreSystem::ledLoop(void)
{
  PANIC_DEBUG();

  static int count = 0;
  int numerator = (wifiIsConnected()) ? divisor - 10 : 5;

  if (!(count % divisor))
    digitalWrite(ledPin, HIGH);
  else if (!((count + divisor - numerator) % divisor))
    digitalWrite(ledPin, LOW);
  
  count = (count+1) % divisor;
}

ulong CoreSystem::getLoadAverage()
{
  return 100 - (100 * loopCounterLast / loopCounterMax);
}

ulong CoreSystem::getLoopCounterLast(void)
{
  return loopCounterLast;
}

ulong CoreSystem::getLoopCounterMax(void)
{
  return loopCounterMax;
}

void CoreSystem::format_time(String &str, unsigned long now)// This should return a String !!!
{
  uint8_t an, se, j, h, m, s;
  char buffer[40];

  s = now % 60; now /= 60;
  m = now % 60; now /= 60;
  h = now % 60; now /= 60;
  j = now % 24; now /= 24;
  se = now % 7; now /= 7;
  an = now % 52; now /= 52;

  if      (an) sprintf_P(buffer, PSTR("%uan%usem%01uj%02uh%02um%02us"), an, se, j, h, m, s);
  else if (se) sprintf_P(buffer, PSTR("%usem%01uj%02uh%02um%02us"), se, j, h, m, s);
  else if (j)  sprintf_P(buffer, PSTR("%uj%02uh%02um%02us"), j, h, m, s);
  else if (h)  sprintf_P(buffer, PSTR("%uh%02um%02us"), h, m, s);
  else if (m)  sprintf_P(buffer, PSTR("%um%02us"), m, s);
  else         sprintf_P(buffer, PSTR("%us"), s);

  str += buffer;
}

String CoreSystem::IPMaskGW(int interface, bool mask, bool gw)
{
    struct ip_info if_ip;
    IPAddress ip;
    String log;

    wifi_get_ip_info(interface, &if_ip);
    
    log = F("IP:");
    ip = if_ip.ip.addr;
    log += ip.toString();
    
    if (mask)
    {
      log += '/';
      log += cidr(if_ip.netmask.addr);
    }
    
    if (gw)
    {
      log += F(", GW:");
      
      ip = if_ip.gw.addr;
      log += ip.toString();
    }
    
    return log;
}

uint8_t CoreSystem::cidr(IPAddress ip)
{
  uint8_t count=32;
  uint32_t mask = (uint32_t)ip;

  // count-- the number of zeros from the right (LSB), does not check for 1's after.
  while( count && !(mask % 2) )
  {
    count--;
    mask>>=1;
  }

  return count;
}

#include "build_defs.h"

unsigned long CoreSystem::getVersion()
{
  // Oupout format : YYYYMMDD
  return (BUILD_YEAR  * 10000) + 
         (BUILD_MONTH * 100) +
         (BUILD_DAY   * 1);
}

unsigned long CoreSystem::getBuild()
{
  // Oupout format : HHMMSS
  return (BUILD_HOUR  * 10000) +
         (BUILD_MIN   * 100) +
         (BUILD_SEC   * 1);
}

CoreSystem coreSystem;

