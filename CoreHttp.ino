#include "user_interface.h"

#include "CoreHttp.h"
#include "CoreLog.h"
#include "CoreSettings.h"

ESP8266WebServer WebServer(80);

boolean WebLoggedIn = false;
int WebLoggedInTimer = 300;
//String webStatusMessage = "";

// Common strings
String texthtml, ignoreValue, save, cancel;


byte loginTimeout = 0; // Not logged in at start
int lineNo = 0;
int currentMenu = 0;

void CoreHttp::addUrl(String url, void (*func)(void))
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  WebServer.on( url.begin(), func );
}

void CoreHttp::setup()
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
#ifdef LOG_LEVEL_DEBUG
  String log = F("HTTP : Initialization.");
  CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif

  // Initialize common strings
  texthtml = F("text/html");
  ignoreValue = F("*** ignore ***");
  save = F("<span class='tick'> Save</span>");
  cancel = F("<span class='cross'> Cancel</span>");

  // Prepare webserver pages handling
  CoreHttp::addUrl( "/",        CoreHttp::handleRoot    );
  CoreHttp::addUrl( "/log",     CoreHttp::handleLog     );
  CoreHttp::addUrl( "/config",  CoreHttp::handleConfig  );
  CoreHttp::addUrl( "/tools",   CoreHttp::handleTools   );

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

void CoreHttp::loop()
{
  WebServer.handleClient();
}

void CoreHttp::loopMedium()
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  if (loginTimeout)
  {
    String log;

    loginTimeout--;
#ifndef LOG_LEVEL_DEBUG
    log = F("HTTP : LoginTimeout... ");
    log += loginTimeout;
    CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif
    if (!loginTimeout)
    {
        log = F("HTTP : Logged out.");        
        CoreLog::add(LOG_LEVEL_INFO, log);
    }
  }
}

void CoreHttp::pageHeader(String& html, int activeMenu)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  html += F( "<!DOCTYPE html>"      );
  html += F( "<html>"      );
  html += F(   "<head><title>" );
  html += CoreSettings::getChar( "system.name" );
  html += F(     "</title>"      );
  html += F(     "<meta charset='utf-8' />" );
  html += F(     "<style>"     );
  html += F(       "html body{font-family:Sans-Serif;font-size:12px;padding:0;margin:0;} "     );
  html += F(       ".table{width:94%;text-align:center;background:#eec;border-collapse:collapse;border-top:7px solid #9af;border-bottom:7px solid #9af;border-right:1px solid #dde;border-left:1px solid #dde;margin:5px;} "     );
  html += F(       ".table th{font-size:15px;font-weight:bold;background:#ccc;color:#666;padding:8px;} "     );
  html += F(       ".table tr{border-bottom:1px solid #ddd;} "     );
  html += F(       ".table tr.odd{background:#eed;} "     );
  html += F(       ".table tr:hover{background:#dde;} "     );
  html += F(       ".table tr td{width:50%;color:#666;padding:4px;} "     );
  html += F(       ".table tr td:first-child{white-space:nowrap;font-weight:bold;} "     );
  html += F(       ".page{margin-left:150px;padding:5px;} "     );
  html += F(       ".menu{list-style-type:none;margin:0;padding:0;width:130px;background-color:#f1f1f1;height:100%;position:fixed;overflow:auto;white-space:nowrap;} "     );
  html += F(       ".menu li{text-align:center;border-bottom:1px solid #dde;} "     );
  html += F(       "li:last-child{border-bottom:none;} "     );
  html += F(       ".menu li.title{font-size:15px;font-weight:bold;background:#888;color:#ddd;padding:8px;} "     );
  html += F(       ".menu li a{font-size: 13px;display: block;color: #999;padding:8px 16px;text-decoration:none;} "     );
  html += F(       ".menu li a.active{background:#e8e8e8;color:#666;font-weight:bold;} "     );
  html += F(       ".menu li a:hover{background:#dde;color:#666;} "     );
  html += F(       ".tick:before{content:\"\\2713\";color: darkgreen;} " );
  html += F(       ".cross:before{content:\"\\2717\";color: crimson;} ");
  html += F(     "</style>"    );
  html += F(   "</head>"     );
  html += F( "<body>"      );

  html += F(   "<script>" );
  html += F(     "function redirect(url) { document.location = url; return false; }" );
  html += F(   "</script>" ); 

  html += F(    "<ul class='menu'>"     );
  html += F(      "<li class='title'>"  );
  html += CoreSettings::getChar( "system.name" );
  html += F(      "</li>"     );

  currentMenu = 0;
  menuItem( html, ".",        "Information", activeMenu);
  menuItem( html, "config",   "Config",      activeMenu);
  menuItem( html, "plugins",  "Plugins",     activeMenu);
  menuItem( html, "devices",  "Devices",     activeMenu);
  //menuItem( html, "rules",    "Rules",       activeMenu);
  menuItem( html, "tools",    "Tools",       activeMenu);
  //menuItem( html, "advanced", "Advanced",    activeMenu);

  html += F(     "</ul>"     );

  html += F(     "<form class='page' method='post'>");
  lineNo = 0;
}

void CoreHttp::menuItem(String &html, String url, String libelle, int activeMenu)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  html += F("<li><a href='");
  html += url;
  html += '\'';
  if (++currentMenu == activeMenu)
    html += F(" class='active'");
  html += '>';
  html += libelle;
  html += F("</a></li>");
}

void CoreHttp::pageFooter(String& html)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  html += F("</form></body></html>");
}

void CoreHttp::tableHeader(String &html, String section)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  html += F("<tr><th colspan=2>");
  html += section;
  html += F("</th></tr>");
}

void CoreHttp::tableLine(String &html, String title, String value)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  html += F("<tr");
  if ((lineNo++) % 2) html += F(" class='odd'");
  html += F("><td");

  if (title != "")
  {
    html += '>';
    html += title;
    html += F("</td><td");
  }
  else html += F(" colspan=2");

  html += '>';

  html += value;
  html += F("</td></tr>");
}

void CoreHttp::select(String &html, String name, String js)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  if (name!="")
  {
    html += F("<select id=\"");
    html += name;
    html += F("\" name=\"");
    html += name;
    if (js != "")
    {
//      onchange="changeFunc(value);" directly pass the value
      html += F("\" onChange=\"");
      html += js;
    }
    html += F("\">");
  }
  else
    html += F("</select>");
}


void CoreHttp::option(String &html, String name, int value, bool selected)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  html += F("<option value='");
  html += value;
  html += '\'';
  if (selected) html += F(" selected");

  html += '>';
  html += name;
  html += F("</option>");
};

void CoreHttp::input(String &html, String field, String value, String type)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  html += F("<input type='");
  html += type;
  html += F("' name='");
  html += field;
  html += F("' value='");
  html += value;
  html += F("'>");
}

void CoreHttp::button(String &html, String value, String url)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  html += F("<button");
  if (url != "")
  {
    html += F(" onClick=\"");
    html += url;
    html += F("; return false;\"");
  }
  html += '>';
  html += value;
  html += F("</button>");
}


//********************************************************************************
// Login check
//********************************************************************************
bool CoreHttp::isLoggedIn(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  // if no password set, we're loggeg in, that's bad so we print an error...
  if (CoreSettings::getChar("system.pass")[0] == '\0')
  {
#ifdef LOG_LEVEL_ERROR
    String log = F("HTTP : No password set, logged in...");
    CoreLog::add(LOG_LEVEL_ERROR, log);
#endif
    return true;
  }

  // We've already logged in
  if (loginTimeout)
  {
#ifdef LOG_LEVEL_DEBUG
    String log = F("HTTP : Already logged in...");
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
    String log = F("HTTP : Logging in...");
    CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif
    // Reset login timeout
    loginTimeout = HTTP_LOGIN_TIMEOUT;

    return true;
  }

  // We're not logged in, send login form
  String html, line;
  pageHeader(html, MENU_OFF);
  html += F("<table class='table'>"); // Ca devrait être dans addHeader si on fait toujours des tables

  tableHeader(html, F("Authentication"));
  tableLine(html, F("Password:"), F("<input type='password' name='pwd'>"));
  html += F("</table>");

  pageFooter(html);
  WebServer.send(200, texthtml, html);

  return false;
}

void CoreHttp::handleRoot(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
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

  String html, line;

  //html += F("<meta HTTP-EQUIV='REFRESH' content='5; url=/'>");

  pageHeader(html, MENU_INFO);

  //  html += webStatusMessage;
  html += F("<table class='table'>"); // Ca devrait être dans addHeader si on fait toujours des tables
  //--------------------------------------------------------------------------
  tableHeader(html, F("System Informations"));
  //--------------------------------------------------------------------------
  line = DEFAULT_VERSION;
  line += " ";
  line += DEFAULT_BUILD;
  tableLine(html, F("Program version:"), line);
  //--------------------------------------------------------------------------
  line = 1;
  tableLine(html, F("Settings version:"), line);
  //--------------------------------------------------------------------------
  line = CoreSettings::getInt( "system.unit" );
  tableLine(html, F("Unit:"), line);
  //--------------------------------------------------------------------------
  line = CoreSystem::getLoadAverage();
  line += F("% ( ");
  line += CoreSystem::getLoopCounterLast();
  line += F(" / ");
  line += CoreSystem::getLoopCounterMax();
  line += F(" )");
  tableLine(html, F("Load:"), line);
  //--------------------------------------------------------------------------
  line = "";
  CoreSystem::format_time(line, millis() / 1000);
  tableLine(html, F("Uptime:"), line);
  //--------------------------------------------------------------------------
  line = CoreSystem::IPMaskGW( STATION_IF, true, true);
  tableLine(html, F("IP address:"), line);
  //--------------------------------------------------------------------------
  line = CoreSystem::IPMaskGW( SOFTAP_IF, true, true);
  tableLine(html, F("AP IP Address:"), line);
  //--------------------------------------------------------------------------
  line = WiFi.softAPmacAddress();
  tableLine(html, F("AP MAC Address:"), line);
  //--------------------------------------------------------------------------
  line = WiFi.RSSI();
  line += F("dBm");
  tableLine(html, F("Signal Strength :"), line);
  //--------------------------------------------------------------------------
  line = ESP.getCoreVersion();
  tableLine(html, F("Library Version:"), line);
  //--------------------------------------------------------------------------
  line = ESP.getVcc();
  tableLine(html, F("Vcc:"), line);
  //--------------------------------------------------------------------------
  line = ESP.getFreeHeap();
  tableLine(html, F("Free Heap :"), line);
  //--------------------------------------------------------------------------
  line = ESP.getChipId();
  tableLine(html, F("Chip ID :"), line);
  //--------------------------------------------------------------------------
  line = ESP.getSdkVersion();
  tableLine(html, F("SDK version :"), line);
  //--------------------------------------------------------------------------
  line = ESP.getCoreVersion();
  tableLine(html, F("Core version :"), line);
  //--------------------------------------------------------------------------
  line = ESP.getFullVersion();
  tableLine(html, F("Full Version :"), line);
  //--------------------------------------------------------------------------
  line = ESP.getBootVersion();
  tableLine(html, F("Boot version :"), line);
  //--------------------------------------------------------------------------
  line = ESP.getBootMode();
  tableLine(html, F("Boot mode :"), line);
  //--------------------------------------------------------------------------
  line = ESP.getCpuFreqMHz();
  line += F(" MHz");
  tableLine(html, F("Frequency :"), line);
  //--------------------------------------------------------------------------
  line = ESP.getFlashChipId();
  tableLine(html, F("Flash Chip ID :"), line);
  //--------------------------------------------------------------------------
  line = ESP.getFlashChipRealSize();
  tableLine(html, F("Flash real size :"), line);
  //--------------------------------------------------------------------------
  line = ESP.getFlashChipSize();
  tableLine(html, F("Flash set size :"), line);
  //--------------------------------------------------------------------------
  line = ESP.getFlashChipSpeed() / 1000000;
  line += F(" MHz");
  tableLine(html, F("Flash speed :"), line);
  //--------------------------------------------------------------------------
  line = ESP.getFlashChipMode();
  tableLine(html, F("Flash mode :"), line);
  //--------------------------------------------------------------------------
  line = ESP.getFlashChipSizeByChipId();
  tableLine(html, F("Flash Chip Size by ID :"), line);
  //--------------------------------------------------------------------------
  line = ESP.getSketchSize();
  tableLine(html, F("Sketch size :"), line);
  //--------------------------------------------------------------------------
  line = ESP.getSketchMD5();
  tableLine(html, F("Sketch MD5 :"), line);
  //--------------------------------------------------------------------------
  line = ESP.getFreeSketchSpace();
  tableLine(html, F("Sketch Free Space :"), line);
  //--------------------------------------------------------------------------
  line = ESP.getResetReason();
  tableLine(html, F("Reset reason :"), line);
  //--------------------------------------------------------------------------
  line = ESP.getResetInfo();
  tableLine(html, F("Reset Info :"), line);
  //--------------------------------------------------------------------------

  html += F("</table>");
  pageFooter(html);
  WebServer.send(200, texthtml, html);
}

void CoreHttp::handleConfigSave(String &res)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
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
      coreCommands.execute( res, line );
    }

    setting = CoreSettings::next();
  }
  line = "save";
  res = "";
  coreController.execute( res, line );
}

void CoreHttp::handleConfig(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
#ifdef LOG_LEVEL_DEBUG
  String log = F("HTTP : GET /config");
  CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif

  if (!CoreHttp::isLoggedIn())
    return;

  String html, line, res;

  line = WebServer.arg("cmd");
  if ( line == "save") handleConfigSave(res);

  pageHeader(html, MENU_CONFIG);
  html += res;

  html += F("<table class='table'>"); // Ca devrait être dans addHeader si on fait toujours des tables

#define sectionMax 20
  char section[sectionMax + 1] = "";
  int pos;

  Setting *setting = CoreSettings::first();
  while (setting)
  {
    // search for a point
    pos = 0;
    while (setting->name[pos] && setting->name[pos] != '.') pos++;

    if (setting->name[pos]) // this is a point
    {
      if (strncasecmp( section, setting->name.begin(), pos - 1))
      {
        strncpy(section, setting->name.begin(), pos); // Keep section
        section[0] += 'A' - 'a';              // Capitalize
        section[pos] = '\0';                  // Set end of string \0

        String name = section;
        name += F(" configuration");
        tableHeader( html, name);
      }
    }
    else  section[0] = '\0';

    String value = F("");
    String type = F("text");
    
    switch (setting->type)
    {
      case SET_TYPE_INT:
        value = setting->value.i;
        break;
      case SET_TYPE_STRING:
        value = *setting->value.str;
        break;
    }

    if (setting->name.endsWith(".pass")) 
    {
      type = F("password");
      value = ignoreValue;
    }
    
    line = "";
    CoreHttp::input(line, setting->name, value, type);
    tableLine(html, setting->name, line);

    setting = CoreSettings::next();
  }

  line = F("<input type='submit' name='cmd' value='save'>");
  tableHeader(html, line);

  html += F("</table>");

  pageFooter(html);
  WebServer.send(200, texthtml, html);
}

void CoreHttp::handleToolsCommands(String &res, String &line)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String log;

  for (int argNo = 1; argNo <= commandArgs; argNo++)
  {
    String argName = F("arg[");
    argName += argNo;
    argName += ']';

    line += ' ';
    line += WebServer.arg( argName );
  }

  log = F("HTTP : command='");
  log += line;
  log += '\'';
  CoreLog::add(LOG_LEVEL_INFO, log);
  coreController.execute( res, line );
}

void CoreHttp::handleTools(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
#ifdef LOG_LEVEL_DEBUG
  String log = F("HTTP : GET /tools");
  CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif

  if (!CoreHttp::isLoggedIn())
    return;

  String html, line, res;

  line = WebServer.arg("cmd");
  if (line != "") handleToolsCommands(res, line);

  pageHeader(html, MENU_TOOLS);
  html += res;

  html += F("<table class='table'>"); // Ca devrait être dans addHeader si on fait toujours des tables
  //--------------------------------------------------------------------------
  tableHeader( html, F("System"));
  //--------------------------------------------------------------------------
  line =  F("<form>");
  line += F("<input type='submit' name='cmd' value='restart'>");
  line += F("<input type='submit' name='cmd' value='reboot'>");
  line += F("</form>");
  tableLine(html, (char*)NULL, line);
  //--------------------------------------------------------------------------
  tableHeader( html, F("Network"));
  //--------------------------------------------------------------------------
  line =  F("<form>");
  line += F("IP Address : ");
  CoreHttp::input(line, "arg[1]", "8.8.8.8");
  line += F("<input type='submit' name='cmd' value='ping'>");
  line += F("</form>");
  tableLine(html, (char*)NULL, line);
  //--------------------------------------------------------------------------
  tableHeader( html, F("Wifi"));
  //--------------------------------------------------------------------------
  line =  F("<form>");
  line += F(  "<input type='submit' name='cmd' value='wifiDisconnect'>");
  line += F(  "<input type='submit' name='cmd' value='wifiConnect'><br>");
  line += F(  "<input type='submit' name='cmd' value='wifiAP'>");
  line += F(  "<input type='submit' name='cmd' value='wifiReset'><br>");
  line += F(  "<input type='submit' name='cmd' value='wifiScan'>");
  line += F("</form>");
  tableLine(html, (char*)NULL, line);
  //--------------------------------------------------------------------------
  tableHeader( html, F("i2c"));
  //--------------------------------------------------------------------------
  line =  F("<form>");
  line += F("<input type='submit' name='cmd' value='i2cScan'>");
  line += F("</form>");
  tableLine(html, (char*)NULL, line);
  //--------------------------------------------------------------------------
  tableHeader( html, F("Settings"));
  //--------------------------------------------------------------------------
  line =  F("<form>");
  line += F(  "<input type='submit' name='cmd' value='settingSave'>");
  line += F(  "<input type='submit' name='cmd' value='settingLoad'>");
  line += F("</form>");
  tableLine(html, (char*)NULL, line);

  html += F("</table>");

  pageFooter(html);
  WebServer.send(200, texthtml, html);
}

void CoreHttp::handleLog(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  if (!CoreHttp::isLoggedIn())
    return;

  String reply, res, line = WebServer.arg("cmd");

  if (line.length() != 0)
    coreController.execute(res, line);

  pageHeader(reply, MENU_ADVANCED);
  reply += res;

  reply += F("<table class='table'>"); // Ca devrait être dans addHeader si on fait toujours des tables
  //--------------------------------------------------------------------------
  tableHeader( reply, F("System Log"));
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
    line += F("<br>");
    log = CoreLog::next();
  }
  tableLine(reply, (char*)NULL, line);

  reply += F("</table>");
  pageFooter(reply);
  WebServer.send(200, texthtml, reply);
}

