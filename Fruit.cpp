//
// Fruit.cpp
//

// Engine includes.
#include "EventCollision.h"
#include "EventOut.h"
#include "EventView.h"
#include "GameManager.h"
#include "LogManager.h"
#include "NetworkManager.h"
#include "WorldManager.h"

// Game includes.
#include "Fruit.h"
#include "Points.h"
#include "Server.h"
#include "Sword.h"
#include "util.h"

Fruit::Fruit(std::string name) {

  setType(name);
  if (setSprite(name) != 0)
    LM.writeLog("Fruit::Fruit(): Error! Unable to find sprite: %s",
		name.c_str());
  m_first_out = true; // To ignore first time outofbounds.
  setSolidness(df::SOFT);
}

// Handle event.
int Fruit::eventHandler(const df::Event *p_e) {

  // Collision event.
  if (p_e -> getType() == df::COLLISION_EVENT && NM.isServer())
    return collide((df::EventCollision *) p_e);

  // Step event.
  if (p_e -> getType() == df::OUT_EVENT && NM.isServer())
    return out((df::EventOut *) p_e);

  // Not handled.
  return 0;
}

// Handle out events.
int Fruit::out(const df::EventOut *p_e) {

  if (m_first_out) { // Ignore first out.
    m_first_out = false;
    return 1;
  }

  // All players lose points for each miss.
  for (int i=0; i<NM.getNumConnections(); i++) {
    std::string s = "Player ";
    s += df::toString(i);
    s += ":";
    df::EventView ev(s, -25, true);
    WM.onEvent(&ev);
  }

  WM.markForDelete(this);

  // Send DELETE message to all clients.
  if (NM.isServer()) {
    LM.writeLog(1, "Fruit::out(): Sending DELETE message....");
    SERVER -> sendMessage(df::MessageType::DELETE_OBJECT, this);
  }

  // Handled.
  return 1;
}

// Handle collision events.
int Fruit::collide(const df::EventCollision *p_e) {

  if (p_e -> getObject1() -> getType() == "Sword") {

    // Add points.
    int sock_index = ((Sword *) (p_e -> getObject1())) -> getSocketIndex();
    std::string s = "Player ";
    s += df::toString(sock_index);
    s += ":";
    df::EventView ev(s, 10, true);
    WM.onEvent(&ev);

    WM.markForDelete(this);

    // Send DELETE message to all clients.
    if (NM.isServer()) {
      LM.writeLog(1, "Fruit::collide(): Sending DELETE message....");
      SERVER -> sendMessage(df::MessageType::DELETE_OBJECT, this);
    }

  }

  // Handled.
  return 1;
}

// Destructor.
Fruit::~Fruit() {

  // If inside the game world and engine not shutting down,
  // create explosion and play sound.
  if (df::boxContainsPosition(WM.getBoundary(), getPosition()) &&
      GM.getGameOver() == false) {
    df::explode(getAnimation().getSprite(), getAnimation().getIndex(), getPosition(),
                EXPLOSION_AGE, EXPLOSION_SPEED, EXPLOSION_ROTATE);

    // Play "splat" sound.
    std::string sound = "splat-" + std::to_string(rand()%6 + 1);
    play_sound(sound);
  }
}

// Setup starting conditions.
void Fruit::start(float speed) {

  df::Vector pos1, pos2;

  // Get world boundaries.
  int world_x = (int) WM.getBoundary().getHorizontal();
  int world_y = (int) WM.getBoundary().getVertical();
  df::Vector world_center(world_x/2.0f, world_y/2.0f);

  // Pick random side to spawn.
  switch (rand() % 4) {

  case 0: // Top.
    pos1.setX((float) (rand()%world_x));
    pos1.setY(0 - 3.0f);
    pos2.setX((float) (rand()%world_x));
    pos2.setY(world_y + 3.0f);
    break;

  case 1: // Right.
    pos1.setX(world_x + 3.0f);
    pos1.setY((float) (rand()%world_y));
    pos2.setX(0 - 3.0f);
    pos2.setY((float) (rand()%world_y));
    break;

  case 2: // Bottom.
    pos1.setX((float) (rand()%world_x));
    pos1.setY(world_y + 3.0f);
    pos2.setX((float) (rand()%world_x));
    pos2.setY(0 - 3.0f);
    break;
    
  case 3: // Left.
    pos1.setX(0 - 3.0f);
    pos1.setY((float) (rand()%world_y));
    pos2.setX(world_x + 3.0f);
    pos2.setY((float) (rand()%world_y));
    break;

  default:
    break;
  }

  // Move Object into position.
  WM.moveObject(this, pos1);

  // Set velocity towards opposite side.
  df::Vector vel = pos2 - pos1;
  vel.normalize();
  setDirection(vel);
  setSpeed(speed);
}

int Fruit::serialize(std::stringstream *p_ss, unsigned int attr) {

  LM.writeLog(20, "Fruit::serialize(): attr is %s",
	      df::maskToString(attr).c_str());

  // Serialize parent first. 
  if (Object::serialize(p_ss, attr))
    LM.writeLog(20, "Fruit::serialize(): error calling Object serialize");

  // Serialize remaining attributes.
  p_ss->write(reinterpret_cast<char*>(&m_first_out), sizeof(m_first_out));

  if (p_ss->good())
    return 0;  // All is well.
  else
    return -1; // Error.
}

int Fruit::deserialize(std::stringstream *p_ss, unsigned int *p_a) {

  LM.writeLog(20, "Fruit::deserialize():");

  // Deserialize parent, first.
  int ret = Object::deserialize(p_ss, p_a);
  if (ret != 0) {
    LM.writeLog("Fruit::deserialize(): Error calling Object::deserialize().");
    return ret;
  }

  // Deserialize local attributes.
  bool first_out;
  p_ss->read(reinterpret_cast<char*>(&first_out), sizeof(first_out));
  m_first_out = first_out;
  LM.writeLog(20, "Fruit::deserialize(): m_first_out is %s",
	      m_first_out ? "true" : "false");

  if (p_ss->good())
    return 0;  // All is well.
  else
    return -1; // Error.
}
df::Vector Fruit::predictPosition() {
    df::Vector current_pos = getPosition();
    df::Vector velocity = getVelocity(); // Assuming there's a method to get velocity

    // Predict the next position
    df::Vector predicted_pos = current_pos + velocity;
    return predicted_pos;
}
