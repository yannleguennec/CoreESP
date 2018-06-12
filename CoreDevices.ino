#include "CoreDevices.h"

#define deviceMax 10
CorePlugins *device[deviceMax];
//int deviceNo = 0;

#define forEachDevice(func) for(int deviceNo=0; deviceNo < deviceMax; deviceNo++) \
          { if (device[ deviceNo ]) device[ deviceNo ]->func(); }

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
    res += '\n';
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

  String html, line, res, form;

  CoreHttp::pageHeader(html, MENU_DEVICES);
  html += res;

  html += F("<table class='table'>");

  line = F("Devices list");
  CoreHttp::tableHeader(html, line);

  for (int deviceNo = 0; deviceNo < deviceMax; deviceNo++)
  {
    line = deviceNo;
    line += F(" : ");

    //    if ( device[ deviceNo ] && !device[ deviceNo ]->saved() )
    //    {
    //      // If device is set but not saved, just delete it,
    //      delete device[ deviceNo ];
    //      device[ deviceNo ] = NULL;
    //    }

    //    String js;
    //    js += "redirect('/device?deviceNo=";
    //    js += deviceNo;
    //    js += "&pluginNo=' + this.options[ this.selectedIndex ].value); ";

    // How can I choose between onClick and onChange ?
    // onClick in /devices
    // onChange in ./device

    String name = F("None");
    String url = F("redirect('/device?deviceNo=");
    url += deviceNo;
    if (device[ deviceNo ])
    {
      name = device[ deviceNo ]->pluginName();
      url += "&pluginNo=";
      url += device[ deviceNo ]->pluginNumber();
    }
    url += "');";
    CoreHttp::button(line, name, url);
    //    CoreHttp::select(line, F("pluginId"), js);
    //    CoreHttp::option(line, "None", 0);
    //    CorePlugins *plugin = CorePlugins::first();
    //    while (plugin)
    //    {
    //      String name = plugin->toString();
    //      bool selected = false;
    //
    //      // Manque le option active !!!
    //      if (device[ deviceNo ])
    //        selected = ( device[ deviceNo ]->pluginNumber() == plugin->pluginNumber() );
    //
    //      CoreHttp::option(line, name, plugin->pluginNumber(), selected);
    //
    //      plugin = CorePlugins::next();
    //    }
    //    CoreHttp::select(line);
    CoreHttp::tableLine(html, "", line);
  }

  html += F("</table>");

  CoreHttp::pageFooter(html);
  WebServer.send(200, texthtml, html);
}

// Check range excluding max
#define checkRangeEx( val, min, max ) \
          { if ( (val) < (min) ) (val) = (min); if ( (val) >= (max) ) (val) = (max); }
// Check range including max
#define checkRangeIn( val, min, max ) \
          { if ( (val) < (min) ) (val) = (min); if ( (val) >  (max) ) (val) = (max); }

void CoreDevices::setupPlugin(int &deviceNo, int pluginNo)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  Serial.println(__PRETTY_FUNCTION__);
  checkRangeEx( deviceNo, 0, deviceMax );
  checkRangeIn( pluginNo, 0, pluginNb);
  Serial.print("deviceNo=");
  Serial.print( deviceNo );
  Serial.print(", new pluginNo=");
  Serial.print( pluginNo );
  if (pluginNo)
  {
    pluginNo--;
    if (!device[ deviceNo ])
    {
      // Create plugin
      device[ deviceNo ] = plugin[ pluginNo ]->factory();
      Serial.print(" Creation ");
    }
    else
    {
      // Check plugin
      Serial.print(", old pluginNo=");
      Serial.print( device[ deviceNo ]->pluginNumber() );
      if (device[ deviceNo ]->pluginNumber() != pluginNo + 1)
      {
        // Change plugin
        delete device[ deviceNo ];
        device[ deviceNo ] = plugin[ pluginNo ]->factory();
        Serial.print(" Modification ");
      }
      else Serial.print(" Rien à faire ");

    }
  }
  else
  {
    // Remove plugin
    if (device[ deviceNo ])
      delete device[ deviceNo ];
    device[ deviceNo ] = NULL;
    Serial.print(" Suppression ");
  }

  Serial.print(", device=");
  Serial.println( (int) device[ deviceNo ] );
}

void CoreDevices::setWeb(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  int deviceNo = atoi( WebServer.arg("deviceNo").begin() ); // should check for range here !!!
  int pluginNo = atoi( WebServer.arg("pluginNo").begin() ); // should check for range here !!!

#ifdef LOG_LEVEL_DEBUG
  String log = F("HTTP : GET /device?deviceNo=");
  log += deviceNo;
  log += F("&pluginNo=");
  log += pluginNo;
  CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif

  if (!CoreHttp::isLoggedIn())
    return;

  // Ici, il faudrait :
  // - si save=true,
  //   -- Creer le device
  //   -- Recupérer les valeurs du formulaire
  //   -- Afficher le formulaire en fct du device
  // - Sinon
  //   -- Afficher le formulaire en fct du plugin (Valeurs par défaut)


  if ( WebServer.arg("save") == "yes" )
  {
    Serial.println("Saving device...");
    setupPlugin(deviceNo, pluginNo);

    if (device[ deviceNo ])
      // Update device with form infos.
      device[ deviceNo ]->webSubmit();
  }

  // Display device
  String html, line, res, form;

  CoreHttp::pageHeader(html, MENU_DEVICES);
  html += res;

  html += F("<table class='table'>");

  line = F("Device ");
  line += deviceNo;
  CoreHttp::tableHeader(html, line);

  String js;
  js = "redirect('/device?deviceNo=";
  js += deviceNo;
  js += "&pluginNo=' + this.options[ this.selectedIndex ].value); ";

  form = "";
  CoreHttp::select(form, F("pluginNo"), js);
  CoreHttp::option(form, "None", 0);
  CorePlugins *plg = CorePlugins::first();
  while (plg)
  { 
    CoreHttp::option(form, plg->toString(), plg->pluginNumber(), ( plg->pluginNumber() == pluginNo ));
    plg = CorePlugins::next();
  }
  CoreHttp::select(form);
  CoreHttp::tableLine(html, F("Plugin : "), form);

  if (pluginNo)
    plugin[ pluginNo - 1 ]->webForm( html );

  form = "";
  CoreHttp::button( form, save,   "document.submit()");
  CoreHttp::button( form, cancel, "redirect('/devices')");
  CoreHttp::tableLine(html, "", form);

  html += F("</table>");

  CoreHttp::pageFooter(html);
  WebServer.send(200, texthtml, html);
}



