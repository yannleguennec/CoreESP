#ifndef __CorePlugin__
#define __CorePlugin__

#include "Array.h"
#define pluginsMax 20

class CorePlugins
{
protected:
  bool _saved;
  
  // Infos du plugin
  size_t    _pluginNumber;
  String _pluginName;
  String _pluginDesc;

  // Infos du device
  String _deviceTopic;
  String _deviceComment;
  
public:
  static std::array<CorePlugins*, pluginsMax> plugins;
  static size_t pluginNb;

  CorePlugins(void) { _saved = false; };
  CorePlugins(String pluginName, String pluginDesc);
  virtual ~CorePlugins() {};

  void registerPlugin( String &pluginName, String &pluginDesc );

  void loopFast(void);
  void loopMedium(void);
  void loopSlow(void);

  virtual CorePlugins* factory(void);
  
  void pluginNumber( size_t pluginNumber ) { _pluginNumber = pluginNumber; };
  size_t pluginNumber( void ) { return _pluginNumber; };
  
  void pluginName( String& pluginName ) { _pluginName = pluginName; };
  String& pluginName( void ) { return _pluginName; };
  
  void pluginDesc( String& pluginDesc ) { _pluginDesc = pluginDesc; };
  String& pluginDesc( void ) { return _pluginDesc; };

  String toString(void) { return _pluginName + " : " + _pluginDesc; };

  virtual void webMenu(String& html, int activeMenu);
  virtual void webForm(String& html);
  virtual void webSubmit( void );

  void deviceTopic( String& deviceTopic ) { _deviceTopic = deviceTopic; };
  String& deviceTopic( void ) { return _deviceTopic; };
  
  void deviceComment( String& deviceComment ) { _deviceComment = deviceComment; };
  String& deviceComment( void ) { return _deviceComment; };

  bool saved(void) { return _saved; };
  void save(void) { _saved = true; };

  virtual void setup(void);
  
  static void listCommand(String &, char **);
  static void listWeb( void );

  size_t size(void) { return pluginNb; };
};

extern CorePlugins corePlugins;

#endif
