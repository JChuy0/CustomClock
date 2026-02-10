#include "AirSensor.h"
#include "Adafruit_BME680.h"

Adafruit_BME680 bme; // I2C


void setupBME680() {
  Serial.println(F("BME680 async test"));
  if (!bme.begin()) {
    Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
    while (1);
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
}

// Reads the temperature and humidity from a BME680 sensor
void readBME680() {
  char temperature[20];
  char humidity[20];
  unsigned long endTime = bme.beginReading();

  // Get measurement results from BME680.
    if (!bme.endReading()) {
    Serial.println(F("Failed to complete reading :("));
    return;
  }

  sprintf(temperature, "%.1f C", bme.temperature);
  sprintf(humidity, "%.1f %", bme.humidity);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Temperature:", 0, 0);
  tft.drawString(temperature, 25, 20, 2);

  tft.drawString("Humidity:", 180, 0);
  tft.drawString(humidity, 195, 20, 2);
}
