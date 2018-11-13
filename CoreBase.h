#ifndef __CoreBase__
#define __CoreBase__

#include <forward_list>

#define flagNone       0
#define flagLog        (1 << 0)
#define flagControl    (1 << 1)
#define flagPlugin     (1 << 2)
#define flagMessage    (1 << 3)

#define prioritySystem     0
#define priorityLog        1
#define priorityControl    2
#define priorityPlugin     4
#define priorityCommand    5
#define priorityDevice     6
#define priorityNone       255

// Multitasking : Schedule the launch of (f)unction every (i)nterval in ms.
#define schedule(f,i) {static unsigned long l=0;unsigned long c=millis();if((unsigned long)(c-l)>=i){l=c;f();}}

class CoreBase
{
  public:
    CoreBase(String name);
    virtual ~CoreBase(void);

    // Getters
    inline String& name(void) { return _name; };
    inline int flags(void) { return _flags; };
    inline int priority(void) { return _priority; };

    // Setters
    inline void setName(String& name) { _name = name; };
    inline void setFlags(byte flags) { _flags = flags; };
    inline void addFlags(byte flags) { _flags |= flags; };
    inline void delFlags(byte flags) { _flags &= flags; };
    inline void setPriority(byte priority) { _priority = priority; };

    // virtual fonction to overload
    virtual void setup(void) {};
    virtual void loop(void) {};

    // static fonctions for main
    static void doSetup(void);
    static void doLoop(void);

protected:

    static std::forward_list<CoreBase*> list;
//    static CoreBaseList list;
    byte _priority;
    byte _flags;
    String _name;

    bool operator<( CoreBase& b)
    {
      PANIC_DEBUG();
      Serial.println("operator<()");
      
      return priority() < b.priority();
      //return true;
    } 

};

//bool operator<( CoreBase& a, CoreBase& b)
//{
//    PANIC_DEBUG();
//    
//    return a.priority() < b.priority();
//    //return true;
//} 


#endif
