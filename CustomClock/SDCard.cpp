#include "SDCard.h"

int sck = 18;
int miso = 19;
int mosi = 23;
int cs = 13;


void setupSDCard() {
  SPI.begin(sck, miso, mosi, cs);
  if (!SD.begin(cs)) {
    Serial.println("Card Mount Failed");
    return;
  }

  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.print("SD Card Size: ");
  Serial.print(cardSize);
  Serial.println("MB");

  Serial.print("Total space: ");
  Serial.print(SD.totalBytes() / (1024 * 1024));
  Serial.println("MB");
  Serial.print("Used space: ");
  Serial.print(SD.usedBytes() / (1024 * 1024));
  Serial.println("MB");
}
