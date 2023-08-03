#ifndef STEP_H
#define STEP_H

class Step {
public:
  Step(int numero);

  int getNumber();
  bool isActive() const;
  void setActive(bool active);
  bool getRisingEdge() const;
  bool getFallingEdge() const;

private:
  int number;
  bool active;
  bool previousActive;
};

#endif  // STEP_H
