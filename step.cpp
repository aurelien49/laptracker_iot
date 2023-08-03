#include "Step.h"

Step::Step(int number)
  : number(number), active(false), previousActive(false) {}

bool Step::isActive() const {
  return active;
}

int Step::getNumber() {
  return number;
}

bool Step::getRisingEdge() const {
  return active && !previousActive;
}

bool Step::getFallingEdge() const {
  return !active && previousActive;
}

void Step::setActive(bool active) {
  this->previousActive = this->active;
  this->active = active;
}
