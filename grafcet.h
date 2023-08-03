#ifndef GRAFCET_H
#define GRAFCET_H

#include <Arduino.h>
#include "step.h"
#include <vector>

class Grafcet {
public:
    Grafcet(const std::vector<int>& stepNumbers);
    void update(int stepNumber);
    int getActiveStepNumber();

private:
    std::vector<Step> steps;
};

#endif // GRAFCET_H
