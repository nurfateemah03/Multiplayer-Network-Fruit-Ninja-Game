//
// GameOver.cpp
//

// Engine includes.
#include "DisplayManager.h"
#include "EventStep.h"
#include "GameManager.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "WorldManager.h"

// Game includes.
#include "GameOver.h"
#include "util.h"

GameOver::GameOver() {

  setType(GAMEOVER_STRING);
  setDrawValue(false);

  // Animate "game over" sprite one time.
  if (setSprite("gameover") == 0)
    m_time_to_live = getAnimation().getSprite()->getFrameCount() * getAnimation().getSprite()->getSlowdown();
  else
    m_time_to_live = 0;
  LM.writeLog(1, "GameOver::GameOver(): m_time_to_live %d", m_time_to_live);

  // Put in center of window.
  setLocation(df::CENTER_CENTER);
  
  // Register for step event.
  registerInterest(df::STEP_EVENT);

  // Shake screen (severity 20 pixels x&y, duration 10 frames).
  DM.shake(20, 20, 10);

  LM.writeLog(1, "GameOver::GameOver(): created");
}

// Handle event.
int GameOver::eventHandler(const df::Event *p_e) {

  // Step event.
  if (p_e->getType() == df::STEP_EVENT) {
    step();
    return 1;
  }

  // If get here, have ignored this event.
  return 0;
}

// Count down to end of message.
int GameOver::step() {

  m_time_to_live--;

  if (m_time_to_live <= 0) {
    GM.setGameOver(true);
    WM.markForDelete(this);
  }

  if (m_time_to_live == 175)
    play_sound("game-over");

  // Handled.
  return 1;
}

int GameOver::draw() {
  return Object::draw();
}

int GameOver::serialize(std::stringstream *p_ss, unsigned int attr) {

  LM.writeLog(20, "GameOver::serialize(): attr is %s",
	      df::maskToString(attr).c_str());

  // Serialize parent first. 
  if (Object::serialize(p_ss, attr))
    LM.writeLog(20, "GameOver::serialize(): error calling Object serialize");

  // Serialize remaining attributes.
  p_ss->write(reinterpret_cast<char*>(&m_time_to_live), sizeof(m_time_to_live));
  LM.writeLog(20, "GameOver::serialize(): wrote m_time_to_live: %d", m_time_to_live);

  if (p_ss->good())
    return 0;  // All is well.
  else
    return -1; // Error.
}

int GameOver::deserialize(std::stringstream *p_ss, unsigned int *p_a) {

  LM.writeLog(20, "GameOver::deserialize():");

  // Deserialize parent, first.
  int ret = Object::deserialize(p_ss, p_a);
  if (ret != 0) {
    LM.writeLog("GameOver::deserialize(): Error calling Object::deserialize().");
    return ret;
  }

  // Deserialize local attributes.
  int time_to_live;
  p_ss->read(reinterpret_cast<char*>(&time_to_live), sizeof(time_to_live));
  m_time_to_live = time_to_live;
  LM.writeLog(20, "GameOver::deserialize(): m_time_to_live is %d", m_time_to_live);

  // Put in center of window since may have been
  // changed based on server window size.
  setLocation(df::CENTER_CENTER);

  if (p_ss->good())
    return 0;  // All is well.
  else
    return -1; // Error.
}
