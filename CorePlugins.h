#ifndef __CorePlugin__
#define __CorePlugin__

class CorePlugins
{
  int    _num;
  String _name;
  String _desc;
  
public:
  CorePlugins(bool first = false);
  CorePlugins(const char *name, const char *desc, bool first = false);

  void loopFast(void);
  void loopMedium(void);
  void loopSlow(void);

  CorePlugins* factory(void);
  
  int num( int num = 0 );
  String* name( String* name = NULL );
  String* desc( String* desc = NULL );

  static void setup(void);
  
  static void listPlugins(String &, char **);

  static void webPlugins( void );
  static void webForm( String &res ) {};
  
  static CorePlugins *first(void);
  static CorePlugins *next(void);

};

#endif
