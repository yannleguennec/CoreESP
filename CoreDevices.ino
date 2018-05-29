#include "CoreDevices.h"

#define deviceMax 10
CorePlugins *device[deviceMax];
int deviceNo = 0;

#define forEachDevice(func) for(int deviceNo=0; deviceNo < deviceMax; deviceNo++) { if (device[ deviceNo ]) device[ deviceNo ]->func(); }

void CoreDevices::setup(void)
{
  CoreHttp::add("/devices", CoreDevices::listWeb);
  CoreHttp::add("/device", CoreDevices::setWeb);

  CoreCommands::add("devices", CoreDevices::listCommand, "List the devices");
  CoreCommands::add("device", CoreDevices::setCommand, "Configure or display device");

  forEachDevice(setup);
}

void CoreDevices::loopSlow(void)
{
  forEachDevice(loopFast);
};

void CoreDevices::loopMedium(void)
{
  forEachDevice(loopFast);
};

void CoreDevices::loopFast(void)
{
  forEachDevice(loopFast);
};

void CoreDevices::listCommand(String& res, char** block)
{
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
      log += device[ deviceNo ]->topic();
      log += F(" (");
      log += device[ deviceNo ]->name();
      log += F(")");
    }
    else log += F("None");

    CoreLog::add(LOG_LEVEL_INFO, log);
    res += log;
  }
}

void CoreDevices::setCommand(String& res, char** block)
{
  String log = F("Device :");
  CoreLog::add(LOG_LEVEL_INFO, log);


}

void CoreDevices::listWeb(void)
{
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
    String title, form;
    title += deviceNo;
    title += F(" : ");

    if (device[ deviceNo ])
    {
      title += device[ deviceNo ]->topic();

      String name = F("device_");
      name += deviceNo;

      form = device[ deviceNo ]->toString();
    }
    else
      form = F("None");

    String url = F("/devices?dn=");
    url += deviceNo;
    CoreHttp::button(form, "Modify", url);

    CoreHttp::tableLine(html, title, form);
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
  checkRangeEx( deviceId, 0, deviceMax );
  if (pluginId)
  {
    if (!device[ deviceId ])
      // Create plugin
      device[ deviceId ] = plugin[ pluginId ]->factory();
    else
    {
      // Check plugin
      if (device[ deviceId ]->num() != pluginId)
      {
        // Change plugin
        delete device[ deviceId ];
        device[ deviceId ] = plugin[ pluginId ]->factory();
      }
    }
  }
  else
  {
    // Remove plugin
    if (device[ deviceId ])
      delete device[ deviceId ];
    device[ deviceId ] = NULL;
  }
}

void CoreDevices::setWeb(void)
{
  int deviceId = atoi( WebServer.arg("id").begin() );

#ifdef LOG_LEVEL_DEBUG
  String log = F("HTTP : GET /device?id=");
  log += deviceId;
  CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif

  if (!CoreHttp::isLoggedIn())
    return;

  int pluginId = atoi( WebServer.arg("pluginId").begin() );
  checkRangeEx( pluginId, 0, pluginNb);

  setupPlugin(deviceId, pluginId);
  if (device[ deviceId ])
    // Update plugin
    device[ deviceId ]->webFormSubmit();

  // Display device
  String html, line, res, form;

  CoreHttp::pageHeader(html, MENU_DEVICES);
  html += res;

  html += F("<table class='table'>");

  line = F("Device ");
  log += deviceId;
  CoreHttp::tableHeader(html, line);

  line = F("Plugin");
  form = "";
  CoreHttp::select(form, "pluginId");
  CoreHttp::option(form, "None", 0);
  CorePlugins *plugin = CorePlugins::first();
  while (plugin)
  {
    String name = plugin->toString();

    // Manque le option active !!!
    bool selected = device[ deviceId ]->num() == plugin->num();
    CoreHttp::option(form, name, plugin->num(), selected);

    plugin = CorePlugins::next();
  }
  CoreHttp::select(form);
  CoreHttp::tableLine(html, line, form);

  if (device[ deviceId ])
    device[ deviceId ]->webForm(html);

  html += F("</table>");

  CoreHttp::pageFooter(html);
  WebServer.send(200, texthtml, html);
}



