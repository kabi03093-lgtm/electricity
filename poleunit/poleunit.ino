#include <SPI.h>
#include <LoRa.h>

#define CURRENT_PIN 35
#define VOLTAGE_PIN 34

#define RELAY_PIN 26

#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 2

float homeVoltage = 0;
float homeCurrent = 0;

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
  voltage = voltage * 100;
  return voltage;
}

void setup()
{
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(433E6))
  {
    Serial.println("LoRa Failed");
    while (1);
  }

  Serial.println("POLE UNIT STARTED");
}

void loop()
{
  int packetSize = LoRa.parsePacket();

  if (packetSize)
  {
    String data="";

    while (LoRa.available())
      data += (char)LoRa.read();

    int commaIndex = data.indexOf(',');

    homeVoltage = data.substring(0, commaIndex).toFloat();
    homeCurrent = data.substring(commaIndex+1).toFloat();
  }

  float poleVoltage = readVoltage();
  float poleCurrent = readCurrent();

  float polePower = poleVoltage * poleCurrent;
  float homePower = homeVoltage * homeCurrent;

  Serial.println("-----------");
  Serial.print("Pole Voltage: ");
  Serial.println(poleVoltage);

  Serial.print("Pole Current: ");
  Serial.println(poleCurrent);

  Serial.print("Home Voltage: ");
  Serial.println(homeVoltage);

  Serial.print("Home Current: ");
  Serial.println(homeCurrent);

  float diff = polePower - homePower;

  Serial.print("Power Difference: ");
  Serial.println(diff);

  if (diff > 50)   // theft threshold
  {
    Serial.println("ELECTRICITY THEFT DETECTED");

    digitalWrite(RELAY_PIN, LOW);

    LoRa.beginPacket();
    LoRa.print("ALERT: THEFT DETECTED - POWER CUT");
    LoRa.endPacket();
  }
  else
  {
    digitalWrite(RELAY_PIN, HIGH);
  }

  delay(3000);
}