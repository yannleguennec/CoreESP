#ifndef __myIPAddress_h__
#define __myIPAddress_h__

#include "IPAddress.h"

class myIPAddress : public IPAddress
{
public:
  myIPAddress(const char *address);
};

#endif
