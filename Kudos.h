//
// Kudos.h
//

#ifndef KUDOS_H
#define KUDOS_H

// System includes.
#include <string>

// Engine includes.
#include "Event.h"
#include "EventStep.h"
#include "Object.h"

const std::string KUDOS_STRING = "Kudos";

class Kudos : public df::Object {

 private:
  int m_countdown;	   // message lifetime, in ticks
  int m_sock_index;	   // socket index at server (doesn't need to be serialized)

  // Handle step events.
  int step(const df::EventStep *p_e);

public:

  // Constructor.
  Kudos(int sock_index=-1);

  // Handle events.
  int eventHandler(const df::Event *p_e) override;

  // Get socket index.
  int getSocketIndex() const;
};

#endif // KUDOS_H
