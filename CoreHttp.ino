#include "user_interface.h"

#include "CoreDefaults.h"
#include "CoreHttp.h"
#include "CoreLog.h"
#include "CoreSettings.h"

ESP8266WebServer WebServer(80);

boolean WebLoggedIn = false;
int WebLoggedInTimer = 300;
//String webStatusMessage = "";
const __FlashStringHelper* texthtml;
const __FlashStringHelper* ignoreValue;
byte loginTimeout = 0; // Not logged in at start
int lineNo = 0;
int currentMenu = 0;

void CoreHttp::add(String url, void (*func)(void))
{
  WebServer.on( url.begin(), func );
}

void CoreHttp::setup()
{
#ifdef LOG_LEVEL_DEBUG
  String log = F("HTTP : Initialization.");
  CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif

  // Initialize constants
  texthtml = F("text/html");
  ignoreValue = F("*** ignore ***");

  // Prepare webserver pages
  CoreHttp::add( "/",        CoreHttp::handleRoot    );
  CoreHttp::add( "/log",     CoreHttp::handleLog     );
  CoreHttp::add( "/config",  CoreHttp::handleConfig  );
  CoreHttp::add( "/tools",   CoreHttp::handleTools   );

  /*  WebServer.on("/hardware", handle_hardware);
    WebServer.on("/devices", handle_devices);
    WebServer.on("/i2cscanner", handle_i2cscanner);
    WebServer.on("/wifiscanner", handle_wifiscanner);
    WebServer.on("/control", handle_control);
    WebServer.on("/download", handle_download);
    WebServer.on("/upload", HTTP_GET, handle_upload);
    WebServer.on("/upload", HTTP_POST, handle_upload_post, handleFileUpload);
    WebServer.onNotFound(handleNotFound);
    #if FEATURE_SPIFFS
    WebServer.on("/filelist", handle_filelist);
    #else
    WebServer.on("/esp.css", handle_css);
    #endif
    WebServer.on("/advanced", handle_advanced);
    WebServer.on("/setup", handle_setup);
    WebServer.on("/json", handle_json);
    WebServer.on("/rules", handle_rules);

    if (ESP.getFlashChipRealSize() > 524288)
    httpUpdater.setup(&WebServer);

    #if FEATURE_SSDP
    if (Settings.UseSSDP)
    {
    WebServer.on("/ssdp.xml", HTTP_GET, []() {
      SSDP_schema(WebServer.client());
    });
    SSDP_begin();
    }
    #endif
  */

  WebServer.begin();
}

inline void CoreHttp::loop()
{
  WebServer.handleClient();
}

inline void CoreHttp::loopMedium()
{
  if (loginTimeout)
  {
    String log;
    
    loginTimeout--;
#ifdef LOG_LEVEL_DEBUG
    log = "HTTP : loginTimeout... ";
    log += loginTimeout;
    CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif
    if (!loginTimeout)
    {
      log = "HTTP : Logged out.";
      CoreLog::add(LOG_LEVEL_INFO, log);
    }
  }
}

void CoreHttp::pageHeader(String& reply, int activeMenu)
{
  reply += F(       "<!DOCTYPE html>"      );
  reply += F(       "<html>"      );
  reply += F(       "<head><title>" );
  reply += CoreSettings::getChar( "system.name" );
  reply += F(       "</title>"      );
  reply += F(       "<style>"     );
  reply += F(         "html body{font-family:Sans-Serif;font-size:12px;padding:0;margin:0;} "     );
  reply += F(         ".table{width:94%;text-align:center;background:#eec;border-collapse:collapse;border-top:7px solid #9af;border-bottom:7px solid #9af;border-right:1px solid #dde;border-left:1px solid #dde;margin:5px;} "     );
  reply += F(         ".table th{font-size:15px;font-weight:bold;background:#ccc;color:#666;padding:8px;} "     );
  reply += F(         ".table tr{border-bottom:1px solid #ddd;} "     );
  reply += F(         ".table tr.odd{background:#eed;} "     );
  reply += F(         ".table tr:hover{background:#dde;} "     );
  reply += F(         ".table tr td{width:50%;color:#666;padding:4px;} "     );
  reply += F(         ".table tr td:first-child{white-space:nowrap;font-weight:bold;} "     );
  reply += F(         ".page{margin-left:150px;padding:5px;} "     );
  reply += F(         ".menu{list-style-type:none;margin:0;padding:0;width:130px;background-color:#f1f1f1;height:100%;position:fixed;overflow:auto;white-space:nowrap;} "     );
  reply += F(         ".menu li{text-align:center;border-bottom:1px solid #dde;} "     );
  reply += F(         "li:last-child{border-bottom:none;} "     );
  reply += F(         ".menu li.title{font-size:15px;font-weight:bold;background:#888;color:#ddd;padding:8px;} "     );
  reply += F(         ".menu li a{font-size: 13px;display: block;color: #999;padding:8px 16px;text-decoration:none;} "     );
  reply += F(         ".menu li a.active{background:#e8e8e8;color:#666;font-weight:bold;} "     );
  reply += F(         ".menu li a:hover{background:#dde;color:#666;} "     );
  reply += F(       "</style>"    );
  reply += F(       "</head>"     );
  reply += F(       "<body>"      );

  reply += F("<ul class='menu'>"     );
  reply += F(  "<li class='title'>");
  reply += CoreSettings::getChar( "system.name" );
  reply += F("</li>"     );
  
  currentMenu = 0;
  menuItem( reply, ".",        "Information", activeMenu);
  menuItem( reply, "config",   "Config",      activeMenu);
  menuItem( reply, "plugins",  "Plugins",     activeMenu);
  menuItem( reply, "devices",  "Devices",     activeMenu);
  //menuItem( reply, "rules",    "Rules",       activeMenu);
  menuItem( reply, "tools",    "Tools",       activeMenu);
  //menuItem( reply, "advanced", "Advanced",    activeMenu);
  
  reply += F("</ul>"     );

  reply += F("<form class='page' method='post'>");
  lineNo = 0;
}

void CoreHttp::menuItem(String &reply, String url, String libelle, int activeMenu)
{  
  reply += F("<li><a href='");
  reply += url;
  reply += '\'';
  if (++currentMenu == activeMenu)
    reply += F(" class='active'");
  reply += '>';
  reply += libelle;
  reply += F("</a></li>");
}

void CoreHttp::pageFooter(String& reply)
{
  reply += F("</form></body></html>");
}

void CoreHttp::tableHeader(String &reply, String section)
{
  reply += F("<tr><th colspan=2>");
  reply += section;
  reply += F("</th></tr>");
}

void CoreHttp::tableLine(String &reply, String title, String value)
{
  reply += F("<tr");
  if ((lineNo++)%2) reply += F(" class='odd'");
  reply += F("><td");
  
  if (title)
  {
    reply += F(">");
    reply += title;
    reply += F("</td><td");
  }
  else reply += F(" colspan=2");
    
  reply += F(">");

  reply += value;
  reply += F("</td></tr>");
}

void CoreHttp::select(String &html, String name, String js)
{
  CoreHttp::select(html, name.begin(), js.begin());
}

void CoreHttp::select(String &html, char* name, char* js)
{
  if (name)
  {
    html += "<select id='";
    html += name;
    html += "' name='";
    html += name;
    html += "' onChange='";
    html += js;
    html += "'>";
  }
  else
    html += "</select>";
}


void CoreHttp::option(String &html, String name, int value)
{
  html += "<option value='";
  html += value;
  html += "'>";
  html += name;
  html += "</option>";
};

void CoreHttp::input(String &reply, String field, String value, bool hide)
{
  String type = "text";
  
  if  (hide)
  {
    type = F("password");
    value = ignoreValue;
  }
   
  reply += F("<input type='");
  reply += type;
  reply += F("' name='");
  reply += field;
  reply += F("' value='");  
  
  reply += value;
  reply += F("'>");
}

void CoreHttp::button(String &reply, String value, String url)
{
  reply += "<button onClick=\"document.location='";
  reply += url;
  reply += "'\">";
  reply += value;
  reply += "</button>";
}


//********************************************************************************
// Login check
//********************************************************************************
bool CoreHttp::isLoggedIn(void)
{
  // if no password set, we're loggeg in, that's bad so we print an error...
  if (CoreSettings::getChar("system.pass")[0] == '\0')
  {
#ifdef LOG_LEVEL_ERROR
    String log = ("HTTP : No password set, logged in...");
    CoreLog::add(LOG_LEVEL_ERROR, log);
#endif
    return true;
  }
  
  // We've already logged in
  if (loginTimeout)
  {
#ifdef LOG_LEVEL_DEBUG
    String log = ("HTTP : Already logged in...");
    CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif
    // Reset login timeout
    loginTimeout = HTTP_LOGIN_TIMEOUT;
    return true;
  }

  // Check the password given
  if ( !strcmp(WebServer.arg("pwd").c_str(), CoreSettings::getChar("system.pass")) )
  {
#ifdef LOG_LEVEL_DEBUG
    String log = ("HTTP : Logging in...");
    CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif
    // Reset login timeout
    loginTimeout = HTTP_LOGIN_TIMEOUT;

    return true;
  }

  // We're not logged in, send login form
  String reply,line;
  pageHeader(reply, MENU_OFF);
  reply += F("<table class='table'>"); // Ca devrait être dans addHeader si on fait toujours des tables

  tableHeader(reply, "Authentication");
  tableLine(reply, F("Password:"), F("<input type='password' name='pwd'>"));
  reply += F("</table>");
  
  pageFooter(reply);
  WebServer.send(200, texthtml, reply);
  
  return false;
}

void CoreHttp::handleRoot(void)
{
#ifdef LOG_LEVEL_DEBUG
  String log = F("HTTP : GET /");
  CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif

  // Redirect to setup, if no SSID set
  if ( CoreSettings::getChar( "wifi.ssid" )[0] == '\0')
  {
    WebServer.send(200, texthtml, F("<meta HTTP-EQUIV='REFRESH' content='0; url=/config'>"));
    return;
  }

  String reply, line;
  
  //reply += F("<meta HTTP-EQUIV='REFRESH' content='5; url=/'>");
  
  pageHeader(reply, MENU_INFO);

//  reply += webStatusMessage;
  reply += F("<table class='table'>"); // Ca devrait être dans addHeader si on fait toujours des tables
//--------------------------------------------------------------------------  
  tableHeader( reply, F("System Informations"));
//--------------------------------------------------------------------------
  line = DEFAULT_VERSION;
  line += " ";
  line += DEFAULT_BUILD;
  tableLine(reply, F("Program version:"), line);
//--------------------------------------------------------------------------
  line = 1;
  tableLine(reply, F("Settings version:"), line);
//--------------------------------------------------------------------------
  line = CoreSettings::getInt( "system.unit" );
  tableLine(reply, F("Unit:"), line);
//--------------------------------------------------------------------------
  line = CoreSystem::getLoadAverage();
  line += F("% ( ");
  line += CoreSystem::getLoopCounterLast();
  line += F(" / ");
  line += CoreSystem::getLoopCounterMax();
  line += F(" )");
  tableLine(reply, F("Load:"), line);
  
//--------------------------------------------------------------------------
  line = "";
  CoreSystem::format_time(line, millis() / 1000);
  tableLine(reply, F("Uptime:"), line);
//--------------------------------------------------------------------------
  line = CoreSystem::IPMaskGW( STATION_IF, true, true);
  tableLine(reply, F("IP address:"), line);
//--------------------------------------------------------------------------
  line = CoreSystem::IPMaskGW( SOFTAP_IF, true, true);
  tableLine(reply, F("AP IP Address:"), line);
//--------------------------------------------------------------------------
  line = WiFi.softAPmacAddress();
  tableLine(reply, F("AP MAC Address:"), line);
//--------------------------------------------------------------------------
  line = WiFi.RSSI();
  line += F("dBm");
  tableLine(reply, F("Signal Strength :"), line);
//--------------------------------------------------------------------------
  line = ESP.getCoreVersion();
  tableLine(reply, F("Library Version:"), line);
//--------------------------------------------------------------------------
  line = ESP.getVcc();
  tableLine(reply, F("Vcc:"), line);
//--------------------------------------------------------------------------
  line = ESP.getFreeHeap();
  tableLine(reply, F("Free Heap :"), line);
//--------------------------------------------------------------------------
  line = ESP.getChipId();
  tableLine(reply, F("Chip ID :"), line);
//--------------------------------------------------------------------------
  line = ESP.getSdkVersion();
  tableLine(reply, F("SDK version :"), line);
//--------------------------------------------------------------------------
  line = ESP.getCoreVersion();
  tableLine(reply, F("Core version :"), line);
//--------------------------------------------------------------------------
  line = ESP.getFullVersion();
  tableLine(reply, F("Full Version :"), line);
//--------------------------------------------------------------------------
  line = ESP.getBootVersion();
  tableLine(reply, F("Boot version :"), line);
//--------------------------------------------------------------------------
  line = ESP.getBootMode();
  tableLine(reply, F("Boot mode :"), line);
//--------------------------------------------------------------------------
  line = ESP.getCpuFreqMHz();
  line += F(" MHz");
  tableLine(reply, F("Frequency :"), line);
//--------------------------------------------------------------------------
  line = ESP.getFlashChipId();
  tableLine(reply, F("Flash Chip ID :"), line);
//--------------------------------------------------------------------------
  line = ESP.getFlashChipRealSize();
  tableLine(reply, F("Flash real size :"), line);
//--------------------------------------------------------------------------
  line = ESP.getFlashChipSize();
  tableLine(reply, F("Flash set size :"), line);
//--------------------------------------------------------------------------
  line = ESP.getFlashChipSpeed() / 1000000;
  line += F(" MHz");
  tableLine(reply, F("Flash speed :"), line);
//--------------------------------------------------------------------------
  line = ESP.getFlashChipMode();
  tableLine(reply, F("Flash mode :"), line);
//--------------------------------------------------------------------------
  line = ESP.getFlashChipSizeByChipId();
  tableLine(reply, F("Flash Chip Size by ID :"), line);
//--------------------------------------------------------------------------
  line = ESP.getSketchSize();
  tableLine(reply, F("Sketch size :"), line);
//--------------------------------------------------------------------------
  line = ESP.getSketchMD5();
  tableLine(reply, F("Sketch MD5 :"), line);
//--------------------------------------------------------------------------
  line = ESP.getFreeSketchSpace();
  tableLine(reply, F("Sketch Free Space :"), line);
//--------------------------------------------------------------------------
  line = ESP.getResetReason();
  tableLine(reply, F("Reset reason :"), line);
//--------------------------------------------------------------------------
  line = ESP.getResetInfo();
  tableLine(reply, F("Reset Info :"), line);
//--------------------------------------------------------------------------

  reply += F("</table>");
  pageFooter(reply);
  WebServer.send(200, texthtml, reply);
}

void CoreHttp::handleConfigSave(String &res)
{
  String line;
  
  Serial.println("Saving configuration");
    
  Setting *setting = CoreSettings::first();
  while (setting)
  {
    String val = WebServer.arg( setting->name );
    if ( (val != ignoreValue) && (val != "") )
    {
      line = F("set '");
      line += setting->name;
      line += F("'='");
      line += WebServer.arg( setting->name );
      line += '\'';
  
      //CoreLog::add(LOG_LEVEL_INFO, line);
      CoreCommands::execute( res, line );  
    }

    setting = CoreSettings::next(); 
  }
  line = "save";
  res="";
  CoreCommands::execute( res, line );
}

void CoreHttp::handleConfig(void)
{
#ifdef LOG_LEVEL_DEBUG
  String log = F("HTTP : GET /config");
  CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif

  if (!CoreHttp::isLoggedIn())
    return;

  String reply, line, res;
  
  line = WebServer.arg("cmd");
  if ( line == "save") handleConfigSave(res);
    
  pageHeader(reply, MENU_CONFIG);
  reply += res;

  reply += F("<table class='table'>"); // Ca devrait être dans addHeader si on fait toujours des tables

  #define sectionMax 20
  char section[sectionMax+1]="";
  int pos;
    
  Setting *setting = CoreSettings::first();
  while (setting)
  { 
    // search for a point
    pos=0;
    while (setting->name[pos] && setting->name[pos]!='.') pos++;

    if (setting->name[pos]) // this is a point
    {      
      if (strncasecmp( section, setting->name.begin(), pos-1))
      {
        strncpy(section, setting->name.begin(), pos); // Keep section
        section[0] += 'A' - 'a';              // Capitalize
        section[pos]='\0';                    // Set end of string \0

        String name = section;
        name += " configuration";
        tableHeader( reply, name);
      }
    }
    else  section[0]='\0';

    line="";
    
    bool hide = false;
    if (setting->name.endsWith(".pass")) hide=true;
    
    switch(setting->type)
    {
    case SET_TYPE_INT:
      CoreHttp::input(line, setting->name, String(setting->value.i));
      break;
    case SET_TYPE_STRING:
      CoreHttp::input(line, setting->name, setting->value.str->begin(), hide);
      break;
    }
    tableLine(reply, setting->name, line);
    
    setting = CoreSettings::next();
  }

  line = F("<input type='submit' name='cmd' value='save'>");
  tableHeader(reply, line);

  reply += F("</table>");
  
  pageFooter(reply);
  WebServer.send(200, texthtml, reply);
}

//void CoreHttp::handlePlugins(void)
//{
//#ifdef LOG_LEVEL_DEBUG
//  String log = F("HTTP : GET /plugins");
//  CoreLog::add(LOG_LEVEL_DEBUG, log);
//#endif
//
//  if (!CoreHttp::isLoggedIn())
//    return;
//
//  String reply, line, res;
//
//  pageHeader(reply, MENU_CONFIG);
//  reply += res;
//
//  reply += F("<table class='table'>"); // Ca devrait être dans addHeader si on fait toujours des tables
//
//  line = F("Plugin list");
//  tableHeader(reply, line);
//
//  Plugin *plugin = CorePlugins::first();
//  while (plugin)
//  {
//    tableLine(reply, *plugin->name(), *plugin->desc());
//    plugin = CorePlugins::next();
//  }
//  
//
//  reply += F("</table>");
//  
//  pageFooter(reply);
//  WebServer.send(200, texthtml, reply);
//}

void CoreHttp::handleToolsCommands(String &res, String &line)
{
  String log;
  
  for (int argNo=1; argNo <= commandArgs; argNo++)
  {
    String argName = F("arg[");
    argName += argNo;
    argName += ']';

    line += ' ';
    line += WebServer.arg( argName );
  }
  
  log = F("HTTP : command='");
  log += line;
  log += "'";
  CoreLog::add(LOG_LEVEL_INFO, log);
  CoreCommands::execute( res, line );  
}

void CoreHttp::handleTools(void)
{
#ifdef LOG_LEVEL_DEBUG
  String log = F("HTTP : GET /tools");
  CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif

  if (!CoreHttp::isLoggedIn())
    return;

  String reply, line, res;
    
  line = WebServer.arg("cmd");
  if (line != "") handleToolsCommands(res, line);
    
  pageHeader(reply, MENU_TOOLS);
  reply += res;
  
  reply += F("<table class='table'>"); // Ca devrait être dans addHeader si on fait toujours des tables
//--------------------------------------------------------------------------  
  tableHeader( reply, "System");
//--------------------------------------------------------------------------  
  line = F("<form>");
  line += F("<input type='submit' name='cmd' value='restart'>");
  line += F("<input type='submit' name='cmd' value='reboot'>");
  line += F("</form>");
  tableLine(reply, (char*)NULL, line);
//--------------------------------------------------------------------------  
  tableHeader( reply, "Network");
//--------------------------------------------------------------------------  
  line = F("<form>");
  line += F("IP Address : ");
  CoreHttp::input(line, "arg[1]", "8.8.8.8");
  line += F("<input type='submit' name='cmd' value='ping'>");
  line += F("</form>");
  tableLine(reply, (char*)NULL, line);
//--------------------------------------------------------------------------  
  tableHeader( reply, "Wifi");
//--------------------------------------------------------------------------  
  line = F("<form>");
  line += F("<input type='submit' name='cmd' value='wifiDisconnect'>");
  line += F("<input type='submit' name='cmd' value='wifiConnect'><br>");
  line += F("<input type='submit' name='cmd' value='wifiAP'>");
  line += F("<input type='submit' name='cmd' value='wifiReset'><br>");
  line += F("<input type='submit' name='cmd' value='wifiScan'>");
  line += F("</form>");
  tableLine(reply, (char*)NULL, line);
//--------------------------------------------------------------------------  
  tableHeader( reply, "i2c");
//--------------------------------------------------------------------------  
  line = F("<form>");
  line += F("<input type='submit' name='cmd' value='i2cScan'>");
  line += F("</form>");
  tableLine(reply, (char*)NULL, line);
//--------------------------------------------------------------------------  
  tableHeader( reply, "Settings");
//--------------------------------------------------------------------------  
  line = F("<form>");
  line += F("<input type='submit' name='cmd' value='settingSave'>");
  line += F("<input type='submit' name='cmd' value='settingLoad'>");
  line += F("</form>");
  tableLine(reply, (char*)NULL, line);

  reply += F("</table>");
  
  pageFooter(reply);
  WebServer.send(200, texthtml, reply);
}

void CoreHttp::handleLog(void)
{
  if (!CoreHttp::isLoggedIn())
    return;
  
  String reply, res, line = WebServer.arg("cmd");

  if (line.length() != 0)
    CoreCommands::execute(res, line);

  pageHeader(reply, MENU_ADVANCED);
  reply += res;

  reply += F("<table class='table'>"); // Ca devrait être dans addHeader si on fait toujours des tables
//--------------------------------------------------------------------------  
  tableHeader( reply, "System Log");
//--------------------------------------------------------------------------  

  int logLevel = CoreSettings::getInt("log.httpLoglevel");
  Log *log = CoreLog::first();
  line = "";
  int lineNo = 0;
  while (log)
  {
    line += lineNo++;
    
    if (log->logLevel <= logLevel)
    {
      CoreLog::display(line, log );
      
    }
    line += "<br>";
    log = CoreLog::next();
  }
  tableLine(reply, (char*)NULL, line);

  reply += F("</table>");
  pageFooter(reply);
  WebServer.send(200, texthtml, reply);
}

