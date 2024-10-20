//
// Sword.cpp
//

// Engine includes.
#include "DisplayManager.h"
#include "EventCollision.h"
#include "EventView.h"
#include "GameManager.h"
#include "LogManager.h"
#include "NetworkManager.h"
#include "WorldManager.h"

// Game includes.
#include "Client.h"
#include "Fruit.h"
#include "Grocer.h"
#include "Kudos.h"
#include "Points.h"
#include "Sword.h"
#include "Timer.h"
#include "util.h"

Sword::Sword() {

  setType(SWORD_STRING);
  setSolidness(df::SPECTRAL);
  setAltitude(df::MAX_ALTITUDE); // Make Sword in foreground.

  // Start sword in center of world.
  df::Vector p(WM.getBoundary().getHorizontal()/2,
	           WM.getBoundary().getVertical()/2);
  setPosition(p);

  // Step event to handle movement.
  registerInterest(df::STEP_EVENT);

  // The server gets network mouse events to move sword.
  if (NM.isServer() == true)
    registerInterest(df::NETWORK_MSE_EVENT);

  // Initialize remaning attributes.
  setColor(df::CYAN);
  m_old_position = getPosition();
  m_sliced = 0;
  m_old_sliced = 0;
}

void Sword::setColor(df::Color new_color) {
  m_color = new_color;
}

// Handle event.
// Return 0 if ignored, else 1.
int Sword::eventHandler(const df::Event *p_e) {

  // Step event.
  if (p_e->getType() == df::STEP_EVENT)
    return step((df::EventStep *) p_e);

  // Network mouse message.
  if (p_e->getType() == df::NETWORK_MSE_EVENT)
    return mouseNetwork((df::EventMouseNetwork *) p_e);

  // If get here, have ignored this event.
  return 0;
}

// Get color.
df::Color Sword::getColor() const {
  return m_color;
}

// Set socket index.
void Sword::setSocketIndex(int new_sock_index) {
  m_sock_index = new_sock_index;
}
  
// Get socket index.
int Sword::getSocketIndex() const {
  return m_sock_index;
}

// Handle step event.
int Sword::step(const df::EventStep *p_e) {

  // If didn't move, nothing to do.
  if (m_old_position == getPosition()) {
    m_sliced = 0;
    return 1;
  }

  // If client, make a trail, nothing else.
  if (NM.isServer() == false) {
    create_trail(getPosition(), m_old_position, getColor());
    m_old_position = getPosition();
    return 1;
  }

  // Only the Server checks for slicing and adjusts points.

  ////////////////////////////////////////////////////
  // SLICING
  // Check if line intersect any Fruit.
  df::Line line(getPosition(), m_old_position);
  df::ObjectList ol = WM.solidObjects();

  for (int i=0; i<ol.getCount(); i++) {
    
    // Only slice Fruit.
    if (!(dynamic_cast <Fruit *> (ol[i])))
      continue;

    // If line from previous position intersects --> slice!
    df::Object *p_o = ol[i];
    df::Box box = getWorldBox(p_o);
    if (lineIntersectsBox(line, box)) {
      df::EventCollision c(this, p_o, p_o->getPosition());
      p_o -> eventHandler(&c);
      m_sliced += 1;
      
      // Kudos for combo, sent to just the player that earned.
      if (m_sliced > 2 && m_sliced > m_old_sliced)
	new Kudos(getSocketIndex());
	  
      m_old_sliced = m_sliced;
      
    } // End of box-line check.
    
  } // End of loop through all objects.

  ////////////////////////////////////////////////////
  // POINTS
  // Compute how far travel.
  float dist = df::distance(getPosition(), m_old_position);

  // If travel far enough, play "swipe" sound.
  if (dist > 15) {
    std::string sound = "swipe-" + std::to_string(rand()%7 + 1);
    play_sound(sound);
  }

  // Lose points for distance traveled.
  int penalty = -1 * (int)(dist/10.0f);
  if (penalty != 0) {
    std::string s = POINTS_STRING;
    s += ":";
    s += df::toString(getColor());
    df::EventView ev(s, penalty, true);
    WM.onEvent(&ev);
  }
  
  m_old_position = getPosition();
  
  return 1;
}

// Handle network mouse event.
int Sword::mouseNetwork(const df::EventMouseNetwork *p_e) {

  // Mouse not for this Sword.
  if (p_e -> getSocketIndex() != this -> getSocketIndex())
    return 0;

  // Change Sword position.
  LM.writeLog(1, "Sword::mouseNetwork():: mouse xy: %s",
	      p_e -> getMousePosition().toString().c_str());
  setPosition(p_e -> getMousePosition());
  
  return 1;
}

// Draw sword on window.
int Sword::draw() {
  if (NM.isServer() == false)
    return DM.drawCh(getPosition(), SWORD_CHAR, m_color);
  else
    return 0;
}

int Sword::serialize(std::stringstream *p_ss, unsigned int attr) {

  LM.writeLog(20, "Sword::serialize(): attr is %s",
	      df::maskToString(attr).c_str());

  // Serialize parent first. 
  if (Object::serialize(p_ss, attr))
    LM.writeLog(20, "Sword::serialize(): error calling Object serialize");

  // Serialize remaining attributes.
  p_ss->write(reinterpret_cast<char*>(&m_color), sizeof(m_color));
  LM.writeLog(25, "\tCOLOR: %s", df::toString(m_color).c_str());

  m_old_position.serialize(p_ss);
  LM.writeLog(25, "\tPOSITION: %s", m_old_position.toString().c_str());

  p_ss->write(reinterpret_cast<char*>(&m_sliced), sizeof(m_sliced));
  LM.writeLog(25, "\tSLICED: %s", df::toString(m_sliced).c_str());

  p_ss->write(reinterpret_cast<char*>(&m_old_sliced), sizeof(m_old_sliced));
  LM.writeLog(25, "\tOLD_SLICED: %s", df::toString(m_old_sliced).c_str());

  if (p_ss->good())
    return 0;  // All is well.
  else
    return -1; // Error.
}

int Sword::deserialize(std::stringstream *p_ss, unsigned int *p_a) {

  LM.writeLog(20, "Sword::deserialize():");

  // Deserialize parent, first.
  int ret = Object::deserialize(p_ss, p_a);
  if (ret != 0) {
    LM.writeLog("Sword::deserialize(): Error calling Object::deserialize().");
    return ret;
  }

  // Deserialize local attributes.
  df::Color color;
  p_ss->read(reinterpret_cast<char*>(&color), sizeof(color));
  m_color = color;
  LM.writeLog(25, "\tCOLOR: %s", df::toString(m_color).c_str());

  df::Vector pos;
  pos.deserialize(p_ss);
  m_old_position = pos;
  LM.writeLog(25, "\tPOSITION: %s", m_old_position.toString().c_str());

  int sliced;
  p_ss->read(reinterpret_cast<char*>(&sliced), sizeof(sliced));
  m_sliced = sliced;
  LM.writeLog(25, "\tSLICED: %s", df::toString(m_old_sliced).c_str());

  int old_sliced;
  p_ss->read(reinterpret_cast<char*>(&old_sliced), sizeof(old_sliced));
  m_old_sliced = sliced;
  LM.writeLog(25, "\tOLD_SLICED: %s", df::toString(m_old_sliced).c_str());

  if (p_ss->good())
    return 0;  // All is well.
  else
    return -1; // Error.
}
