extern "C" {
#include "user_interface.h"
}
#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
// https://github.com/dancol90/ESP8266Ping - GNU LESSER GENERAL PUBLIC LICENSE

#include "CoreWifi.h"
#include "CoreCommands.h"

// Timeouts
#define WIFI_RETRIES              3
#define WIFI_AP_TIMEOUT           2
#define WIFI_WAIT_NONE            0
#define WIFI_WAIT_RECONNECT       30
#define WIFI_WAIT_CONNECT         20
#define WIFI_WAIT_DISCONNECT      5
#define WIFI_WAIT_CONNECTED       10
#define WIFI_WAIT_APMODE          30

// Wifi states :
#define WIFI_STATE_CONFIGURE       0
#define WIFI_STATE_CONNECT         1
#define WIFI_STATE_WAIT_CONNECT    2
#define WIFI_STATE_CONNECTED       3
#define WIFI_STATE_DISCONNECT      4
#define WIFI_STATE_WAIT_DISCONNECT 5
#define WIFI_STATE_DISCONNECTED    6
#define WIFI_STATE_APMODE          7
#define WIFI_STATE_WAIT_APMODE     8

byte wifiWait;
byte wifiState;
//byte wifiRetries;
byte wifiAPWait = 0;

//const byte DNS_PORT = 53;
//IPAddress apIP(192, 168, 204, 1);
//DNSServer dnsServer;

CoreWifi::CoreWifi(void) : CoreBase("CoreWifi")
{
  PANIC_DEBUG();

}

void CoreWifi::setup()
{
  PANIC_DEBUG();

#ifdef LOG_LEVEL_DEBUG
  String log = F("WIFI : Initialization.");
  CoreLog::addLog(LOG_LEVEL_DEBUG, log);
#endif

  CoreCommands::registerCommand( "ping",   CoreWifi::pingCommand, "Pings hostname or IP" );

  CoreCommands::registerCommand( "wifiConnect",    CoreWifi::connectCommand,    "Connects to wifi" );
  CoreCommands::registerCommand( "wifiDisconnect", CoreWifi::disconnectCommand, "Disconnects wifi" );
  CoreCommands::registerCommand( "wifiAP",         CoreWifi::setAPModeCommand,  "Start AP Mode" );
  CoreCommands::registerCommand( "wifiReset",      CoreWifi::resetCommand,      "Reset wifi configuration" );
  CoreCommands::registerCommand( "wifiScan",       CoreWifi::scanAPCommand,     "Scan AP around" );

  CoreSettings::registerSetting( "wifi.ssid",          DEFAULT_SSID );
  CoreSettings::registerSetting( "wifi.pass",          DEFAULT_PWD  );

  CoreSettings::registerSetting( "wifi.ap.ssid",       DEFAULT_AP_SSID );
  CoreSettings::registerSetting( "wifi.ap.pass",       DEFAULT_AP_PWD  );

  CoreSettings::registerSetting( "network.ip",         DEFAULT_IP      );
  CoreSettings::registerSetting( "network.mask",       DEFAULT_NETMASK );
  CoreSettings::registerSetting( "network.gateway",    DEFAULT_GATEWAY );
  CoreSettings::registerSetting( "network.dns",        DEFAULT_DNS     );

  CoreSettings::registerSetting( "network.ap.ip",      DEFAULT_AP_IP      );
  CoreSettings::registerSetting( "network.ap.mask",    DEFAULT_AP_NETMASK );
  CoreSettings::registerSetting( "network.ap.gateway", DEFAULT_AP_GATEWAY );

  wifiWait = WIFI_WAIT_NONE;
  wifiState = WIFI_STATE_CONFIGURE;
}

void CoreWifi::loop(void)
{
  schedule(handleWifi, 1000);
}

//********************************************************************************
// Print WiFi state with known values
//********************************************************************************
void CoreWifi::displayState()
{
  PANIC_DEBUG();

#ifdef LOG_LEVEL_DEBUG_EVEN_MORE
  String log = F("WIFI : State ");
  switch (wifiState)
  {
    case WIFI_STATE_CONFIGURE:
      log += F("WIFI_STATE_CONFIGURE");
      break;

    case WIFI_STATE_CONNECT:
      log += F("WIFI_STATE_CONNECT");
      break;
    case WIFI_STATE_WAIT_CONNECT:
      log += F("WIFI_STATE_WAIT_CONNECT");
      break;
    case WIFI_STATE_CONNECTED:
      log += F("WIFI_STATE_CONNECTED");
      break;

    case WIFI_STATE_DISCONNECT:
      log += F("WIFI_STATE_DISCONNECT");
      break;
    case WIFI_STATE_WAIT_DISCONNECT:
      log += F("WIFI_STATE_WAIT_DISCONNECT");
      break;
    case WIFI_STATE_DISCONNECTED:
      log += F("WIFI_STATE_DISCONNECTED");
      break;

    case WIFI_STATE_APMODE:
      log += F("WIFI_STATE_APMODE");
      break;
    case WIFI_STATE_WAIT_APMODE:
      log += F("WIFI_STATE_WAIT_APMODE");
      break;

    default:
      log += F("WIFI_BAD_STATE");
  }
  log += F(" (");
  log += wifiState;
  log += ')';
  CoreLog::addLog(LOG_LEVEL_DEBUG_EVEN_MORE, log);
#endif
}

//********************************************************************************
// Print WiFi status with known values
//********************************************************************************
void CoreWifi::displayStatus()
{
  PANIC_DEBUG();

#ifdef LOG_LEVEL_DEBUG_EVEN_MORE
  String log = F("WIFI : Status ");
  switch (WiFi.status())
  {
    case WL_IDLE_STATUS:
      log += F("WL_IDLE_STATUS");
      break;
    case WL_NO_SSID_AVAIL:
      log += F("WL_NO_SSID_AVAIL");
      break;
    case WL_CONNECTED:
      log += F("WL_CONNECTED");
      break;
    case WL_CONNECT_FAILED:
      log += F("WL_CONNECT_FAILED");
      break;
    case WL_DISCONNECTED:
      log += F("WL_DISCONNECTED");
      break;
    default:
      log += WiFi.status();
  }
  log += '(';
  log += WiFi.status();
  log += ')';
  CoreLog::addLog(LOG_LEVEL_DEBUG_EVEN_MORE, log);
#endif
}

//********************************************************************************
// Handle WiFi automaton
//********************************************************************************
void CoreWifi::handleWifi()
{
  PANIC_DEBUG();

  String res;

  displayState();
  displayStatus();

  switch (wifiState)
  {
    case WIFI_STATE_CONFIGURE:
      configure();
      break;

    case WIFI_STATE_CONNECT:
      connect();
      break;
    case WIFI_STATE_WAIT_CONNECT:
      waitConnect();
      break;
    case WIFI_STATE_CONNECTED:
      connected();
      break;

    case WIFI_STATE_DISCONNECT:
      disconnect();
      break;
    case WIFI_STATE_WAIT_DISCONNECT:
      waitDisconnect();
      break;
    case WIFI_STATE_DISCONNECTED:
      // When disconnected, we do nothing and stay disconnected.
      // Do we need to switch to AP Mode ???
      break;

    case WIFI_STATE_APMODE:
      setAPMode();
      break;
    case WIFI_STATE_WAIT_APMODE:
      waitAPMode();
      break;

    default:
      // This should never happen !!!
      String log = F("WIFI : Bad state:");
      log += wifiState;
      CoreLog::addLog(LOG_LEVEL_ERROR, log);
      wifiState = WIFI_STATE_CONFIGURE;
  }
}

//********************************************************************************
// Configure Wifi
//********************************************************************************
void CoreWifi::configure()
{
  PANIC_DEBUG();

  String log;

  WiFi.persistent(false); // Do not use SDK storage of SSID/WPA parameters

  configHostname();
  configAP();

  //  wifiRetries = 0;
#ifdef LOG_LEVEL_DEBUG
  log = F("WIFI : Configured.");
  CoreLog::addLog(LOG_LEVEL_DEBUG, log);
#endif

  wifiWait = WIFI_WAIT_NONE;
  wifiState = WIFI_STATE_CONNECT;
}

//********************************************************************************
// Configure Wifi host name
//********************************************************************************
void CoreWifi::configHostname()
{
  PANIC_DEBUG();

  String *hostname = CoreSettings::getString( "system.name" );

  // Replace spaces by '-' to get hostName
  char *tmp = hostname->begin() - 1;
  while (*++tmp) if (*tmp == ' ') *tmp = '-';

#ifdef LOG_LEVEL_DEBUG
  String log = F("WIFI : Hostname is ");
  log += *hostname;
  CoreLog::addLog(LOG_LEVEL_DEBUG, log);
#endif

  wifi_station_set_hostname(hostname->begin());
}

//********************************************************************************
// Configure WifiAP settings
//********************************************************************************
void CoreWifi::configAP()
{
  PANIC_DEBUG();

  String log;
  // create and store unique AP SSID/PW to prevent ESP from starting AP mode with default SSID and No password!
  //sprintf_P(settings.wifiAPSSID, PSTR("%s%u"), DEFAULT_AP_SSID, settings.deviceUnit);

  String *ssid = CoreSettings::getString( "wifi.ap.ssid" );
  (*ssid) += CoreSettings::getInt( "system.unit" );

  String *pass = CoreSettings::getString( "wifi.ap.pass" );

#ifdef LOG_LEVEL_DEBUG
  log = F("WIFI : Configuring AP, SSID:");
  log += *ssid;
  CoreLog::addLog(LOG_LEVEL_DEBUG, log);
#endif

  //WiFi.softAPConfig (settings.ipAP, settings.gatewayAP, settings.netmaskAP );

  if (!WiFi.softAP(ssid->begin(), pass->begin()))
  {
    log = F("WIFI : SoftAp not configured!");
    CoreLog::addLog(LOG_LEVEL_ERROR, log);
  }
}

//********************************************************************************
// Start Wifi AP Mode
//********************************************************************************
void CoreWifi::setAPModeCommand(String &, char **)
{
  PANIC_DEBUG();

  CoreWifi::setAPMode();
}

void CoreWifi::setAPMode(void)
{
  PANIC_DEBUG();

  String log;

  if (wifiWait)
  {
#ifdef LOG_LEVEL_DEBUG_MORE
    log = F("WIFI : Wait in AP mode... ");
    log += wifiWait;
    CoreLog::addLog(LOG_LEVEL_DEBUG_MORE, log);
#endif
    wifiWait--;
    return;
  }

  log += F("WIFI : AP Mode, SSID:");
  log += CoreSettings::getString( "wifi.ap.ssid" )->begin();

  log += F(", Pwd:");
  log += CoreSettings::getString( "wifi.ap.pass" )->begin();

  log += F(", ");
  log += CoreSystem::IPMaskGW(SOFTAP_IF, true, false);
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  if ( !(WiFi.getMode() & WIFI_AP) )
    WiFi.mode(WIFI_AP_STA);

  wifiAPWait = WIFI_AP_TIMEOUT;

  wifiWait = WIFI_WAIT_APMODE;
  wifiState = WIFI_STATE_WAIT_APMODE;
}

//********************************************************************************
// Wait in Wifi AP Mode
//********************************************************************************
void CoreWifi::waitAPMode()
{
  PANIC_DEBUG();

  String log;
  if (wifiWait)
  {
#ifdef LOG_LEVEL_DEBUG_MORE
    log = F("WIFI : Wait in AP mode... ");
    log += wifiWait;
    CoreLog::addLog(LOG_LEVEL_DEBUG_MORE, log);
#endif
    wifiWait--;
    return;
  }

  log = F("WIFI : AP Mode, ");
  log += CoreSystem::IPMaskGW(SOFTAP_IF, true, false);
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  wifiWait = WIFI_WAIT_NONE;
  wifiState = WIFI_STATE_WAIT_CONNECT;
}

//------------------------------------------------------------------------------------------------------------------------
//********************************************************************************
// Connect to Wifi AP
//********************************************************************************
void CoreWifi::connectCommand(String &, char **)
{
  PANIC_DEBUG();

  CoreWifi::connect();
}

void CoreWifi::connect(void)
{
  PANIC_DEBUG();

  String log;

  wifiState = WIFI_STATE_CONNECT;

  if (wifiIsConnected())
  {
    // This should never happen !!!
    log = F("WIFI : Already connected. ");
    log += CoreSystem::IPMaskGW(STATION_IF, true, true);
    //    log += WiFi.localIP().toString();
    CoreLog::addLog(LOG_LEVEL_ERROR, log);

    wifiWait = WIFI_WAIT_CONNECTED;
    wifiState = WIFI_STATE_CONNECTED;
    return;
  }

  if (wifiWait)
  {
#ifdef LOG_LEVEL_DEBUG_MORE
    log = F("WIFI : Waiting connection... ");
    log += wifiWait;
    CoreLog::addLog(LOG_LEVEL_DEBUG_MORE, log);
#endif
    wifiWait--;
    return;
  }

  String *ssid = CoreSettings::getString( "wifi.ssid" );
  String *pass = CoreSettings::getString( "wifi.pass" );

  log = F("WIFI : Connecting to SSID '");
  log += *ssid;
  log += '\'';
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  // IF IPFIXED
  //  String *ip = CoreSettings::getString( "network.ip" );
  //  if (ip && ip[0])
  //  {
  //    log = "ip=";
  //    log += CoreSettings::getString("network.ip");
  //
  //    log = "mask=";
  //    log += CoreSettings::getString("network.netmask");
  //
  //    IPAddress ip     ( CoreSettings::getString("network.ip")     ->begin() );
  //    IPAddress netmask( CoreSettings::getString("network.netmask")->begin() );
  //    IPAddress gateway( CoreSettings::getString("network.gateway")->begin() );
  //    IPAddress dns    ( CoreSettings::getString("network.dns")    ->begin() );
  //
  //    if (!WiFi.config(ip, gateway, netmask, dns))
  //    {
  //#ifdef LOG_LEVEL_DEBUG
  //      log = F("WIFI : Configuration not possible");
  //      CoreLog::add(LOG_LEVEL_DEBUG, log);
  //#endif
  //      wifiWait = WIFI_WAIT_NONE;
  //      wifiState = WIFI_STATE_APMODE;
  //      return;
  //    }
  //  }

  // Starting connection
  if (WiFi.begin(ssid->begin(), pass->begin()) == WL_CONNECT_FAILED)
  {
#ifdef LOG_LEVEL_DEBUG
    log = F("WIFI : Connection impossible,");
    CoreLog::addLog(LOG_LEVEL_DEBUG, log);
#endif
    wifiWait = WIFI_WAIT_NONE;
    wifiState = WIFI_STATE_APMODE;
    return;
  }

  //wifiRetries = WIFI_RETRIES;
  wifiWait  = WIFI_WAIT_CONNECT;
  wifiState = WIFI_STATE_WAIT_CONNECT;

}

//********************************************************************************
// Wait for connection to Wifi AP
//********************************************************************************
void CoreWifi::waitConnect()
{
  PANIC_DEBUG();

  String log;

  if (wifiIsConnected())
  {
    log = F("WIFI : Connected. ");
    log += CoreSystem::IPMaskGW( STATION_IF, true, true);
    CoreLog::addLog(LOG_LEVEL_INFO, log);

    wifiWait = WIFI_WAIT_CONNECTED;
    wifiState = WIFI_STATE_CONNECTED;
    return;
  }

  if (wifiIsNotAvailable())
  {
    log = F("WIFI : SSID not available.");
    CoreLog::addLog(LOG_LEVEL_INFO, log);

    WiFi.disconnect();

    wifiWait = 2;
    wifiState = WIFI_STATE_APMODE;
    return;
  }

  if (wifiWait)
  {
#ifdef LOG_LEVEL_DEBUG
    log = F("WIFI : Waiting connection... ");
    log += wifiWait;
    CoreLog::addLog(LOG_LEVEL_DEBUG_MORE, log);
#endif
    wifiWait--;
    return;
  }

  log = F("WIFI : Retring connection...");
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  WiFi.begin();

  wifiWait = WIFI_WAIT_CONNECT;
}

//********************************************************************************
// Connected to Wifi AP
//********************************************************************************
void CoreWifi::connected()
{
  PANIC_DEBUG();

  String log;
  if (wifiWait)
  {
#ifdef LOG_LEVEL_DEBUG_MORE
    log = F("WIFI : Waiting for connection checking... ");
    log += wifiWait;
    CoreLog::addLog(LOG_LEVEL_DEBUG_MORE, log);
#endif
    wifiWait--;
    return;
  }

  if (!wifiIsConnected())
  {
    log = F("WIFI : Not connected... ");
    log += WiFi.status();
    CoreLog::addLog(LOG_LEVEL_INFO, log);

    // Try to reconnect
    wifiWait = WIFI_WAIT_CONNECT;
    wifiState = WIFI_STATE_CONNECT;
    return;
  }

  if ( wifiIsAP() )
  {
    if (wifiAPWait) // close AP after timeout if a Wifi connection is established...
    {
#ifdef LOG_LEVEL_DEBUG_MORE
      log = F("WIFI : Wait to inactive AP");
      CoreLog::addLog(LOG_LEVEL_DEBUG_MORE, log);
#endif
      wifiAPWait--;
    }
    else
    {
      log = F("WIFI : inactivating AP Mode");
      CoreLog::addLog(LOG_LEVEL_INFO, log);

      WiFi.softAPdisconnect( false );
      WiFi.mode(WIFI_STA);
    }
  }
  else
  {
#ifdef LOG_LEVEL_DEBUG_MORE
    log = F("WIFI : AP Mode is inactive");
    CoreLog::addLog(LOG_LEVEL_DEBUG_MORE, log);
#endif
  }

#ifdef LOG_LEVEL_DEBUG_MORE
  log = F("WIFI : Connection checked... ");
  CoreLog::addLog(LOG_LEVEL_DEBUG_MORE, log);
  displayStatus();
#endif
  wifiWait = WIFI_WAIT_CONNECTED;
}


//********************************************************************************
// Disconnect from Wifi AP
//********************************************************************************
void CoreWifi::disconnectCommand(String &, char**)
{
  PANIC_DEBUG();

  CoreWifi::disconnect();
}

void CoreWifi::disconnect(void)
{
  PANIC_DEBUG();

  String log;

  wifiState = WIFI_STATE_DISCONNECT;

  if (wifiIsDisconnected())
  {
    // This should never happen !!!
    log = F("WIFI : Already disconnected.");
    CoreLog::addLog(LOG_LEVEL_ERROR, log);
    wifiWait = WIFI_WAIT_NONE;
    wifiState = WIFI_STATE_DISCONNECTED;
    return;
  }

  log = F("WIFI : Disconnecting...");
  CoreLog::addLog(LOG_LEVEL_INFO, log);

  //  WiFi.disconnect(); // This scraps the configuration and sets wifi idle
  wifi_station_disconnect(); // we just want to disconnect
  //WiFi.reconnect();

  //  wifiRetries = 0;

  wifiWait = WIFI_WAIT_DISCONNECT;
  wifiState = WIFI_STATE_WAIT_DISCONNECT;
}

//********************************************************************************
// Wait for disconnection from Wifi AP
//********************************************************************************
void CoreWifi::waitDisconnect()
{
  PANIC_DEBUG();

  String log;

  if (wifiIsDisconnected())
  {
    log = F("WIFI : Disconnected.");
    CoreLog::addLog(LOG_LEVEL_INFO, log);
    wifiWait = WIFI_WAIT_NONE;
    wifiState = WIFI_STATE_DISCONNECTED;
    return;
  }

  if (wifiIsInactive())
  {
    log = F("WIFI : Inactive.");
    CoreLog::addLog(LOG_LEVEL_INFO, log);
    wifiWait = WIFI_WAIT_NONE;
    wifiState = WIFI_STATE_DISCONNECTED;
    return;
  }

  if (wifiIsConnected())  /// Pourquoi ?
  {
    wifiWait = WIFI_WAIT_NONE;
    wifiState = WIFI_STATE_WAIT_CONNECT;
    return;
  }
  // if (wifiIsConnected())
  // {
  // IPAddress ip;

  // log = F("WIFI : Connected IP: ");

  // log += IPMaskGW(STATION_IF, true, true);
  // CoreLog::add(LOG_LEVEL_INFO, log);


  // wifiWait = WIFI_WAIT_CONNECTED;
  // wifiState = WIFI_STATE_CONNECTED;
  // return;
  // }

  if (wifiWait)
  {
#ifdef LOG_LEVEL_DEBUG_MORE
    log = F("WIFI : Waiting disconnection... ");
    log += wifiWait;
    CoreLog::addLog(LOG_LEVEL_DEBUG_MORE, log);
#endif
    wifiWait--;
    return;
  }

  wifiWait = WIFI_WAIT_DISCONNECT;
}

//********************************************************************************
// Disconnect from Wifi AP and reset stored SSID/WPA parameters
//********************************************************************************
void CoreWifi::resetCommand(String &, char**)
{
  PANIC_DEBUG();

  String log = F("WIFI : Reset...");
  CoreLog::addLog(LOG_LEVEL_INFO, log);
  WiFi.persistent(true);  // Use SDK storage of SSID/WPA parameters
  WiFi.disconnect();      // this will store empty ssid/wpa into sdk storage
  WiFi.persistent(false); // Do not use SDK storage of SSID/WPA parameters
  wifiState = WIFI_STATE_CONFIGURE;
}

void CoreWifi::pingCommand(String &res, char **block)
{
  PANIC_DEBUG();

  IPAddress ip;
  bool ret;

  if (commandArg(1))
  {
    if ( ip.fromString( commandArg(1) ) )
      ret = Ping.ping(ip, 1);
    else
      ret = Ping.ping(commandArg(1), 1);

    res += "ping ";
    res += commandArg(1);
    res += ' ';
    if (ret)
    {
      res += "Ok (";
      res += Ping.averageTime();
      res += "ms)";
    }
    else res += "Nok";

    String log = F("NET  : ");
    log += res;
    CoreLog::addLog(LOG_LEVEL_INFO, log);
  }
}

void CoreWifi::scanAPCommand(String &res, char **)
{
  PANIC_DEBUG();

  // Scan for wifi AP around
  int n = WiFi.scanNetworks(true /* Async */, true /* showHidden */);

  if (n == 0)
  {
    res = "No AP found";
  }
  else
  {
    res = n;
    res += " AP found : \n";

    for (int i = 0; i < n; i++)
    {
      res += i;
      res += ": CH";
      res += WiFi.channel(i);
      uint8_t enc = WiFi.encryptionType(i);

      switch (enc)
      {
        case ENC_TYPE_WEP :
          res += " Wep";
          break;
        case ENC_TYPE_TKIP :
          res += " Tkip";
          break;
        case ENC_TYPE_CCMP :
          res += " Ccmp";
          break;
        case ENC_TYPE_NONE :
          res += " None";
          break;
        case ENC_TYPE_AUTO :
          res += " Auto";
          break;
        default :
          res += enc;
          break;
      }

      res += " ";
      res += WiFi.SSID(i);
      res += "(";
      res += WiFi.RSSI(i);
      res += "dB)";
      res += "\n";
    }
  }
  CoreLog::addLog(LOG_LEVEL_INFO, res);
}

CoreWifi coreWifi;

