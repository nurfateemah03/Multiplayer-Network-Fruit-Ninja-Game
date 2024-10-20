//
// Points.h
// 

#ifndef POINTS_H
#define POINTS_H

// Engine includes.
#include "Color.h"
#include "Event.h"		
#include "ViewObject.h"

#define POINTS_STRING "Points"

class Points : public df::ViewObject {

 public:
  // Constructor.
  Points();

  // Set value.
  void setValue(int value) override;
};

#endif // POINTS_H
