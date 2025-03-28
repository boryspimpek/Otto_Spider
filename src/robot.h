// robot.h
#pragma once

#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>

#define SERVO_MIN  150
#define SERVO_MAX  600

extern Adafruit_PWMServoDriver pwm;
extern float servoTrim[8];
extern float currentAngle[8];

class Oscillator {
public:
  float amplitude = 0;
  float phase = 0;
  float offset = 0;
  int period = 1000;
  unsigned long refTime = 0;
  bool running = true;

  void reset();
  void setAmplitude(float a);
  void setPhase(float p);
  void setOffset(float o);
  void setPeriod(int p);
  float refresh();
};

extern Oscillator osc[8];

void setServo(uint8_t ch, float angle);
void moveServos(int totalTime, float target[8]);
void configureGait(int T, float x_amp, float z_amp, float ap, float hi, float front_x);
void transitionToWalkForward(int T = 2000);
void walkForward(int steps, int T = 2000);
void turnR(int steps, int T = 2000);
void turnL(int steps, int T = 2000);
void hello();
void moonwalkL(int steps, int T = 5000);
void run(int steps, int T = 5000);
void dance(int steps, int T = 600);
void frontBack(int steps, int T = 600);
void upDown(int steps, int T = 5000);
void pushUp(int steps, int T = 600);
void jump();
void home();
void standStill();
