//
// Grocer.cpp
//

// System includes.
#include <stdlib.h>		// for rand()
#include <string.h>

// Engine includes.
#include "EventStep.h"
#include "GameManager.h"
#include "LogManager.h"
#include "WorldManager.h"

// Game includes.
#include "Fruit.h"
#include "GameOver.h"
#include "Grocer.h"
#include "Server.h"

Grocer::Grocer(){
  setType(GROCER_STRING);
  setSolidness(df::SPECTRAL);
  setVisible(false);
  registerInterest(df::STEP_EVENT);
  m_wave = 1;
  m_wave_end = WAVE_LEN;
  m_wave_speed = WAVE_SPEED; // Starting speed (spaces/tick).
  m_wave_spawn = WAVE_SPAWN; // Starting groc rate (ticks).
  m_spawn = m_wave_spawn;
  LM.writeLog(1, "Grocer::Grocer(): Grocer started.");
}

// Handle event.
// Return 0 if ignored, else 1.
int Grocer::eventHandler(const df::Event *p_e) {

  // Step event.
  if (p_e->getType() == df::STEP_EVENT)
    return step((df::EventStep *) p_e);

  // If get here, have ignored this event.
  return 0;
}

// Handle step event.
int Grocer::step(const df::EventStep *p_e) {

  LM.writeLog(5, "Grocer::step(): wave %d, spawn %d", m_wave, m_spawn);

  // Fruit grocer.
  m_spawn -= 1;
  if (m_spawn < 0) {

    int mod = m_wave+1 > NUM_FRUITS ? NUM_FRUITS : m_wave+1;
    int num = rand() % mod;
    LM.writeLog(1, "Grocer::step(): wave %d, mod %d, num %d, creating fruit %s",
		m_wave, mod, num, FRUIT[num].c_str()); 
    Fruit *p_f = new Fruit(FRUIT[num]);
    if (!p_f) {
      LM.writeLog("Grocer::step(): Error! Unable to allocate Fruit.");
      return 0;
    }

    p_f -> start(m_wave_speed);

    m_spawn = m_wave_spawn;
  }

  // Advance wave.
  m_wave_end -= 1;
  if (m_wave_end < 0) {

    m_wave_end = WAVE_LEN;
    m_wave_spawn += SPAWN_INC; // Increase groc rate.
    m_wave_speed += SPEED_INC; // Increase Fruit speed.
    m_wave += 1;
    if (m_wave == NUM_WAVES+1) {
      LM.writeLog(1, "Grocer::step() waves is %d, NUM_WAVES+1 is %d",
		  m_wave, NUM_WAVES + 1);
      LM.writeLog(1, "Grocer::step() calling gameOver()");
      this->gameOver();

      // Send message to Client(s).
      char buff[] = "game over";
      SERVER -> sendMessage(df::MessageType::CUSTOM_MESSAGE, (int) strlen(buff), buff, -1);
    }
  }
    
  return 1;
}

// Do game over actions.
void Grocer::gameOver() {
  new GameOver();
  WM.markForDelete(this);
  
  // Destroy all remaining Fruit (no points).
  df::ObjectList ol = WM.solidObjects();
  for (int i=0; i<ol.getCount(); i++)
    if (dynamic_cast <Fruit *> (ol[i]))
      WM.markForDelete(ol[i]);
}

int Grocer::serialize(std::stringstream *p_ss, unsigned int attr) {

  LM.writeLog(20, "Grocer::serialize(): attr is %s",
	      df::maskToString(attr).c_str());

  // Serialize parent first. 
  if (Object::serialize(p_ss, attr))
    LM.writeLog(20, "Grocer::serialize(): error calling Object serialize");

  // Serialize remaining attributes.
  p_ss->write(reinterpret_cast<char*>(&m_spawn), sizeof(m_spawn));
  p_ss->write(reinterpret_cast<char*>(&m_wave), sizeof(m_wave));
  p_ss->write(reinterpret_cast<char*>(&m_wave_spawn), sizeof(m_wave_spawn));
  p_ss->write(reinterpret_cast<char*>(&m_wave_end), sizeof(m_wave_end));
  p_ss->write(reinterpret_cast<char*>(&m_wave_speed), sizeof(m_wave_speed));

  if (p_ss->good())
    return 0;  // All is well.
  else
    return -1; // Error.
}

int Grocer::deserialize(std::stringstream *p_ss, unsigned int *p_a) {

  LM.writeLog(20, "Grocer::deserialize():");

  // Deserialize parent, first.
  int ret = Object::deserialize(p_ss, p_a);
  if (ret != 0) {
    LM.writeLog("Grocer::deserialize(): Error calling Object::deserialize().");
    return ret;
  }

  // Deserialize local attributes.
  int spawn;
  p_ss->read(reinterpret_cast<char*>(&spawn), sizeof(spawn));
  m_spawn = spawn;
  LM.writeLog(20, "Grocer::deserialize(): m_spawn %d", m_spawn);

  int wave;
  p_ss->read(reinterpret_cast<char*>(&wave), sizeof(wave));
  m_wave = wave;
  LM.writeLog(20, "Grocer::deserialize(): m_wave %d", m_wave);

  int wave_groc;
  p_ss->read(reinterpret_cast<char*>(&wave_groc), sizeof(wave_groc));
  m_wave_spawn = wave_groc;
  LM.writeLog(20, "Grocer::deserialize(): m_wave_spawn %d", m_wave_spawn);

  int wave_end;
  p_ss->read(reinterpret_cast<char*>(&wave_end), sizeof(wave_end));
  m_wave_end = wave_end; 
  LM.writeLog(20, "Grocer::deserialize(): m_wave_end %d", m_wave_end);

  float wave_speed;
  p_ss->read(reinterpret_cast<char*>(&wave_speed), sizeof(wave_speed));
  m_wave_speed = wave_speed;
  LM.writeLog(20, "Grocer::deserialize(): m_wave_speed %.2f", m_wave_speed);

  if (p_ss->good())
    return 0;  // All is well.
  else
    return -1; // Error.
}
