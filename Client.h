//
// Client.h
//

#ifndef CLIENT_H
#define CLIENT_H

// Engine includes.
#include "EventKeyboard.h"
#include "EventMouse.h"
#include "EventNetwork.h"
#include "EventNetworkCustom.h"
#include "NetworkNode.h"
#include "Ping.h"
#include "EventStep.h"

const std::string CLIENT_STRING = "Client";

class Client : public df::NetworkNode {

 public:
  Client();
  int player_index;
  int m_latency;
  // Handle event.
  int eventHandler(const df::Event *p_e) override;

  // Handle close event.
  int handleClose(const df::EventNetwork *p_en) override;

  // Create Object of given type.
  // Return pointer to Object.
  df::Object *createObject(std::string obj_type) override; 

 private:
  // Handle mouse event.
  int mouse(const df::EventMouse *p_e);
  
  // Handle keyboard event.
  int keyboard(const df::EventKeyboard *p_e);

  // Handle custom network event.
  int net(const df::EventNetworkCustom *p_en);
  Ping* ping_display;
  void sendPing();
  int handleStep(const df::EventStep* p_es);



};

#endif
