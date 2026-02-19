#include "Display.h"

#define FileSys LittleFS
#define MAX_IMAGE_WIDTH 240

TFT_eSPI tft = TFT_eSPI();

int16_t image_xpos = 350;
int16_t image_ypos = 180;

File pngfile;
PNG png;

void displayTimeEditor(timeData &time, const char* label = "");
void displayAlarmOnOff(bool alarmOnOff, const char* label = "");


void setupScreen() {
  tft.init();

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setRotation(3);

  if (!FileSys.begin()) {
    Serial.println("LittleFS initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }
}

void resetScreen() {
  tft.fillScreen(TFT_BLACK);
}

void displayClock(DateTime currTime, BME680Data airData, bool alarmOnOffValue) {
  Serial.println("Entered displayClock.");

  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  // Air Sensor Data
  char temperature[20];
  char humidity[20];

  sprintf(temperature, "%.1f C", airData.temperature);
  sprintf(humidity, "%.1f %", airData.humidity);

  // Temperature
  tft.drawString("Temperature:", 0, 0);
  tft.drawString(temperature, 25, 20, 2);

  // Humidity
  tft.drawString("Humidity:", 180, 0);
  tft.drawString(humidity, 195, 20, 2);

  // Clock
  char timeStr[10];
  sprintf(timeStr, "%02d:%02d:%02d", currTime.hour(), currTime.minute(), currTime.second());

  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(timeStr, 0, 250, 4);

  // Alarm
  displayAlarmOnOff(alarmOnOffValue);
}

void displayTimeEditor(timeData &time, const char* label) {

  // Time
  char timeStr[9];
  sprintf(timeStr, "%02d:%02d", time.hour, time.minute);

  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(label, 0, 200);
  tft.drawString(timeStr, 0, 250, 4);
}

void displayAlarmOnOff(bool alarmOnOff, const char* label) {
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(label, 250, 200, 2);

  if (alarmOnOff) {
    tft.drawString("On  ", 250, 250, 2);
  } else {
    tft.drawString("Off", 250, 250, 2);
  }
}


void * pngOpen(const char *filename, int32_t *size) {
  Serial.printf("Attempting to open %s\n", filename);
  pngfile = FileSys.open(filename, "r");
  *size = pngfile.size();
  return &pngfile;
}

void pngClose(void *handle) {
  File pngfile = *((File*)handle);
  if (pngfile) pngfile.close();
}

int32_t pngRead(PNGFILE *page, uint8_t *buffer, int32_t length) {
  if (!pngfile) return 0;
  page = page; // Avoid warning
  return pngfile.read(buffer, length);
}

int32_t pngSeek(PNGFILE *page, int32_t position) {
  if (!pngfile) return 0;
  page = page; // Avoid warning
  return pngfile.seek(position);
}

int pngDraw(PNGDRAW *pDraw) {
  uint16_t lineBuffer[MAX_IMAGE_WIDTH];
  png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
  tft.pushImage(image_xpos, image_ypos + pDraw->y, pDraw->iWidth, 1, lineBuffer);

  return 1;
}


void displayImage()
{
  String filePath ="clock/";

  // Scan LittleFS and load any *.png files
  File root = LittleFS.open("/" + filePath, "r");

  while (File file = root.openNextFile()) {
    String strname = file.name();
    strname = "/" + filePath + strname;

    // If it is not a directory and filename ends in .png then load it
    if (!file.isDirectory() && strname.endsWith(".png")) {
      // Pass support callback function names to library
      int16_t rc = png.open(strname.c_str(), pngOpen, pngClose, pngRead, pngSeek, pngDraw);

      if (rc == PNG_SUCCESS) {
        tft.startWrite();
        // Serial.printf("image specs: (%d x %d), %d bpp, pixel type: %d\n", png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType());
        uint32_t dt = millis();

        if (png.getWidth() > MAX_IMAGE_WIDTH) {
          Serial.println("Image too wide for allocated line buffer size!");
        } else {
          rc = png.decode(NULL, 0);
          png.close();
        }

        tft.endWrite();
      }
    }

    delay(150);
  }
}
