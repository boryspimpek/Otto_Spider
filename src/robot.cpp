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
  const int steps = 20;
  int delayTime = totalTime / steps;

  for (int j = 0; j <= steps; j++) {
    for (int i = 0; i < 8; i++) {
      float angle = currentAngle[i] + (target[i] - currentAngle[i]) * j / steps;
      setServo(i, angle);
    }
    delay(delayTime);
  }
}

void playMotion(int steps, int T, int amplitude[8], int offset[8], int phase[8], int period[8]) {
  for (int i = 0; i < 8; i++) {
    osc[i].reset();
    osc[i].setPeriod(period[i]);
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

void stand(int transitionTime) {
  const int steps = 20;
  int delayTime = transitionTime / steps;
  float current[8];

  // Pobierz aktualną pozycję każdego serwa z currentAngle (nie oscylatora!)
  for (int i = 0; i < 8; i++) {
    current[i] = currentAngle[i];
  }

  // Docelowe pozycje: offsety
  float offset[8] = {
    98,  // serwo 0
    82,  // serwo 1
    60,  // serwo 2
    120, // serwo 3
    58,  // serwo 4
    122, // serwo 5
    120, // serwo 6
    60   // serwo 7
  };

  for (int j = 0; j <= steps; j++) {
    for (int i = 0; i < 8; i++) {
      float angle = current[i] + (offset[i] - current[i]) * j / steps;
      setServo(i, angle);
    }
    delay(delayTime);
  }
}

void walkForward(int steps, int T) {
  float x_amp = 15;
  float z_amp = 20;
  float ap = 20;
  float hi = 10;
  float front_x = 12;

  float offset[8] = {90 + ap - front_x, 90 - ap + front_x, 90 - hi, 90 + hi, 90 - ap - front_x, 90 + ap + front_x, 90 + hi, 90 - hi};
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

  // Ruch
  unsigned long start = millis();
  unsigned long duration = (unsigned long)(T * steps);

  while ((millis() - start) < duration) {
    int side = ((millis() - start) / (T / 2)) % 2;

    // Przód-tył nogi (ciągle)
    setServo(0, osc[0].refresh());
    setServo(1, osc[1].refresh());
    setServo(4, osc[4].refresh());
    setServo(5, osc[5].refresh());

    // Góra-dół nogi (na zmianę)
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

  int offset[8] = {90 + ap, 90 - ap, 90 - hi, 90 + hi, 90 - ap, 90 + ap, 90 + hi, 90 - hi};
  int phase[8] = {0, 180, 90, 90, 180, 0, 90, 90};
  int period[8] = {T, T, T, T, T, T, T, T};
  int amplitude[8] = {x_amp, x_amp, z_amp, z_amp, x_amp, x_amp, z_amp, z_amp};

  playMotion(steps, T, amplitude, offset, phase, period);
}

void turnL(int steps, int T) {
  int x_amp = 15;
  int z_amp = 15;
  int ap = 15;
  int hi = 23;

  int offset[8] = {90 + ap, 90 - ap, 90 - hi, 90 + hi, 90 - ap, 90 + ap, 90 + hi, 90 - hi};
  int phase[8] = {180, 0, 90, 90, 0, 180, 90, 90};
  int period[8] = {T, T, T, T, T, T, T, T};
  int amplitude[8] = {x_amp, x_amp, z_amp, z_amp, x_amp, x_amp, z_amp, z_amp};

  playMotion(steps, T, amplitude, offset, phase, period);
}

void hello() {
  float helloPose[8] = {90 + 15, 90 - 15, 90 - 65, 90 + 65, 90 + 20, 90 - 20, 90 + 10, 90 - 10};
  moveServos(500, helloPose);
  delay(200);

  int T = 350;
  int period[8] = {T, T, T, T, T, T, T, T};
  int amplitude[8] = {0, 50, 0, 50, 0, 0, 0, 0};
  int offset[8]    = {90 + 15, 40, 90 - 65, 90, 90 + 20, 90 - 20, 90 + 10, 90 - 10};
  int phase[8]     = {0, 0, 0, 90, 0, 0, 0, 0};

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

  float endPose[8] = {160, 20, 90, 90, 90 - 20, 90 + 20, 90 + 10, 90 - 10};
  moveServos(500, endPose);
  delay(200);
}

void moonwalkL(int steps, int T) {
  int z_amp = 45;

  int period[8] = {T, T, T, T, T, T, T, T};
  int amplitude[8] = {0, 0, z_amp, z_amp, 0, 0, z_amp, z_amp};
  int offset[8] = {90, 90, 90, 90, 90, 90, 90, 90};
  int phase[8] = {0, 0, 0, 120, 0, 0, 180, 290};

  playMotion(steps, T, amplitude, offset, phase, period);
}

void run(int steps, int T) {
  int x_amp = 15;
  int z_amp = 15;
  int ap = 15;
  int hi = 15;
  int front_x = 6;

  int period[8] = {T, T, T, T, T, T, T, T};
  int amplitude[8] = {x_amp, x_amp, z_amp, z_amp, x_amp, x_amp, z_amp, z_amp};
  int offset[8] = {90 + ap - front_x, 90 - ap + front_x, 90 - hi, 90 + hi, 90 - ap - front_x, 90 + ap + front_x, 90 + hi, 90 - hi};
  int phase[8] = {0, 0, 90, 90, 180, 180, 90, 90};

  playMotion(steps, T, amplitude, offset, phase, period);
}

void dance(int steps, int T) {
  int x_amp = 0;
  int z_amp = 40;
  int ap = 30;
  int hi = 20;

  int period[8] = {T, T, T, T, T, T, T, T};
  int amplitude[8] = {x_amp, x_amp, z_amp, z_amp, x_amp, x_amp, z_amp, z_amp};
  int offset[8] = {90 + ap, 90 - ap, 90 - hi, 90 + hi, 90 - ap, 90 + ap, 90 + hi, 90 - hi};
  int phase[8] = {0, 0, 0, 270, 0, 0, 90, 180};

  playMotion(steps, T, amplitude, offset, phase, period);
}

void frontBack(int steps, int T) {
  int x_amp = 30;
  int z_amp = 25;
  int ap = 20;
  int hi = 30;

  int period[8] = {T, T, T, T, T, T, T, T};
  int amplitude[8] = {x_amp, x_amp, z_amp, z_amp, x_amp, x_amp, z_amp, z_amp};
  int offset[8] = {90 + ap, 90 - ap, 90 - hi, 90 + hi, 90 - ap, 90 + ap, 90 + hi, 90 - hi};
  int phase[8] = {0, 180, 270, 90, 0, 180, 90, 270};

  playMotion(steps, T, amplitude, offset, phase, period);
}

void upDown(int steps, int T) {
  int x_amp = 0;
  int z_amp = 35;
  int ap = 20;
  int hi = 25;
  int front_x = 0;

  int period[8] = {T, T, T, T, T, T, T, T};
  int amplitude[8] = {x_amp, x_amp, z_amp, z_amp, x_amp, x_amp, z_amp, z_amp};
  int offset[8] = {90 + ap - front_x, 90 - ap + front_x, 90 - hi, 90 + hi, 90 - ap - front_x, 90 + ap + front_x, 90 + hi, 90 - hi};
  int phase[8] = {0, 0, 90, 270, 180, 180, 270, 90};

  playMotion(steps, T, amplitude, offset, phase, period);
}

void pushUp(int steps, int T) {
  int z_amp = 40;
  int x_amp = 65;
  int hi = 30;

  int period[8] = {T, T, T, T, T, T, T, T};
  int amplitude[8] = {0, 0, z_amp, z_amp, 0, 0, 0, 0};
  int offset[8] = {90, 90, 90 - hi, 90 + hi, 90 - x_amp, 90 + x_amp, 90 + hi, 90 - hi};
  int phase[8] = {0, 0, 0, 180, 0, 0, 0, 180};

  playMotion(steps, T, amplitude, offset, phase, period);
}

void jump() {
  float jumpPose[8] = {90 + 15, 90 - 15, 90 - 65, 90 + 65, 90 + 20, 90 - 20, 90 + 10, 90 - 10};
  float ap = 20;
  float hi = 35;
  float salto[8] = {90 + ap, 90 - ap, 90 - hi, 90 + hi, 90 - ap * 3, 90 + ap * 3, 90 + hi, 90 - hi};

  moveServos(150, jumpPose);
  delay(200);
  moveServos(0, salto); // instant
  delay(100);
  home();
}

void home() {
  float ap = 20;
  float hi = 35;
  float position[8] = {90 + ap, 90 - ap, 90 - hi, 90 + hi, 90 - ap, 90 + ap, 90 + hi, 90 - hi};

  for (int i = 0; i < 8; i++) {
    setServo(i, position[i]);
  }
}
