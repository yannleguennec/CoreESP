#ifndef __CorePlugin__
#define __CorePlugin__

class CorePlugins
{
protected:
  bool _saved;
  
  // Infos du plugin
  int    _pluginNumber;
  String _pluginName;
  String _pluginDesc;

  // Infos du device
  String _deviceTopic;
  String _deviceComment;
  
public:
  CorePlugins(void) { _saved = false; };
  CorePlugins(String pluginName, String pluginDesc);
  void registerPlugin();

  void loopFast(void);
  void loopMedium(void);
  void loopSlow(void);

  virtual CorePlugins* factory(void);
  
  void pluginNumber( int pluginNumber ) { _pluginNumber = pluginNumber; };
  int pluginNumber( void ) { return _pluginNumber; };
  
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

  static void setup(void);
  
  static void listCommand(String &, char **);
  static void listWeb( void );
  
  static CorePlugins *first(void);
  static CorePlugins *next(void);

};

#define pluginMax 20
extern CorePlugins *plugin[ pluginMax ];
extern int pluginNb;

#endif
