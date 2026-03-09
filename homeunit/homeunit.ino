#include <SPI.h>
#include <LoRa.h>

#define CURRENT_PIN 34
#define VOLTAGE_PIN 35

#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 2

float readCurrent()
{
  int value = analogRead(CURRENT_PIN);
  float voltage = value * (3.3 / 4095.0);
  float current = (voltage - 2.5) / 0.066;
  return abs(current);
}

float readVoltage()
{
  int value = analogRead(VOLTAGE_PIN);
  float voltage = value * (3.3 / 4095.0);
  voltage = voltage * 100;   // calibration factor
  return voltage;
}

void setup()
{
  Serial.begin(115200);

  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(433E6))
  {
    Serial.println("LoRa Failed");
    while (1);
  }

  Serial.println("HOME UNIT STARTED");
}

void loop()
{
  float homeCurrent = readCurrent();
  float homeVoltage = readVoltage();

  String data = String(homeVoltage) + "," + String(homeCurrent);

  LoRa.beginPacket();
  LoRa.print(data);
  LoRa.endPacket();

  Serial.print("Voltage: ");
  Serial.print(homeVoltage);
  Serial.print(" V   Current: ");
  Serial.print(homeCurrent);
  Serial.println(" A");

  int packetSize = LoRa.parsePacket();

  if (packetSize)
  {
    String msg="";
    while (LoRa.available())
      msg += (char)LoRa.read();

    Serial.println("Message from Pole:");
    Serial.println(msg);
  }

  delay(3000);
}