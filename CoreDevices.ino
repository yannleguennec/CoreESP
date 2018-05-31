#include "CoreDevices.h"

#define deviceMax 10
CorePlugins *device[deviceMax];
int deviceNo = 0;

#define forEachDevice(func) for(int deviceNo=0; deviceNo < deviceMax; deviceNo++) { if (device[ deviceNo ]) device[ deviceNo ]->func(); }

void CoreDevices::setup(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  CoreHttp::add("/devices", CoreDevices::listWeb);
  CoreHttp::add("/device", CoreDevices::setWeb);

  CoreCommands::add("devices", CoreDevices::listCommand, "List the devices");
  CoreCommands::add("device",  CoreDevices::setCommand,  "Configure or display device");

  forEachDevice(setup);
}

void CoreDevices::loopSlow(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  forEachDevice(loopSlow);
};

void CoreDevices::loopMedium(void)
{
#ifdef LOG_LEVEL_PANIC
 // Serial.println(__PRETTY_FUNCTION__);
#endif
  forEachDevice(loopMedium);
};

void CoreDevices::loopFast(void)
{
#ifdef LOG_LEVEL_PANIC
 // Serial.println(__PRETTY_FUNCTION__);
#endif
  forEachDevice(loopFast);
};

void CoreDevices::listCommand(String& res, char** block)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String log = F("Devices :");
  CoreLog::add(LOG_LEVEL_INFO, log);

  for (int deviceNo = 0; deviceNo < deviceMax; deviceNo++)
  {
    log = F("  ");
    if (deviceNo < 10) log += " ";
    log += deviceNo;
    log += F(" : ");

    if (device[ deviceNo ])
    {
      log += device[ deviceNo ]->deviceTopic();
      log += F(" (");
      log += device[ deviceNo ]->pluginName();
      log += F(")");
    }
    else log += F("None");

    CoreLog::add(LOG_LEVEL_INFO, log);
    res += log;
  }
}

void CoreDevices::setCommand(String& res, char** block)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  String log = F("Device :");
  CoreLog::add(LOG_LEVEL_INFO, log);


}

void CoreDevices::listWeb(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
#ifdef LOG_LEVEL_DEBUG
  String log = F("HTTP : GET /devices");
  CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif

  if (!CoreHttp::isLoggedIn())
    return;

  String html, line, res;

  CoreHttp::pageHeader(html, MENU_DEVICES);
  html += res;

  html += F("<table class='table'>");

  line = F("Devices list");
  CoreHttp::tableHeader(html, line);

  for (deviceNo = 0; deviceNo < deviceMax; deviceNo++)
  {
    line = deviceNo;
    line += F(" : ");

    if (device[ deviceNo ])
    {
      line += device[ deviceNo ]->deviceTopic();

      String name = F("device_");
      name += deviceNo;

      line += device[ deviceNo ]->toString();
    }
    else
      line += F("None");

    line += ' ';
    
    String url = F("/device?id=");
    url += deviceNo;
    CoreHttp::button(line, "Modify", url);

    CoreHttp::tableLine(html, "", line);
  }

  html += F("</table>");

  CoreHttp::pageFooter(html);
  WebServer.send(200, texthtml, html);
}

// Check range excluding max
#define checkRangeEx( val, min, max ) { if ( (val) < (min) ) (val) = (min); if ( (val) >= (max) ) (val) = (max); }
// Check range including max
#define checkRangeIn( val, min, max ) { if ( (val) < (min) ) (val) = (min); if ( (val) >  (max) ) (val) = (max); }

void CoreDevices::setupPlugin(int &deviceId, int pluginId)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  Serial.println(__PRETTY_FUNCTION__);
  checkRangeEx( deviceId, 0, deviceMax );
  checkRangeIn( pluginId, 0, pluginNb);
  Serial.print("deviceId=");
  Serial.print( deviceId );
  Serial.print(", new pluginId=");
  Serial.print( pluginId );
  if (pluginId)
  {
    pluginId--;
    if (!device[ deviceId ])
    {
      // Create plugin
      device[ deviceId ] = plugin[ pluginId ]->factory();
      Serial.print(" Creation ");
    }
    else
    {
      // Check plugin
      Serial.print(", old pluginId=");
      Serial.print( device[ deviceId ]->pluginNumber() );
      if (device[ deviceId ]->pluginNumber() != pluginId+1)
      {
        // Change plugin
        delete device[ deviceId ];
        device[ deviceId ] = plugin[ pluginId ]->factory();
        Serial.print(" Modification ");
      }
      else Serial.print(" Rien à faire ");

    }
  }
  else
  {
    // Remove plugin
    if (device[ deviceId ])
      delete device[ deviceId ];
    device[ deviceId ] = NULL;
    Serial.print(" Suppression ");
  }

  Serial.print(", device=");
  Serial.println( (int) device[ deviceId ] );
}

void CoreDevices::setWeb(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  int deviceId = atoi( WebServer.arg("id").begin() );

#ifdef LOG_LEVEL_DEBUG
  String log = F("HTTP : GET /device?id=");
  log += deviceId;
  CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif

  if (!CoreHttp::isLoggedIn())
    return;

  int pluginId = atoi( WebServer.arg("pluginId").begin() );
  setupPlugin(deviceId, pluginId);
  
  if (device[ deviceId ])
    // Update plugin
    device[ deviceId ]->webSubmit();

  // Display device
  String html, line, res, form;

  CoreHttp::pageHeader(html, MENU_DEVICES);
  html += res;

  html += F("<table class='table'>");

  line = F("Device ");
  line += deviceId;
  line += F(", Plugin ");
  line += pluginId;
  line += " (0x";
  line += (int) device[ deviceId ];
  line += ")";
  CoreHttp::tableHeader(html, line);

  line = F("Plugin");
  form = "";
  String js = 
      //" alert(this.options[ this.selectedIndex ].value); ";
      "form.submit();";
  CoreHttp::select(form, F("pluginId"), js);
  CoreHttp::option(form, "None", 0);
  CorePlugins *plugin = CorePlugins::first();
  while (plugin)
  {
    String name = plugin->toString();
    bool selected = false;
    
    // Manque le option active !!!
    if (device[ deviceId ])
      selected = device[ deviceId ]->pluginNumber() == plugin->pluginNumber();

    CoreHttp::option(form, name, plugin->pluginNumber(), selected);

    plugin = CorePlugins::next();
  }  
  CoreHttp::select(form);
  CoreHttp::tableLine(html, line, form);
  
  if (device[ deviceId ])
    device[ deviceId ]->webForm( html );

  form = "";
  CoreHttp::button( form, "Save", "save()");
  CoreHttp::button( form, "Cancel", "cancel()");
  CoreHttp::tableLine(html, "", form);

  html += F("</table>");

  CoreHttp::pageFooter(html);
  WebServer.send(200, texthtml, html);
}



