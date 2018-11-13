#include "myIPAddress.h"

myIPAddress::myIPAddress(const char *address) {
  this->fromString( address );
}
myIPAddress::myIPAddress(String *address) {
  this->fromString( address->begin() );
}

