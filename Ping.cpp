// Ping.cpp
#include "Ping.h"
#include "GameManager.h"
#include <string>

// Constructor.
Ping::Ping() {
    setLocation(df::TOP_RIGHT); // Position the ping display in the upper right corner.
    setViewString("Ping: 0 ms"); // Initial display.
    setColor(df::Color::GREEN); // Initial color.
    latency = 0; // Initial latency value.
}

// Set the latency value and update display.
void Ping::setValue(int new_latency) {
    latency = new_latency; // Update latency.

    // Update the displayed string.
    setViewString("Ping: " + std::to_string(latency) + " ms");

    // Change color based on latency.
    if (latency < 100) {
        setColor(df::Color::GREEN);
    }
    else if (latency < 300) {
        setColor(df::Color::YELLOW);
    }
    else {
        setColor(df::Color::RED);
    }

    // Call parent to ensure display is updated.
    ViewObject::setValue(latency);
}
