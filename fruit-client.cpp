//
// Fruit Ninjas - multiplayer fruit ninja
// 

// System includes.
#if !defined(_WIN32) && !defined(_WIN64)
#include <unistd.h> // for getpid()
#else
#include <process.h>
#endif

// Engine includes.
#include "GameManager.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "WorldManager.h"
#include "utility.h"

// Game includes.
#include "Client.h"
#include "util.h"

///////////////////////////////////////////////
int main(int argc, char *argv[]) {

  // Set environment for config file (server).
#if defined(_WIN32) || defined(_WIN64)
  _putenv_s("DRAGONFLY_CONFIG", "df-config-client.txt");
#else
  setenv("DRAGONFLY_CONFIG", "df-config-client.txt", 1);
#endif

  // Set environment for logfile (client + pid).
#if defined(_WIN32) || defined(_WIN64)
  int pid = _getpid();
#else
  int pid = getpid();
#endif    
  std::string logfile = "log";
  logfile += std::to_string(pid);
  logfile += ".log";
#if defined(_WIN32) || defined(_WIN64)
  _putenv_s("DRAGONFLY_LOG", logfile.c_str());
#else
  setenv("DRAGONFLY_LOG", logfile.c_str(), 1);
#endif

  // Start up game manager.
  if (GM.startUp())  {
    LM.writeLog("Error starting game manager!");
    GM.shutDown();
    return 0;
  }

  // Setup logging.
  LM.setLogLevel(0);
  LM.setFlush(true);
  LM.writeLog("Fruit Ninjas (v%.1f)", VERSION);
  
  // Load resources.
  loadResources();

  // Dragonfly splash screen.
  //df::splash();

  // Fruit Ninja splash screen.
  //splash();

  new Client;

  // Run game (this blocks until game loop is over).
  GM.run();

  // Shut everything down.
  GM.shutDown();

  // All is well.
  return 0;
}

