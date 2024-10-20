//
// Grocer.h
//

// Engine includes.
#include "EventStep.h"
#include "Object.h"

// Game includes.
#include "Fruit.h"
#include "util.h"

const std::string GROCER_STRING = "Grocer";

class Grocer : public df::Object {

 private:
  int m_spawn;		 // countdown to next fruit, in ticks
  int m_wave;		 // wave number
  int m_wave_spawn;	 // current wave countdown, in ticks
  int m_wave_end;	 // current wave end, in ticks
  float m_wave_speed;    // current fruit speed, in spaces/tick

  // Handle step events.
  int step(const df::EventStep *p_e);

public:

  // Constructor.
  Grocer();

  // Handle events.
  int eventHandler(const df::Event *p_e) override;

  // Do game over actions.
  void gameOver();

  // Serialize modified attributes to stream.
  // Can specify individual attribute(s) to force (modified or not).
  // Default is only modified attributes.
  // Clears modified bits for attributes serialized.
  // Return 0 if ok, else -1.
  int serialize(std::stringstream *p_ss, unsigned int attr=0) override;

  // Deserialize stream to attributes and apply.
  // p_ss - incoming stream to deserialize.
  // p_a - outgoing bitmask of attributes modified (NULL means no outgoing).
  // Return 0 if ok, else -1.  
  int deserialize(std::stringstream *p_ss, unsigned int *p_a=NULL) override;
};
