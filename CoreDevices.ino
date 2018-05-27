#include "CoreDevices.h"

#define deviceMax 10
CoreDevices device[deviceMax];
int deviceNb = 10;
int deviceNo = 0;

int CoreDevices::num(int num)
{
  if (num) this->_num = num;
  return this->_num;
}

String* CoreDevices::name(String* name)
{
  if (name) this->_name = *name;
  return &this->_name;
}

String* CoreDevices::desc(String* desc)
{
  if (desc) this->_desc = *desc;
  return &this->_desc;
}

#define forEachDevice(func) for(int deviceNo=0; deviceNo < deviceNb; deviceNo++) { if (device[ deviceNo ].plugin) device[ deviceNo ].plugin->func(); }
CoreDevices::CoreDevices(void)
{
}

void CoreDevices::setup(void)
{
  WebServer.on( "/devices", CoreDevices::devicesWeb );
  CoreHttp::add("/devices", CoreDevices::deviceWeb);
  CoreCommands::add("devices", CoreDevices::deviceCommand, "List the devices");

  for ( int deviceNo = 0; deviceNo < deviceMax; deviceNo++ )
  {
    device[ deviceNo ].num( deviceNo + 1 );
    device[ deviceNo ].plugin = NULL;
  }
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

void CoreDevices::devicesCommand(String& res, char** block)
{
  String log = "Devices :";
  CoreLog::add(LOG_LEVEL_INFO, log);

  for (int deviceNo = 0; deviceNo < deviceMax; deviceNo++)
  {
    log = "  ";
    log += deviceNo;
    log += " : ";
    log += *device[ deviceNo ].name();
    log += " (";
    if (device[ deviceNo ].plugin)
      log += *device[ deviceNo ].plugin->name();
    log += ")";
    CoreLog::add(LOG_LEVEL_INFO, log);

    res += log;
  }
}

void CoreDevices::deviceCommand(String& res, char** block)
{
  
}

void CoreDevices::devicesWeb(void)
{
#ifdef LOG_LEVEL_DEBUG
  String log = F("HTTP : GET /devices");
  CoreLog::add(LOG_LEVEL_DEBUG, log);
#endif

  if (!CoreHttp::isLoggedIn())
    return;

  String reply, line, res;

  CoreHttp::pageHeader(reply, MENU_DEVICES);
  reply += res;

  reply += F("<table class='table'>");

  line = F("Devices list");
  CoreHttp::tableHeader(reply, line);
  
  CoreDevices *device = CoreDevices::first();
  while (device)
  {
    String title, form;
    title += device->num();
    title += " : ";
    title += *device->name();

    String name = "device_";
    name += device->num();

    String js = "loading.style.visibility = 'visible'";
    CoreHttp::select(form, name, js);
    CoreHttp::option(form, "None", 0);
    CorePlugins *plugin = CorePlugins::first();
    while (plugin)
    {
      name = *plugin->name();
      name += " : ";
      name += *plugin->desc();
      
      CoreHttp::option(form, name, plugin->num());

      plugin = CorePlugins::next();
    }
    CoreHttp::select(form);

    String url = "/devices?dn=";
    url += device->num();    
    CoreHttp::button(form, "Modify", url);
    
    CoreHttp::tableLine(reply, title, form);
    device = CoreDevices::next();
  }
  
  reply += F("</table>");
  
  CoreHttp::pageFooter(reply);
  WebServer.send(200, texthtml, reply);
}

void CoreDevices::deviceWeb(void)
{
  
}

CoreDevices* CoreDevices::first(void)
{
  deviceNo = 0;
  return &device[ deviceNo ];
}

CoreDevices* CoreDevices::next(void)
{
  if ( ++deviceNo >= deviceNb)
    return NULL;
  return &device[ deviceNo ];
}


