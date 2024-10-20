//
// Fruit Ninjas - multiplayer fruit ninja
//

// System includes.

// Engine includes.
#include "GameManager.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "WorldManager.h"
#include "utility.h"

// Game includes.
#include "Server.h"
#include "util.h"

///////////////////////////////////////////////
int main(int argc, char *argv[]) {

  // Set environment for config file (server).
#if defined(_WIN32) || defined(_WIN64)
  _putenv_s("DRAGONFLY_CONFIG", "df-config-server.txt");
#else
  setenv("DRAGONFLY_CONFIG", "df-config-server.txt", 1);
#endif

  // Start up game manager.
  if (GM.startUp())  {
    LM.writeLog("Error starting game manager!");
    GM.shutDown();
    return 0;
  }

  // Setup logging.
  LM.setFlush(true);
  LM.setLogLevel(0);
  LM.writeLog("Fruit Ninjas (v%.1f)", VERSION);
  
  // Load resources.
  loadResources();

  // Start server game object.
  new Server();

  // Run game (this blocks until game loop is over).
  GM.run();

  // Shut everything down.
  GM.shutDown();

  // All is well.
  return 0;
}

