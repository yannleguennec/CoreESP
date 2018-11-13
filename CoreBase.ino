#include "CoreBase.h"

std::forward_list<CoreBase*> CoreBase::list;
//CoreBase::CoreBaseList CoreBase::list;

CoreBase::CoreBase(String name) : _priority(priorityNone), _flags(flagNone), _name(name)
{
  list.push_front(this);
} 

CoreBase::~CoreBase(void)
{
  list.remove(this);
}

//void CoreBase::Register( Flags newflags )
//{
//  flags = newflags;
//  list.push_front(this);
//};

class ascendingPriority
{
public:
  bool operator()( CoreBase* left, CoreBase* right) { return left->priority() < right->priority(); };
};

void CoreBase::doSetup(void)
{
  PANIC_DEBUG();

  // do the sorting only on setup, its done once for all.
  list.sort( ascendingPriority() );
  
  for ( CoreBase* base : list )
    base->setup();
}

void CoreBase::doLoop(void)
{
//  PANIC_DEBUG();

  for ( CoreBase* base : list )
    base->loop();
}

