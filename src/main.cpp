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
  transitionToWalkForward(2000);
  walkForward(5, 2000);
  standStill();
  delay(2000);
  turnR(5, 2000);
  delay(2000);
  standStill();
  delay(2000);
  //turnL(5, 2000);
  //standStill();
  //delay(2000);
  hello();
  delay(2000);
  standStill();
  delay(2000);
  //run(2, 2000);
  //delay(2000);
  //standStill();
  //delay(2000);
  dance(5, 600);
  delay(2000);
  standStill();
  delay(2000);
  //frontBack(2, 600);
  //delay(2000);
  //standStill();
  //delay(2000);
  upDown(3, 600);
  delay(2000);
  standStill();
  delay(2000);
  pushUp(3, 600);
  delay(2000);
  standStill();
  delay(2000);
  //jump();
  //delay(2000);
  //standStill();
  //delay(2000);
}
