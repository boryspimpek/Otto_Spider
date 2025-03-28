// robot.cpp
#include "robot.h"

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
float servoTrim[8] = {0, 4, 0, -7, 6, 7, 1, -5};
float currentAngle[8] = {90, 90, 90, 90, 90, 90, 90, 90};
Oscillator osc[8];

void Oscillator::reset() { refTime = millis(); }
void Oscillator::setAmplitude(float a) { amplitude = a; }
void Oscillator::setPhase(float p) { phase = p * PI / 180.0; }
void Oscillator::setOffset(float o) { offset = o; }
void Oscillator::setPeriod(int p) { period = p; }

float Oscillator::refresh() {
  if (!running) return offset;
  unsigned long now = millis();
  unsigned long dt = (now - refTime) % period;
  float rad = dt * 2.0 * PI / period;
  return amplitude * sin(rad + phase) + offset;
}

void setServo(uint8_t ch, float angle) {
  angle += servoTrim[ch];
  angle = constrain(angle, 0, 180);
  int pulse = map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
  pwm.setPWM(ch, 0, pulse);
  currentAngle[ch] = angle;
}

void moveServos(int totalTime, float target[8]) {
  const int steps = 30;
  int delayTime = totalTime / steps;

  for (int j = 0; j <= steps; j++) {
    for (int i = 0; i < 8; i++) {
      float angle = currentAngle[i] + (target[i] - currentAngle[i]) * j / steps;
      setServo(i, angle);
    }
    delay(delayTime);
  }
}

void configureGait(int T, float x_amp, float z_amp, float ap, float hi, float front_x) {
  float offset[8] = {
    90 + ap - front_x, 90 - ap + front_x,
    90 - hi, 90 + hi,
    90 - ap - front_x, 90 + ap + front_x,
    90 + hi, 90 - hi
  };
  float phase[8] = {90, 90, 270, 90, 270, 270, 90, 270};
  int period[8] = {T, T, T/2, T/2, T, T, T/2, T/2};
  float amp[8] = {x_amp, x_amp, z_amp, z_amp, x_amp, x_amp, z_amp, z_amp};

  for (int i = 0; i < 8; i++) {
    osc[i].reset();
    osc[i].setPeriod(period[i]);
    osc[i].setAmplitude(amp[i]);
    osc[i].setPhase(phase[i]);
    osc[i].setOffset(offset[i]);
  }
}

void transitionToWalkForward(int T) {
  configureGait(T, 15, 20, 20, 10, 12);
  float target[8];
  for (int i = 0; i < 8; i++) target[i] = osc[i].refresh();

  const int steps = 30;
  const int delayTime = 15;
  for (int j = 0; j <= steps; j++) {
    for (int i = 0; i < 8; i++) {
      float angle = currentAngle[i] + (target[i] - currentAngle[i]) * j / steps;
      setServo(i, angle);
    }
    delay(delayTime);
  }
}

void walkForward(int steps, int T) {
  configureGait(T, 15, 20, 20, 10, 12);
  unsigned long start = millis();
  unsigned long duration = (unsigned long)(T * steps);

  while ((millis() - start) < duration) {
    int side = ((millis() - start) / (T / 2)) % 2;
    setServo(0, osc[0].refresh());
    setServo(1, osc[1].refresh());
    setServo(4, osc[4].refresh());
    setServo(5, osc[5].refresh());
    if (side == 0) {
      setServo(3, osc[3].refresh());
      setServo(6, osc[6].refresh());
    } else {
      setServo(2, osc[2].refresh());
      setServo(7, osc[7].refresh());
    }
    delay(1);
  }
}

void turnR(int steps, int T) {
  int x_amp = 15;
  int z_amp = 15;
  int ap = 15;
  int hi = 23;

  float offset[8] = {
    90.0f + ap, 90.0f - ap, 90.0f - hi, 90.0f + hi,
    90.0f - ap, 90.0f + ap, 90.0f + hi, 90.0f - hi
  
  };
  float phase[8] = {0.0f, 180.0f, 90.0f, 90.0f, 180.0f, 0.0f, 90.0f, 90.0f};
  int period[8] = {T, T, T, T, T, T, T, T};
  float amp[8] = {(float)x_amp, (float)x_amp, (float)z_amp, (float)z_amp,
    (float)x_amp, (float)x_amp, (float)z_amp, (float)z_amp};

  for (int i = 0; i < 8; i++) {
    osc[i].reset();
    osc[i].setPeriod(period[i]);
    osc[i].setAmplitude(amp[i]);
    osc[i].setPhase(phase[i]);
    osc[i].setOffset(offset[i]);
  }

  unsigned long start = millis();
  unsigned long duration = (unsigned long)(T * steps);

  while ((millis() - start) < duration) {
    for (int i = 0; i < 8; i++) {
      setServo(i, osc[i].refresh());
    }
    delay(1);
  }
}

void turnL(int steps, int T) {
  int x_amp = 15;
  int z_amp = 15;
  int ap = 15;
  int hi = 23;

  float offset[8] = {
    90.0f + ap, 90.0f - ap, 90.0f - hi, 90.0f + hi,
    90.0f - ap, 90.0f + ap, 90.0f + hi, 90.0f - hi
  };

  float phase[8] = {180.0f, 0.0f, 90.0f, 90.0f, 0.0f, 180.0f, 90.0f, 90.0f};
  int period[8] = {T, T, T, T, T, T, T, T};
  float amp[8] = {
    (float)x_amp, (float)x_amp, (float)z_amp, (float)z_amp,
    (float)x_amp, (float)x_amp, (float)z_amp, (float)z_amp
  };

  for (int i = 0; i < 8; i++) {
    osc[i].reset();
    osc[i].setPeriod(period[i]);
    osc[i].setAmplitude(amp[i]);
    osc[i].setPhase(phase[i]);
    osc[i].setOffset(offset[i]);
  }

  unsigned long start = millis();
  unsigned long duration = (unsigned long)(T * steps);

  while ((millis() - start) < duration) {
    for (int i = 0; i < 8; i++) {
      setServo(i, osc[i].refresh());
    }
    delay(1);
  }
}

void hello() {
  float helloPose[8] = {90.0f + 15, 90.0f - 15, 90.0f - 65, 90.0f + 65, 90.0f + 20, 90.0f - 20, 90.0f + 10, 90.0f - 10};
  moveServos(500, helloPose);
  delay(200);

  int T = 350;
  int period[8] = {T, T, T, T, T, T, T, T};

  float amplitude[8] = {0.0f, 50.0f, 0.0f, 50.0f, 0.0f, 0.0f, 0.0f, 0.0f};
  float offset[8]    = {90.0f + 15, 40.0f, 90.0f - 65, 90.0f, 90.0f + 20, 90.0f - 20, 90.0f + 10, 90.0f - 10};
  float phase[8]     = {0.0f, 0.0f, 0.0f, 90.0f, 0.0f, 0.0f, 0.0f, 0.0f};

  for (int i = 0; i < 8; i++) {
    osc[i].reset();
    osc[i].setPeriod((int)period[i]);
    osc[i].setAmplitude(amplitude[i]);
    osc[i].setPhase(phase[i]);
    osc[i].setOffset(offset[i]);
  }

  unsigned long start = millis();
  unsigned long duration = T * 4;

  while ((millis() - start) < duration) {
    for (int i = 0; i < 8; i++) {
      setServo(i, osc[i].refresh());
    }
    delay(1);
  }

  float endPose[8] = {160.0f, 20.0f, 90.0f, 90.0f, 90.0f - 20, 90.0f + 20, 90.0f + 10, 90.0f - 10};
  moveServos(500, endPose);
  delay(200);
}

void moonwalkL(int steps, int T) {
  int z_amp = 45;

  int period[8] = {T, T, T, T, T, T, T, T};
  int amplitude[8] = {0, 0, z_amp, z_amp, 0, 0, z_amp, z_amp};
  float offset[8] = {90.0f, 90.0f, 90.0f, 90.0f, 90.0f, 90.0f, 90.0f, 90.0f};
  float phase[8] = {0.0f, 0.0f, 0.0f, 120.0f, 0.0f, 0.0f, 180.0f, 290.0f};

  for (int i = 0; i < 8; i++) {
    osc[i].reset();
    osc[i].setPeriod((int)period[i]);
    osc[i].setAmplitude(amplitude[i]);
    osc[i].setOffset(offset[i]);
    osc[i].setPhase(phase[i]);
  }

  unsigned long start = millis();
  unsigned long duration = (unsigned long)(T * steps);

  while ((millis() - start) < duration) {
    for (int i = 0; i < 8; i++) {
      setServo(i, osc[i].refresh());
    }
    delay(1);
  }
}

void run(int steps, int T) {
  int x_amp = 15;
  int z_amp = 15;
  int ap = 15;
  int hi = 15;
  int front_x = 6;

  float period[8];
  for (int i = 0; i < 8; i++) period[i] = (float)T;

  float amplitude[8] = {
    (float)x_amp, (float)x_amp, (float)z_amp, (float)z_amp,
    (float)x_amp, (float)x_amp, (float)z_amp, (float)z_amp
  };

  float offset[8] = {
    90.0f + ap - front_x,
    90.0f - ap + front_x,
    90.0f - hi,
    90.0f + hi,
    90.0f - ap - front_x,
    90.0f + ap + front_x,
    90.0f + hi,
    90.0f - hi
  };

  float phase[8] = {0.0f, 0.0f, 90.0f, 90.0f, 180.0f, 180.0f, 90.0f, 90.0f};

  for (int i = 0; i < 8; i++) {
    osc[i].reset();
    osc[i].setPeriod((int)period[i]);
    osc[i].setAmplitude(amplitude[i]);
    osc[i].setOffset(offset[i]);
    osc[i].setPhase(phase[i]);
  }

  unsigned long start = millis();
  unsigned long duration = (unsigned long)(T * steps);

  while ((millis() - start) < duration) {
    for (int i = 0; i < 8; i++) {
      setServo(i, osc[i].refresh());
    }
    delay(1);
  }
}

void dance(int steps, int T) {
  int x_amp = 0;
  int z_amp = 40;
  int ap = 30;
  int hi = 20;

  float period[8];
  for (int i = 0; i < 8; i++) period[i] = (float)T;

  float amplitude[8] = {
    (float)x_amp, (float)x_amp, (float)z_amp, (float)z_amp,
    (float)x_amp, (float)x_amp, (float)z_amp, (float)z_amp
  };

  float offset[8] = {
    90.0f + ap, 90.0f - ap,
    90.0f - hi, 90.0f + hi,
    90.0f - ap, 90.0f + ap,
    90.0f + hi, 90.0f - hi
  };

  float phase[8] = {0.0f, 0.0f, 0.0f, 270.0f, 0.0f, 0.0f, 90.0f, 180.0f};

  for (int i = 0; i < 8; i++) {
    osc[i].reset();
    osc[i].setPeriod((int)period[i]);
    osc[i].setAmplitude(amplitude[i]);
    osc[i].setOffset(offset[i]);
    osc[i].setPhase(phase[i]);
  }

  unsigned long start = millis();
  unsigned long duration = (unsigned long)(T * steps);

  while ((millis() - start) < duration) {
    for (int i = 0; i < 8; i++) {
      setServo(i, osc[i].refresh());
    }
    delay(1);
  }
}

void frontBack(int steps, int T) {
  int x_amp = 30;
  int z_amp = 25;
  int ap = 20;
  int hi = 30;

  float period[8]; 
  for (int i = 0; i < 8; i++) period[i] = (float)T;

  float amplitude[8] = {
    (float)x_amp, (float)x_amp,
    (float)z_amp, (float)z_amp,
    (float)x_amp, (float)x_amp,
    (float)z_amp, (float)z_amp
  };

  float offset[8] = {
    90.0f + ap, 90.0f - ap,
    90.0f - hi, 90.0f + hi,
    90.0f - ap, 90.0f + ap,
    90.0f + hi, 90.0f - hi
  };

  float phase[8] = {
    0.0f, 180.0f, 270.0f, 90.0f,
    0.0f, 180.0f, 90.0f, 270.0f
  };

  for (int i = 0; i < 8; i++) {
    osc[i].reset();
    osc[i].setPeriod((int)period[i]);
    osc[i].setAmplitude(amplitude[i]);
    osc[i].setOffset(offset[i]);
    osc[i].setPhase(phase[i]);
  }

  unsigned long start = millis();
  unsigned long duration = (unsigned long)(T * steps);

  while ((millis() - start) < duration) {
    for (int i = 0; i < 8; i++) {
      setServo(i, osc[i].refresh());
    }
    delay(1);
  }
}

void upDown(int steps, int T) {
  int x_amp = 0;
  int z_amp = 35;
  int ap = 20;
  int hi = 25;
  int front_x = 0;

  float period[8];
  for (int i = 0; i < 8; i++) period[i] = (float)T;

  float amplitude[8] = {
    (float)x_amp, (float)x_amp,
    (float)z_amp, (float)z_amp,
    (float)x_amp, (float)x_amp,
    (float)z_amp, (float)z_amp
  };

  float offset[8] = {
    90.0f + ap - front_x,
    90.0f - ap + front_x,
    90.0f - hi,
    90.0f + hi,
    90.0f - ap - front_x,
    90.0f + ap + front_x,
    90.0f + hi,
    90.0f - hi
  };

  float phase[8] = {
    0.0f, 0.0f, 90.0f, 270.0f,
    180.0f, 180.0f, 270.0f, 90.0f
  };

  for (int i = 0; i < 8; i++) {
    osc[i].reset();
    osc[i].setPeriod((int)period[i]);
    osc[i].setAmplitude(amplitude[i]);
    osc[i].setOffset(offset[i]);
    osc[i].setPhase(phase[i]);
  }

  unsigned long start = millis();
  unsigned long duration = (unsigned long)(T * steps);

  while ((millis() - start) < duration) {
    for (int i = 0; i < 8; i++) {
      setServo(i, osc[i].refresh());
    }
    delay(1);
  }
}

void pushUp(int steps, int T) {
  int z_amp = 40;
  int x_amp = 65;
  int hi = 30;

  float period[8];
  for (int i = 0; i < 8; i++) period[i] = (float)T;

  float amplitude[8] = {0.0f, 0.0f, (float)z_amp, (float)z_amp, 0.0f, 0.0f, 0.0f, 0.0f};

  float offset[8] = {90.0f, 90.0f, 90.0f - hi, 90.0f + hi, 90.0f - x_amp, 90.0f + x_amp, 90.0f + hi, 90.0f - hi};

  float phase[8] = {0.0f, 0.0f, 0.0f, 180.0f, 0.0f, 0.0f, 0.0f, 180.0f};

  for (int i = 0; i < 8; i++) {
    osc[i].reset();
    osc[i].setPeriod((int)period[i]);
    osc[i].setAmplitude(amplitude[i]);
    osc[i].setOffset(offset[i]);
    osc[i].setPhase(phase[i]);
  }

  unsigned long start = millis();
  unsigned long duration = (unsigned long)(T * steps);

  while ((millis() - start) < duration) {
    for (int i = 0; i < 8; i++) {
      setServo(i, osc[i].refresh());
    }
    delay(1);
  }
}

void jump() {
  float jumpPose[8] = {
    90.0f + 15, 90.0f - 15,
    90.0f - 65, 90.0f + 65,
    90.0f + 20, 90.0f - 20,
    90.0f + 10, 90.0f - 10
  };

  float ap = 20.0f;
  float hi = 35.0f;

  float salto[8] = {
    90.0f + ap, 90.0f - ap,
    90.0f - hi, 90.0f + hi,
    90.0f - ap * 3, 90.0f + ap * 3,
    90.0f + hi, 90.0f - hi
  };

  moveServos(150, jumpPose);
  delay(200);
  moveServos(0, salto); // instant
  delay(100);
  home();
}

void home() {
  float ap = 20.0f;
  float hi = 35.0f;
  float position[8] = {
    90.0f + ap, 90.0f - ap,
    90.0f - hi, 90.0f + hi,
    90.0f - ap, 90.0f + ap,
    90.0f + hi, 90.0f - hi
  };

  for (int i = 0; i < 8; i++) {
    setServo(i, position[i]);
  }
}

void standStill() {
  const int steps = 20;
  const int delayTime = 15;
  float current[8];
  for (int i = 0; i < 8; i++) current[i] = osc[i].refresh();

  for (int j = 0; j <= steps; j++) {
    for (int i = 0; i < 8; i++) {
      float angle = current[i] + (90 - current[i]) * j / steps;
      setServo(i, angle);
    }
    delay(delayTime);
  }
}
