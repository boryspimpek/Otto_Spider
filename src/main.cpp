// main.cpp
#include <Arduino.h>
#include "robot.h"

void setup() {
  pwm.begin();
  pwm.setPWMFreq(50);
  delay(500);

  for (int i = 0; i < 8; i++) {
    osc[i].setOffset(90);
    osc[i].setAmplitude(0);
    osc[i].setPhase(0);
    osc[i].setPeriod(1000);
    osc[i].reset();
    setServo(i, 90);
  }

  delay(1000);
}

void loop() {
  standStill();
  delay(3000);
  walkForward(5, 1000);
  turnL(5, 1000);
  walkForward(5, 1000);
  turnR(5, 1000);
  delay(2000);
  hello();
  delay(2000);
  
}
