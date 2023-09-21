#include "Grafcet.h"
#include "helpers.h"
#include "Step.h"

Grafcet::Grafcet(const std::vector<int>& stepNumbers) {
  for (int number : stepNumbers) {
    steps.push_back(Step(number));
  }
}

void Grafcet::update(int stepNumber) {
  for (Step& step : steps) {
    if (step.getNumber() == stepNumber) {
      logInfo("Active step : " + String(stepNumber));
    }
    step.setActive(step.getNumber() == stepNumber);
  }
}

int Grafcet::getActiveStepNumber() {
  for (Step& step : steps) {
    if (step.isActive()) {
      return step.getNumber();
    }
  }
  return -1;  // No active step
}
