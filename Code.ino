#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <FastLED.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

#define NUM_LEDS 16
#define DATA_PIN 4
#define BUZZER_PIN 13 // Pino do buzzer

CRGB leds[NUM_LEDS];
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  dht.begin();

  Wire.begin(21, 22);  // Configurar os pinos SDA e SCL para o LCD
  lcd.begin(16, 2);
  lcd.backlight();

  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  delay(2000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  updateLedBar(h, t);
  checkAndSoundBuzzer(h, t);

  // Atualiza o LCD com os valores de temperatura e umidade
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Humidity:"));
  lcd.print(h);

  lcd.setCursor(0, 1);
  lcd.print(F("Temperature:"));
  lcd.print(t);
}

void updateLedBar(float humidity, float temperature) {
  // Condições para as cores dos LEDs
  bool isTemperatureInRange = (temperature >= 25 && temperature <= 35);
  bool isHumidityInRange = (humidity >= 20 && humidity <= 40);

  // Atualiza a cor dos LEDs NeoPixel com base nas condições
  if (isTemperatureInRange && isHumidityInRange) {
    // Condição ideal: temperatura e umidade dentro do padrão (verde)
    fill_solid(leds, NUM_LEDS, CRGB::Green);
  } else if (!isTemperatureInRange && !isHumidityInRange) {
    // Ambas temperatura e umidade fora do padrão (vermelho)
    fill_solid(leds, NUM_LEDS, CRGB::Red);
  } else {
    // Apenas uma das duas (temperatura ou umidade) fora do padrão (amarelo)
    fill_solid(leds, NUM_LEDS, CRGB::Yellow);
  }

  FastLED.show();
}

void checkAndSoundBuzzer(float humidity, float temperature) {
  // Condições para acionar o buzzer
  bool isTemperatureOutOfRange = (temperature < 25 || temperature > 35);
  bool isHumidityOutOfRange = (humidity < 20 || humidity > 40);

  if (isTemperatureOutOfRange || isHumidityOutOfRange) {
    // Aciona o buzzer com um tom específico (ajuste conforme necessário)
    tone(BUZZER_PIN, 3000); 
  } else {
    // Desliga o buzzer
    noTone(BUZZER_PIN);
  }
}
