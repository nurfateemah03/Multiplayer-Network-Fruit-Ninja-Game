//
// Server.cpp
//

// System includes.
#include <string.h> // for memcpy()

// Engine includes.
#include "EventNetwork.h"
#include "EventStep.h"
#include "EventNetworkCustom.h"
#include "GameManager.h"
#include "LogManager.h"
#include "NetworkManager.h"
#include "WorldManager.h"
#include "NetworkNode.h"

// Game includes.
#include "Fruit.h"
#include "GameOver.h"
#include "Grocer.h"
#include "Kudos.h"
#include "Server.h"
#include "Sword.h"
#include "Timer.h"
#include "util.h"

Server::Server() {

    // Initialize member attributes.
    for (int i = 0; i < MAX_PLAYERS; i++) {
        p_sword[i] = NULL;
        p_points[i] = NULL;
    }

    // Set as network server.
    setType(SERVER_STRING);
    if (NM.setServer(true) != 0) {
        LM.writeLog("Server::Server(): Failed to set server mode.");
        exit(-1);
    }
    NM.setMaxConnections(MAX_PLAYERS);

    // Register for step events for sync.
    registerInterest(df::STEP_EVENT);

    LM.writeLog("Server::Server(): Server started.  Players needed: %d",
        MAX_PLAYERS);
}

// Handle event.
// Return 0 if ignored, else 1.
int Server::eventHandler(const df::Event* p_e) {

    // Step event.
    if (p_e->getType() == df::STEP_EVENT && NM.isConnected())
        return handleStep((const df::EventStep*)p_e);



    // Handle custom network event.
    if (p_e->getType() == df::NETWORK_CUSTOM_EVENT) {
        LM.writeLog("RECEVIED CUSTOM EVENT");

        const df::EventNetworkCustom* p_custom = dynamic_cast<const df::EventNetworkCustom*>(p_e);

        // Ensure p_custom is valid.
        if (p_custom) {
            const void* msg = p_custom->getMessage();
            int client_time;
            memcpy(&client_time, msg, sizeof(client_time));

            // Prepend "PING" to the message.
            std::string ping_msg = "PING " + std::to_string(client_time);

            // Send the ping message back to the client.
            if (sendMessage(df::MessageType::CUSTOM_MESSAGE, ping_msg.size() + 1, ping_msg.c_str(), p_custom->getSocketIndex()) == -1) {
                LM.writeLog("Server: Error echoing PING message.");
            }
            return 1; // Event handled.
        }
    }


    // Call parent event handler.
    return NetworkNode::eventHandler(p_e);

} // End of eventHandler().

int Server::handleAccept(const df::EventNetwork* p_en) {
    LM.writeLog("Server::handleAccept(): server now connected socket: %d",
        p_en->getSocketIndex());
    int sock_index = p_en->getSocketIndex();  // Store the socket index from the event.

    const int DELAY = 5;  // 3 ticks = ~99 ms
   NM.setDelay(DELAY, sock_index);  // Now use the declared sock_index.


    if (NM.isConnected(sock_index)) {  // Ensure client is connected
        std::string player_index_msg = "PLAYER_INDEX " + std::to_string(sock_index);  // Add the prefix
        LM.writeLog("Server: Sending player index message: %s", player_index_msg.c_str());

        int result = sendMessage(df::MessageType::CUSTOM_MESSAGE, player_index_msg.size() + 1, player_index_msg.c_str(), sock_index);
        if (result == -1) {
            LM.writeLog("Server: Error sending player index to client. Socket index: %d, Message: %s", sock_index, player_index_msg.c_str());
            exit(-1);
        }
        else {
            LM.writeLog("Server: Player index successfully sent to client at socket index %d", sock_index);
        }
    }
    else {
        LM.writeLog("Server: Client at socket index %d is not connected.", sock_index);
    }


    // If enough players connected or Grocer started, nothing else to do.
    if (NM.getNumConnections() < MAX_PLAYERS ||
        WM.objectsOfType("Grocer").getCount() == 1)
        return 1;

    // Otherwise, start game.

    // Create Swords.
    for (int i = 0; i < NM.getNumConnections(); i++) {
        Sword* p_s = new Sword();
        if (!p_s) {
            LM.writeLog("Server::handleAccept(): Error!  Cannot allocate Sword.");
            exit(-1);
        }
        p_s->setColor(sockToColor(i));
        p_s->setId(SWORD_ID + i);
        p_s->setSocketIndex(i);
        p_sword[i] = p_s;
        LM.writeLog(1, "Server::handleAccept(): Sword %d created.", i);
    }

    // Create Points.
    for (int i = 0; i < NM.getNumConnections(); i++) {
        Points* p_p = new Points();
        if (!p_p) {
            LM.writeLog("Server::handleAccept(): Error!  Cannot allocate Points.");
            exit(-1);
        }
        p_p->setLocation(sockToLocation(i));
        std::string s = "Player ";
        s += df::toString(i);
        s += ":";
        p_p->setViewString(s);
        p_points[i] = p_p;
        LM.writeLog(1, "Server::handleAccept(): Points %d created.", i);
    }

    // Grocer to start spawning Fruit.
    new Grocer();

    // Timer for time progress.
    new Timer();

    LM.writeLog(1, "Server::handleAccept(): Game has begun.");

    return 1;
}

// If any Objects need to be synchronized, send to Clients.
int Server::handleStep(const df::EventStep* p_es) {

    // Iterate through all Objects.
    df::ObjectList ol = WM.getAllObjects();
    for (int i = 0; i < ol.getCount(); i++) {
        Object* p_o = ol[i];

        // (See project description writeup for what & when to sync.)
        int sock_index = -2; // -2 means don't sync
        if (p_o->getType() == SWORD_STRING)
            if (p_o->isModified(df::ObjectAttribute::POSITION))
                sock_index = -1; // sync with all
        if ((dynamic_cast <Fruit*> (p_o)))
            if (p_o->isModified(df::ObjectAttribute::ID))
                sock_index = -1; // sync with all
        if ((dynamic_cast <Timer*> (p_o)))
            if (p_o->isModified(df::ObjectAttribute::ID))
                sock_index = -1; // sync with all
        if ((dynamic_cast <Points*> (p_o))) {
            unsigned int mask = (unsigned int)df::ObjectAttribute::ID |
                (unsigned int)df::ViewObjectAttribute::VALUE;
            if (p_o->isModified((df::ObjectAttribute)mask))
                sock_index = -1; // sync with all
        }
        if ((dynamic_cast <Kudos*> (p_o)))
            if (p_o->isModified(df::ObjectAttribute::ID)) {
                Kudos* p_k = dynamic_cast <Kudos*> (p_o);
                sock_index = p_k->getSocketIndex(); // sync with only that client
            }

        // If needed, send to client(s).
        if (sock_index != -2) {
            LM.writeLog(1, "Server::handleStep(): SYNC %s (id %d), sock_index %d",
                p_o->getType().c_str(),
                p_o->getId(),
                sock_index);
            if (sendMessage(df::MessageType::SYNC_OBJECT, p_o, sock_index) == -1) {
                LM.writeLog("Server::handleStep(): Error! after sendMessage().");
                exit(-1);
            }
        }

    } // End of iterate through all Objects.

    return 1;
}

// Handle close event.
int Server::handleClose(const df::EventNetwork* p_en) {

    int sock_index = p_en->getSocketIndex();
    LM.writeLog(1, "Server::handleClose(): socket %d", sock_index);

    // If any client leaves, shut down.
    GM.setGameOver();

    return 1;
}