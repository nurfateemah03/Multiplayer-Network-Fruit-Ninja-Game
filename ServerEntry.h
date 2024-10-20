//
// ServerEntry - Uses TextEntry to get server name.
//

#ifndef SERVER_ENTRY_H
#define SERVER_ENTRY_H

// Engine includes.
#include "TextEntry.h"

class ServerEntry : public df::TextEntry {

 public:
  ServerEntry();
  void callback();
};

#endif // SERVER_ENTRY_H
