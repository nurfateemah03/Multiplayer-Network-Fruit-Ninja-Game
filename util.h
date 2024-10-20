//
// util.h
//

#ifndef UTIL_H
#define UTIL_H

// Engine includes.
#include "Color.h"

const float VERSION = 1.0;

const int MAX_PLAYERS = 2; 

const df::Color COLOR = df::WHITE;
const int MAX_WIDTH = 24; // In characters.

// Fruit settings.
const int NUM_FRUITS = 5;
const std::string FRUIT[NUM_FRUITS] = {
  "pear",
  "grapes",
  "apple",
  "banana",
  "blueberries"
};
const int EXPLOSION_AGE = 45;        // in ticks
const float EXPLOSION_SPEED = 0.05f; // in spaces/tick
const float EXPLOSION_ROTATE = 1.0f; // in degrees

const int SWORD_ID = 100; // base sword id for sync

// Sound settings.
const int NUM_SPLATS = 6;
const int NUM_SWIPES = 7;
const int NUM_KUDOS = 10;

// Wave settings.
const int NUM_WAVES = NUM_FRUITS;
const int WAVE_LEN = 300;      // in ticks
const int WAVE_SPAWN = 30;     // in ticks
const float WAVE_SPEED = 0.5f; // in ticks
const float SPEED_INC = 0.1f;  // in spaces/tick
const int SPAWN_INC = -5 ;     // in ticks

// Map socket index to location.
df::ViewObjectLocation sockToLocation(int sock_index);

// Map socket index to color.
df::Color sockToColor(int sock_index);

////////////////////////////////////////////////////
// Fruit Ninja game functions.

void loadResources(void);
void splash(void);
void create_trail(df::Vector p1, df::Vector p2, df::Color color);
void play_sound(std::string sound);

#endif // UTIL_H
