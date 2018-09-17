#include "CoreDevices.h"

std::array<CorePlugins*, deviceMax> CoreDevices::devices;
#define forEachDevice(func) for( CorePlugins *device : devices ) { if (device) device->func(); }

void CoreDevices::setup(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  coreHttp.addUrl("/devices", CoreDevices::listWeb);
  coreHttp.addUrl("/device", CoreDevices::setWeb);

  coreCommand.addCommand("devices", CoreDevices::listCommand, "List the devices");
  coreCommand.addCommand("device",  CoreDevices::setCommand,  "Configure or display device");

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

void CoreDevices::listCommand(String& res, char**)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  res += corePlugins.size();
  res += F(" devices :");

  uint deviceNo = 0;
  for (CorePlugins* device : coreDevices.devices)
//  for (int deviceNo = 0; deviceNo < deviceMax; deviceNo++)
  {
    deviceNo++;
    res = F("  ");
    // add a space for padding when < 10
    if (deviceNo < 10) res += " ";
    res += deviceNo;
    res += F(" : ");

    if (device)
    {
      res += device->deviceTopic();
      res += F(" (");
      res += device->pluginName();
      res += F(")");
    }
    else res += F("None");

    res += '\n';
  }
}

#define param(x) block[x]

void CoreDevices::setCommand(String& res, char** block)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  int deviceNo = atoi( param(1) );
  String log = F("Device ");
  log += deviceNo;
  log += F(" : ");
  CoreLog::add(LOG_LEVEL_INFO, log);
  
  res += log;

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

  if (!coreHttp.isLoggedIn())
    return;

  String html, line, res, form;

  coreHttp.pageHeader(html, MENU_DEVICES);
  html += res;

  html += F("<table class='table'>");

  line = F("Devices list");
  coreHttp.tableHeader(html, line);

  for (int deviceNo = 0; deviceNo < deviceMax; deviceNo++)
  {
    line = deviceNo;
    line += F(" : ");

    String name = F("None");
    String url = F("redirect('/device?deviceNo=");
    url += deviceNo;
    
    if (devices[ deviceNo ])
    {
      name = devices[ deviceNo ]->pluginName();
      url += "&pluginNo=";
      url += devices[ deviceNo ]->pluginNumber();
    }
    url += "');";
    coreHttp.button(line, name, url);
    coreHttp.tableLine(html, "", line);
  }

  html += F("</table>");

  coreHttp.pageFooter(html);
  WebServer.send(200, texthtml, html);
}

// Check range excluding max
#define checkRangeExcluding( val, min, max ) \
          { if ( (val) < (min) ) (val) = (min); if ( (val) >= (max) ) (val) = (max); }
// Check range including max
#define checkRangeIncluding( val, min, max ) \
          { if ( (val) < (min) ) (val) = (min); if ( (val) >  (max) ) (val) = (max); }

void CoreDevices::setupPlugin(size_t deviceNo, size_t pluginNo)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  Serial.println(__PRETTY_FUNCTION__);

  if (deviceNo >= deviceMax) return;
  if (pluginNo > corePlugins.size()) return;
  
  Serial.print("deviceNo=");
  Serial.print( deviceNo );
  Serial.print(", new pluginNo=");
  Serial.print( pluginNo );
  if (pluginNo)
  {
    pluginNo--;
    if (!devices[ deviceNo ])
    {
      // Create plugin
      devices[ deviceNo ] = CorePlugins::plugins[ pluginNo ]->factory();
      Serial.print(" Creation ");
    }
    else
    {
      // Check plugin
      Serial.print(", old pluginNo=");
      Serial.print( devices[ deviceNo ]->pluginNumber() );
      if (devices[ deviceNo ]->pluginNumber() != pluginNo + 1)
      {
        // Change plugin
        delete devices[ deviceNo ];
        devices[ deviceNo ] = CorePlugins::plugins[ pluginNo ]->factory();
        Serial.print(" Modification ");
      }
      else Serial.print(" Rien à faire ");

    }
  }
  else
  {
    // Remove plugin
    if (devices[ deviceNo ])
      delete devices[ deviceNo ];
    devices[ deviceNo ] = nullptr;
    Serial.print(" Suppression ");
  }

  Serial.print(", device=");
  Serial.println( (int) devices[ deviceNo ] );
}

void CoreDevices::setWeb(void)
{
#ifdef LOG_LEVEL_PANIC
  Serial.println(__PRETTY_FUNCTION__);
#endif
  size_t deviceNo = atoi( WebServer.arg("deviceNo").begin() ); // should check for range here !!!
  size_t pluginNo = atoi( WebServer.arg("pluginNo").begin() ); // should check for range here !!!

#ifdef LOG_LEVEL_DEBUG
  String log = F("HTTP : GET /device?deviceNo=");
  log += deviceNo;
  log += F("&pluginNo=");
  log += pluginNo;
  CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif

  if (!coreHttp.isLoggedIn())
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
    coreDevices.setupPlugin(deviceNo, pluginNo);

    if (devices[ deviceNo ])
      // Update device with form infos.
      devices[ deviceNo ]->webSubmit();
  }

  // Display device
  String html, line, res, form;

  coreHttp.pageHeader(html, MENU_DEVICES);
  html += res;

  html += F("<table class='table'>");

  line = F("Device ");
  line += deviceNo;
  coreHttp.tableHeader(html, line);

  String js;
  js = "redirect('/device?deviceNo=";
  js += deviceNo;
  js += "&pluginNo=' + this.options[ this.selectedIndex ].value); ";

  form = "";
  coreHttp.select(form, F("pluginNo"), js);
  coreHttp.option(form, "None", 0);

  for (CorePlugins *plugin : corePlugins.plugins)
    coreHttp.option(form, plugin->toString(), plugin->pluginNumber(), ( plugin->pluginNumber() == pluginNo ));

  coreHttp.select(form);
  coreHttp.tableLine(html, F("Plugin : "), form);

  if (pluginNo)
    CorePlugins::plugins[ pluginNo - 1 ]->webForm( html );

  form = "";
  coreHttp.button( form, save,   "document.submit()");
  coreHttp.button( form, cancel, "redirect('/devices')");
  coreHttp.tableLine(html, "", form);

  html += F("</table>");

  coreHttp.pageFooter(html);
  WebServer.send(200, texthtml, html);
}

CoreDevices coreDevices;

