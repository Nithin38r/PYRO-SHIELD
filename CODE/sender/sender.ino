#include <SPI.h>
#include <LoRa.h>
#include "DHT.h"

// LoRa Pins
#define SS   5
#define RST  14
#define DIO0 2

// Sensor Pins
#define FLAME_SENSOR 34 // Flame sensor digital pin
#define MQ2_SENSOR   35 // MQ-2 sensor digital pin
#define DHTPIN       4  // DHT22 data pin
#define DHTTYPE      DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  while (!Serial);

  pinMode(FLAME_SENSOR, INPUT);
  pinMode(MQ2_SENSOR, INPUT);

  Serial.println("LoRa Sender with DHT22 + Flame + MQ-2 Sensor");
  dht.begin();

  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed. Check wiring.");
    while (1);
  }
}

void loop() {
  int flameValue = digitalRead(FLAME_SENSOR); // LOW = flame detected
  int mq2Value   = digitalRead(MQ2_SENSOR);   // LOW = smoke detected
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT22!");
    return;
  }

  // Prepare LoRa message
  LoRa.beginPacket();
  LoRa.print("Temp: ");
  LoRa.print(temperature);
  LoRa.print("C, Hum: ");
  LoRa.print(humidity);
  LoRa.print("% ");

  if (flameValue == LOW || mq2Value == LOW) {
    LoRa.print(" ALERT: SMOKE/FLAME DETECTED!");
    Serial.println("Smoke or flame detected! Sending alert...");
  } else {
    LoRa.print(" Status: Normal");
    Serial.println("No smoke, no flame. Sending normal feed...");
  }

  LoRa.endPacket();

  delay(2000); // Send every 2 seconds
}
