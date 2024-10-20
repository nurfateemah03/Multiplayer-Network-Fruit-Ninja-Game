//
// ServerEntry.cpp
//

// Engine includes.
#include "GameManager.h"
#include "LogManager.h"
#include "NetworkManager.h"
#include "WorldManager.h"

// Game includes.
#include "ServerEntry.h"
#include "util.h"

ServerEntry::ServerEntry() {
  setType("ServerEntry");
  setText("");
  setViewString("Server hostname: ");
  setBorder(true);
  setColor(df::YELLOW);
  setLocation(df::CENTER_CENTER);
  setLimit(MAX_WIDTH/2); // In characters
}

// On callback, connect then start yak.
void ServerEntry::callback() {

  LM.writeLog(1, "ServerEntry::callback(): server: %s", getText().c_str());

  // Connect.
  std::string server_port = df::DRAGONFLY_PORT;
  LM.writeLog(1, "ServerEntry::callback(): Connecting to %s at port %s.",
              getText().c_str(), server_port.c_str());
  if (NM.connect(getText(), server_port) < 0) {
    LM.writeLog("ServerEntry::callback(): Error!  Unable to connect.");
    exit(-1);
  }

  LM.writeLog(1, "ServerEntry::callback(): Client connected.");
  
  // Done.
  WM.markForDelete(this);
}
