#ifndef __CorePlugin__
#define __CorePlugin__

#include <vector>

class CorePlugins : public CoreBase
{
protected:
  static size_t _pluginsNb;

  bool _saved;
  
  // Infos du plugin
  size_t    _pluginNumber;
  String _pluginName;
  String _pluginDesc;

  // Infos du device
  String _deviceName;
  String _deviceTopic;
  String _deviceComment;
  
public:
  CorePlugins(void);
  //CorePlugins(String& pluginName, String& pluginDesc);
  CorePlugins(const char* name, const char* desc);
  
  virtual ~CorePlugins() {};

  void registerPlugin( String &pluginName, String &pluginDesc );

  virtual void setup(void);
  virtual void loop(void);
  
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

  void deviceName( String& name ) { _deviceName = name; };
  String& deviceName( void ) { return _deviceName; };

  void deviceTopic( String& topic ) { _deviceTopic = topic; };
  String& deviceTopic( void ) { return _deviceTopic; };
  
  void deviceComment( String& comment ) { _deviceComment = comment; };
  String& deviceComment( void ) { return _deviceComment; };

  bool saved(void) { return _saved; };
  void save(void) { _saved = true; };
  
  static void listCommand(String &, char **);
  static void listWeb( void );

  static size_t pluginsNb(void) { return _pluginsNb; };
};

#endif
