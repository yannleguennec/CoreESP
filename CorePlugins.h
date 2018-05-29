#ifndef __CorePlugin__
#define __CorePlugin__

class CorePlugins
{
protected:
  // Infos du plugin
  int    _num;
  String _name;
  String _desc;

  // Infos du device
  String _topic;
  String _comment;
  
public:
  CorePlugins(bool first = false);
  CorePlugins(const char *name, const char *desc, bool first = false);

  void loopFast(void);
  void loopMedium(void);
  void loopSlow(void);

  CorePlugins* factory(void);
  
  void num( int num ) { _num = num; };
  int num( void ) { return _num; };
  
  void name( String& name ) { _name = name; };
  String& name( void ) { return _name; };
  
  void desc( String& desc ) { _desc = desc; };
  String& desc( void ) { return _desc; };

  String& toString(void) { return _name + " : " + _desc; };

  void webForm(String& html);
  void webFormSubmit( void );

  
  void topic( String topic ) { _topic = topic; };
  void topic( String& topic ) { _topic = topic; };
  String& topic( void ) { return _topic; };
  
  void comment( String comment ) { _comment = comment; };
  void comment( String& comment ) { _comment = comment; };
  String& comment( void ) { return _comment; };

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
