//
// GameOver.h
//

#ifndef GAMEOVER_H
#define GAMEOVER_H

#include "ViewObject.h"

const std::string GAMEOVER_STRING = "GameOver";

class GameOver : public df::ViewObject {

 private:
  int m_time_to_live;		// in ticks

  // Handle step events.
  int step();

public:
  // Default constructor;
  GameOver();

  // Handle events
  int eventHandler(const df::Event *p_e) override;

  // Draw sprite.
  int draw() override;

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

#endif // GAMEOVER_H
