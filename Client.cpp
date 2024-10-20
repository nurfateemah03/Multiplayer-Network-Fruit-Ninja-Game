// Client.cpp

// System includes.
#include <string.h> // for memcpy()

// Engine includes.
#include "DisplayManager.h"
#include "EventKeyboard.h"
#include "EventMouse.h"
#include "EventNetworkCustom.h"
#include "EventStep.h"
#include "GameManager.h"
#include "LogManager.h"
#include "NetworkManager.h"
#include "WorldManager.h"
#include "NetworkNode.h"
#include "EventNetworkCreate.h"

// Game includes.
#include "Client.h"
#include "Fruit.h"
#include "GameOver.h"
#include "Kudos.h"
#include "Points.h"
#include "ServerEntry.h"
#include "Sword.h"
#include "Timer.h"
#include "util.h"
#include "Ping.h"

#define PREDICT_SWORD

Client::Client() {
    ping_display = new Ping();
    // Set as network client.
    setType(CLIENT_STRING);
    NM.setServer(false);

    // ServerEntry spawns others upon callback.
    if (new ServerEntry() == NULL) {
        LM.writeLog("Client::Client(): Error!  Cannot allocate ServerEntry.");
        exit(-1);
    }

    // Client handles mouse, sending it to Server.
    registerInterest(df::MSE_EVENT);
    registerInterest(df::NETWORK_CREATE_EVENT);


    // Keyboard for Q to quit.
    registerInterest(df::KEYBOARD_EVENT);
    registerInterest(df::STEP_EVENT);


    LM.writeLog("Client::Client(): Client started.");
}

// Handle event.
int Client::eventHandler(const df::Event* p_e) {

    // Keyboard event.
    if (p_e->getType() == df::KEYBOARD_EVENT)
        return keyboard((df::EventKeyboard*)p_e);

    // Mouse event.
    if (p_e->getType() == df::MSE_EVENT)
        return mouse((df::EventMouse*)p_e);

    if (p_e->getType() == df::STEP_EVENT) {
        LM.writeLog("got step event ");
        return handleStep(static_cast<const df::EventStep*>(p_e)); // Ensure this calls the right method
    }
    // Handle network creation event.
    if (p_e->getType() == df::NETWORK_CREATE_EVENT) {
        // Cast the event to your specific event type
        const df::EventNetworkCreate* p_create_event = dynamic_cast<const df::EventNetworkCreate*>(p_e);
        if (p_create_event) {
            Object* p_o = p_create_event->getObject(); // Get the object from the event

            if (p_o) { // Ensure p_o is not null
                // Assume you have access to m_latency and the fruit's velocity
                df::Vector velocity = p_o->getVelocity(); // Get the fruit's velocity

                // Adjust position for each tick of latency
                for (int i = 0; i < m_latency; i++) {
                    df::Vector new_pos = p_o->predictPosition(); // Predict position based on velocity
                    WM.moveObject(p_o, new_pos); // Move the object to the predicted position
                }
            }
        }
        // Return 1 to indicate the event was handled
        return 1;
    }
 
    // Network event - custom "game over".
    if (p_e->getType() == df::NETWORK_CUSTOM_EVENT)
        return net((df::EventNetworkCustom*)p_e);

    // Call parent.
    return NetworkNode::eventHandler(p_e);
}
// Handle mouse event.
int Client::mouse(const df::EventMouse* p_e) {

    // If not connected, nothing to do.
    if (NM.isConnected() == false)
        return 0;

    // Check if mouse outside game window.
    sf::RenderWindow* p_win = DM.getWindow();
    sf::Vector2i pos = sf::Mouse::getPosition(*p_win);
    if (pos.x > df::Config::getInstance().getWindowHorizontalPixels() ||
        pos.x < 0 ||
        pos.y > df::Config::getInstance().getWindowVerticalPixels() ||
        pos.y < 0) {
        return 0; // Do nothing.
    }

    // Move the client's own sword immediately.
    Sword* p_sword = dynamic_cast<Sword*>(WM.objectWithId(SWORD_ID + player_index));  // Ensure this is the correct sword.

    if (p_sword == nullptr) {
        LM.writeLog("Client: Error! Sword object for player %d not found.", player_index);  // Add logging if sword is not found.
        return 0;
    }
    LM.writeLog("Client: Sword object ID for player %d is %d", player_index, SWORD_ID + player_index);


#ifdef PREDICT_SWORD
   

  //  if (p_sword && p_sword->getPlayerIndex() == player_index) {
        p_sword->setPosition(p_e->getMousePosition());  // Update the position directly.
        LM.writeLog("Client: Sword position updated to (%f, %f)", p_e->getMousePosition().getX(), p_e->getMousePosition().getY());  // Log updated position.
    //}
    //else {
      //  LM.writeLog("not moved sorry");
           
    //}

#endif

    // If "move", send mouse position to server.
    if (p_e->getMouseAction() == df::MOVED) {
        int ret = sendMessage(df::MessageType::MOUSE_INPUT, df::MOVED,
            df::Mouse::UNDEFINED_MOUSE_BUTTON,
            p_e->getMousePosition());
        LM.writeLog(1, "Client::mouse(): Send mouse %s",
            p_e->getMousePosition().toString().c_str());
        return 1; // Handled.
    }

    // If get here, not handled.
    return 0;
}


// Handle keyboard event.
int Client::keyboard(const df::EventKeyboard* p_e) {

    // Only handle Q pressed.
    if (p_e->getKeyboardAction() == df::KEY_PRESSED &&
        p_e->getKey() == df::Keyboard::Q) {
        GM.setGameOver();
        return 1; // Handled.
    }

    // Not handled.
    return 0;
}

// Create Object of given type.
// Return pointer to Object (NULL if creation failed).
df::Object* Client::createObject(std::string obj_type) {

    LM.writeLog(1, "Client::createObject(): Creating %s",
        obj_type.c_str());

    df::Object* p_o = NULL;

    if (obj_type == SWORD_STRING) {
        LM.writeLog(1, "Client::createObject(): Creating Sword");
        p_o = (df::Object*) new Sword();
    }
    else if (obj_type == "pear" ||
        obj_type == "grapes" ||
        obj_type == "apple" ||
        obj_type == "banana" ||
        obj_type == "blueberries") {
        LM.writeLog(1, "Client::createObject(): Creating %s", obj_type.c_str());
        p_o = (df::Object*) new Fruit(obj_type);
    }
    else if (obj_type == POINTS_STRING) {
        LM.writeLog(1, "Client::createObject(): Creating Points");
        p_o = (df::Object*) new Points();
    }
    else if (obj_type == TIMER_STRING) {
        LM.writeLog(1, "Client::createObject(): Creating Timer");
        p_o = (df::Object*) new Timer();
    }
    else if (obj_type == KUDOS_STRING) {
        LM.writeLog(1, "Client::createObject(): Creating Kudos");
        p_o = (df::Object*) new Kudos();
    }
    else {
        LM.writeLog(1, "Client::createObject(): Unknown type: %s",
            obj_type.c_str());
    }

    return p_o;
}

// Handle custom network event.
int Client::net(const df::EventNetworkCustom* p_en) {
    LM.writeLog("In event network custom");

    // Convert the message to a string.
    std::string msg = static_cast<const char*>(p_en->getMessage());

    // Check if the message is "game over".
    if (msg == "game over") {
        new GameOver();  // Trigger game over event.
        return 1;  // Message handled.
    }

    // Check if message is the player index.
    if (msg.substr(0, 12) == "PLAYER_INDEX") {
        int player_index = std::stoi(msg.substr(13));  // Extract the player index.
        this->player_index = player_index;

        LM.writeLog("Client: Received player index %d", player_index);

        // Store the player index to differentiate this client's sword.
        return 1;  // Message handled.
    }

    // Check if the message starts with "PING".
    if (msg.substr(0, 4) == "PING") {
        LM.writeLog(1, "Received PING", msg.c_str());

        try {
            // Extract the time sent from the message (after "PING").
            int sent_time = std::stoi(msg.substr(5));
            int current_time = GM.getStepCount(); // Get current game tick.
            int latency_ticks = current_time - sent_time;  // Calculate round-trip time.
            int latency_milliseconds = latency_ticks * GM.getFrameTime();  // Convert ticks to ms.

            // Update the ping display with the calculated latency.
            if (ping_display) {
                ping_display->setValue(latency_milliseconds);
            }

            return 1;  // Message handled.
        }
        catch (const std::exception& e) {
            LM.writeLog(1, "Client::net(): Error! Invalid PING message format: '%s'", msg.c_str());
            return -1;  // Return error for invalid message format.
        }
    }

    // Log an error for unexpected messages and exit.
    LM.writeLog(1, "Client::net(): Error! Unexpected message '%s'", msg.c_str());
    exit(-1);  // Exit program due to unexpected message.

    return 0;  // Message not handled.
}


int Client::handleClose(const df::EventNetwork* p_en) {
    LM.writeLog(1, "Client::handleClose():");
    GM.setGameOver();
    return 1;
}

void Client::sendPing() {
    int current_time = GM.getStepCount();
    LM.writeLog("Attempting to send ping!");

    if (sendMessage(df::MessageType::CUSTOM_MESSAGE, sizeof(int), &current_time) == -1) {
        LM.writeLog("Client::sendPing(): Error sending PING message.");
    }
}

int Client::handleStep(const df::EventStep* p_es) {
    LM.writeLog("In handlestep");

    static int ping_counter = 0;

    // Increment ping counter.
    ping_counter++;

    // Send ping every 15 ticks.
    if (ping_counter >= 15) {
        sendPing();
        ping_counter = 0; // Reset counter.
    }

    return 1; // Handled.
}
