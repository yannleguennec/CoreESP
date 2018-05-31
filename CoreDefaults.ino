#ifndef __CoreDefaults__
#define __CoreDefaults__

#define SETTING_VERSION 1

#define DEFAULT_NAME          "CoreESP"
#define DEFAULT_VERSION       CoreSystem::getVersion()
#define DEFAULT_BUILD         CoreSystem::getBuild()
#define DEFAULT_NOTES         "Alpha version"
#define DEFAULT_UNIT          0
#define DEFAULT_PASSWORD      ""
//#define DEFAULT_PASSWORD      "yann"

#define DEFAULT_SSID   ""
#define DEFAULT_PWD    ""

#define DEFAULT_AP_SSID "Yann_"
#define DEFAULT_AP_PWD "configesp"

#define DEFAULT_IP         "0.0.0.0"
#define DEFAULT_NETMASK    "0.0.0.0"
#define DEFAULT_GATEWAY    "0.0.0.0"
#define DEFAULT_DNS        "8.8.8.8"

#define DEFAULT_AP_IP      "192.168.12.13"
#define DEFAULT_AP_NETMASK "255.255.255.0"
#define DEFAULT_AP_GATEWAY "0.0.0.0"

#define DEFAULT_SERIALLOGLEVEL LOG_LEVEL_DEBUG
#define DEFAULT_SYSLOGLOGLEVEL LOG_LEVEL_NONE
#define DEFAULT_HTTP_LOGLEVEL  LOG_LEVEL_NONE
#define DEFAULT_MQTTLOGLEVEL   LOG_LEVEL_NONE

#endif
