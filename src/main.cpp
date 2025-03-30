#include <Arduino.h>
#include <espnow.h>
#include <ESP8266WiFi.h>
#include "robot.h"

// Definicja struktury dla wiadomości wysyłanej przez kontroler
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

// Zmienna globalna przechowująca odebrane dane
struct_message receivedCommand;  // Struktura do przechowywania danych z kontrolera

// Funkcja do obsługi odbioru wiadomości ESP-NOW
void onDataReceive(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&receivedCommand, incomingData, sizeof(receivedCommand));

  // Debugging: wypisz dane przychodzące
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

    // Inicjalizacja ESP-NOW na ESP8266
    if (esp_now_init() != 0) {
        Serial.println("Błąd inicjalizacji ESP-NOW!");
        return;
    }

    // Rejestracja funkcji odbierającej dane
    esp_now_register_recv_cb(onDataReceive);

    // Konfiguracja serwomechanizmów i innych komponentów robota (bez zmian)
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
    stand(500);
    delay(1000);
}

void loop() {
    // Obsługa przycisków (jeśli są wciśnięte)
    if (receivedCommand.L1Pressed) {
      walkForward(5, 500);  // Idź do przodu
    }

    delay(50);  // Opóźnienie dla responsywności
}
