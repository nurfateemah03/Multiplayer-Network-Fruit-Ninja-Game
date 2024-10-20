//
// Sword.h
//

#ifndef SWORD_H
#define SWORD_H

// Engine includes.
#include "Color.h"
#include "EventMouseNetwork.h"
#include "EventStep.h"
#include "Object.h"

#define SWORD_CHAR '+'
const std::string SWORD_STRING = "Sword";

class Sword : public df::Object {

 private:
  df::Color m_color;         // trail color
  df::Vector m_old_position; // previous position
  int m_sliced;		     // fruits sliced this move
  int m_old_sliced;	     // previous sliced
  int m_sock_index;	     // socket index at server (doesn't need to be serialized)
  int player_index;

  // Handle step event.
  int step(const df::EventStep *p_e);

  // Handle network mouse event.
  int mouseNetwork(const df::EventMouseNetwork *p_e);

 public:
     void setPlayerIndex(int index) {
         player_index = index;
     }

     int getPlayerIndex() const {
         return player_index;
     }
  // Constructor.
  Sword();

  // Handle events.
  int eventHandler(const df::Event *p_e) override;

  // Set color.
  void setColor(df::Color new_color);

  // Get color.
  df::Color getColor() const;

  // Set socket index.
  void setSocketIndex(int new_sock_index);
  
  // Get socket index.
  int getSocketIndex() const;
  
  // Draw.
  int draw(void) override;

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

#endif // SWORD_H
