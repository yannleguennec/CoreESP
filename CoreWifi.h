#ifndef __CoreWifi__
#define __CoreWifi__

#define wifiIsAP()           (WiFi.getMode() & WIFI_AP)
#define wifiIsConnected()    (WiFi.status() == WL_CONNECTED)
#define wifiIsDisconnected() (WiFi.status() == WL_DISCONNECTED)
#define wifiIsNotAvailable() (WiFi.status() == WL_NO_SSID_AVAIL)
#define wifiIsInactive()     (WiFi.status() == WL_IDLE_STATUS)

class CoreWifi
{
public:
  static void setup(void);
  static void loopMedium(void);
private:
  static void displayState(void);
  static void displayStatus(void);

  static void configure(void);
  static void configHostname(void);
  static void configAP(void);

  static void setAPMode(void);
  static void waitAPMode(void);

  static void connect(void);
  static void waitConnect(void);
  static void connected(void);

  static void disconnect(void);
  static void waitDisconnect(void);
  
public:
  static void connectCommand(String &res, char **b=NULL);
  static void disconnectCommand(String &res, char**b=NULL);
  static void setAPModeCommand(String &res, char **b=NULL);
  static void resetCommand(String &res, char**);
  static void pingCommand(String &res, char **block);
  static void scanAPCommand(String &res, char **block);
};

#endif
