#pragma once
// Ping.h
#ifndef __PING_H__
#define __PING_H__

#include "ViewObject.h"

class Ping : public df::ViewObject {
private:
    int latency; // Store the current latency value in milliseconds.

public:
    Ping(); // Constructor.
    void setValue(int new_latency); // Set the latency value.
};

#endif // __PING_H__
