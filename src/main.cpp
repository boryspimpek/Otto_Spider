#include <Arduino.h>
#include <espnow.h>
#include <ESP8266WiFi.h>
#include "robot.h"

typedef struct struct_message {
    bool L1Pressed;
    bool L2Pressed;
    bool L3Pressed;
    bool L4Pressed;
    bool R1Pressed;
    bool R2Pressed;
    bool R3Pressed;
    bool R4Pressed;
    int joy1_x;
    int joy1_y;
    int joy2_x;
    int joy2_y;
} struct_message;

struct_message receivedCommand;  

const int DEADZONE = 20;
const int T = 500;

bool walkActive = false;
bool turnRightActive = false;
bool turnLeftActive = false;

bool wasR1Pressed = false;
bool wasR2Pressed = false;
bool wasR3Pressed = false;
bool wasR4Pressed = false;

bool wasL1Pressed = false;
bool wasL2Pressed = false;
bool wasL3Pressed = false;

extern bool walkInitialized;
extern bool turnRightInitialized;
extern bool turnLeftInitialized;

void onDataReceive(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&receivedCommand, incomingData, sizeof(receivedCommand));

  Serial.println("Otrzymano dane:");
  Serial.printf("L1: %d, L2: %d, L3: %d, L4: %d\n", 
  receivedCommand.L1Pressed, receivedCommand.L2Pressed,
  receivedCommand.L3Pressed, receivedCommand.L4Pressed);

  Serial.printf("R1: %d, R2: %d, R3: %d, R4: %d\n", 
  receivedCommand.R1Pressed, receivedCommand.R2Pressed,
  receivedCommand.R3Pressed, receivedCommand.R4Pressed);

  Serial.printf("JOY1_X: %d, JOY1_Y: %d\n", receivedCommand.joy1_x, receivedCommand.joy1_y);
  Serial.printf("JOY2_X: %d, JOY2_Y: %d\n", receivedCommand.joy2_x, receivedCommand.joy2_y);
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);  // ESP8266 w trybie Station

    if (esp_now_init() != 0) {
        Serial.println("Błąd inicjalizacji ESP-NOW!");
        return;
    }

    esp_now_register_recv_cb(onDataReceive);

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
    stand(100);
    delay(1000);
}

void loop() {
    // --- CHODZENIE DO PRZODU ---
    if (receivedCommand.joy1_y > DEADZONE) {
        walkForwardSteps(1, 500);
        walkActive = true;
    } else if (walkActive) {
        //stand(100);
        walkActive = false;
        walkInitialized = false;
    }

    // --- OBRÓT W PRAWO ---
    if (receivedCommand.joy1_x < -DEADZONE) {
        turnRight(T);
        turnRightActive = true;
    } else if (turnRightActive) {
        stand(100);
        turnRightActive = false;
        turnRightInitialized = false;
    }

    // --- OBRÓT W LEWO ---
    if (receivedCommand.joy1_x > DEADZONE) {
        turnLeft(T);
        turnLeftActive = true;
    } else if (turnLeftActive) {  // <-- tu był błąd
        stand(100);
        turnLeftActive = false;
        turnLeftInitialized = false;
    }

    // --- PRZYCISK R1 ---
    if (receivedCommand.R1Pressed) {
        if (!wasR1Pressed) {
          hello();
          wasR1Pressed = true;
        }
      } else if (wasR1Pressed) {
        stand(100);  
        wasR1Pressed = false;
      }

    // --- PRZYCISK R2 ---
    if (receivedCommand.R2Pressed) {
        if (!wasR2Pressed) {
          moonwalkL(3, 1000);
          wasR2Pressed = true;
        }
    } else if (wasR2Pressed) {
        stand(100);
        wasR2Pressed = false;
    }

    // --- PRZYCISK R3 ---
    if (receivedCommand.R3Pressed) {
        if (!wasR3Pressed) {
          dance(3, 1000);
          wasR3Pressed = true;
        }
    } else if (wasR3Pressed) {
        stand(100);
        wasR3Pressed = false;
    }

    // --- PRZYCISK R4 ---
    if (receivedCommand.R4Pressed) {
        if (!wasR4Pressed) {
          frontBack(3, 1000);
          wasR4Pressed = true;
        }
    } else if (wasR4Pressed) {
        stand(100);
        wasR4Pressed = false;
    }

    // --- PRZYCISK L1 ---
    if (receivedCommand.L1Pressed) {
        if (!wasL1Pressed) {
          upDown(3, 1000);
          wasL1Pressed = true;
        }
    } else if (wasL1Pressed) {
        stand(100);
        wasL1Pressed = false;
    }

    // --- PRZYCISK L2 ---
    if (receivedCommand.L2Pressed) {
        if (!wasL2Pressed) {
          pushUp(3, 1000);
          wasL2Pressed = true;
        }
    } else if (wasL2Pressed) {
        stand(100);
        wasL2Pressed = false;
    }
}
