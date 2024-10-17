#include "DHTesp.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int DHT_PIN = 10;
const int BASE_PIN = 2;
const int LED_PIN_5 = 5;
const int BUZZER_PIN = 9;
const float TEMP_STEP = 2.0;
float baseTemp = 0.0;
bool buzzerActive = false;

DHTesp dhtSensor;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(115200);
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
  
  lcd.begin(16, 2);
  lcd.backlight();

  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  baseTemp = data.temperature;
  
  lcd.setCursor(0, 0);
  lcd.print("Depart: ");
  lcd.print(baseTemp);

  for (int i = BASE_PIN; i <= 8; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }

  pinMode(LED_PIN_5, OUTPUT);
  digitalWrite(LED_PIN_5, HIGH);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
}

void loop() {
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  float currentTemp = data.temperature;

  if (isnan(currentTemp)) {
    delay(2000);
    return;
  }

  lcd.setCursor(0, 1);
  lcd.print("Actuelle: ");
  lcd.print(currentTemp);

  for (int i = BASE_PIN; i <= 8; i++) {
    if (i != LED_PIN_5) {
      digitalWrite(i, LOW);
    }
  }

  int ledCount = (currentTemp > baseTemp) ? (currentTemp - baseTemp) / TEMP_STEP 
                                          : (baseTemp - currentTemp) / TEMP_STEP;
  ledCount = constrain(ledCount, 0, 3);

  if (currentTemp > baseTemp) {
    for (int i = 0; i < ledCount; i++) {
      digitalWrite(BASE_PIN + 2 - i, HIGH);
    }
  } else {
    for (int i = 0; i < ledCount; i++) {
      digitalWrite(BASE_PIN + 4 + i, HIGH);
    }
  }

  buzzerActive = digitalRead(BASE_PIN) == HIGH || digitalRead(8) == HIGH;

  if (buzzerActive) {
    digitalWrite(BUZZER_PIN, HIGH);
    Serial.println("Buzzer activé");
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  delay(500);
}
