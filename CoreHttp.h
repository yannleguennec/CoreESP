#ifndef __CoreHttp__
#define __CoreHttp__

#define HTTP_LOGIN_TIMEOUT 60

#define MENU_OFF      0
#define MENU_INFO     1
#define MENU_CONFIG   2
#define MENU_PLUGINS  3
#define MENU_DEVICES  4
#define MENU_RULES    5
#define MENU_TOOLS    6
#define MENU_ADVANCED 7

#include <ESP8266WebServer.h>
extern ESP8266WebServer WebServer;
extern const __FlashStringHelper* texthtml;

class CoreHttp
{
public:
  static void add(String url, void (*func)(void));

  static void setup(void);
  static void loop(void);
  static void loopMedium(void);

  static void pageHeader(String& html, int activeMenu=MENU_OFF);
  static void pageFooter(String &html);
  static void menuItem(String &html, String url, String libelle, int activeMenu);

  // HTML helper functions
  static void tableHeader(String &html, String section);
  static void tableLine(String &hrml, String title, String value);
  static void select(String &html, String name="", String js="");
  static void option(String &html, String name, int value, bool selected=false);
  static void input(String &html, String name, String value, bool hide=false);
  static void button(String &html, String value, String url="");

  static bool isLoggedIn();

  static void handleLogin(void);
  static void handleRoot(void);
  static void handleConfigSave(String &res);
  static void handleConfig(void);
  static void handlePlugins(void);
  static void handleToolsCommands(String &res, String &line);
  static void handleTools(void);
  static void handleLog(void);
};


#endif

