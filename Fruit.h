//
// Fruit.h
//

#ifndef FRUIT_H
#define FRUIT_H

// System includes.
#include <string>

// Engine includes.
#include "Event.h"
#include "EventCollision.h"
#include "EventOut.h"
#include "Object.h"

class Fruit : public df::Object {

 private:
  bool m_first_out;

  // Handle out events.
  int out(const df::EventOut *p_e);

  // Handle collision events.
  int collide(const df::EventCollision *p_e);

public:

  // Constructor.
  Fruit(std::string name);

  // Destructor.
  ~Fruit();
  df::Vector predictPosition();


  // Handle events.
  int eventHandler(const df::Event *p_e) override;

  // Setup starting conditions.
  void start(float speed);

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

#endif // FRUIT_H
