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
BME680Data readBME680() {

  BME680Data data;
  unsigned long endTime = bme.beginReading();

  // Get measurement results from BME680.
    if (!bme.endReading()) {
    Serial.println(F("Failed to complete reading :("));
    return data;
  }

  // Serial.print("Temp: ");
  // Serial.println(bme.temperature);
  // Serial.print("Humidity: ");
  // Serial.println(bme.humidity);
  // Serial.println();

  data.temperature = bme.temperature;
  data.humidity = bme.humidity;

  return data;
}
