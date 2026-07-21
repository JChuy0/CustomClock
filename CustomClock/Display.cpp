#include "Display.h"

#define FileSys LittleFS
#define MAX_IMAGE_WIDTH 240

TFT_eSPI tft = TFT_eSPI();


// FOR DISPLAYING IMAGES
String imageFiles[15];
int imageCount = 0;
int16_t image_xpos = 350;
int16_t image_ypos = 180;

File myFile;
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

void displayClock(BME680Data airData, bool alarmOnOffValue) {
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
  sprintf(timeStr, "%02d:%02d:%02d", clockTime.hour, clockTime.minute, clockTime.second);

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



void * myOpen(const char *fileName, int32_t *size) {
  // Serial.printf("Attempting to open %s\n", fileName);
  myFile = FileSys.open(fileName);
  *size = myFile.size();
  return &myFile;
}

void myClose(void *handle) {
  if (myFile) myFile.close();
}

int32_t myRead(PNGFILE *handle, uint8_t *buffer, int32_t length) {
  if (!myFile) return 0;
  return myFile.read(buffer, length);
}

int32_t mySeek(PNGFILE *handle, int32_t position) {
  if (!myFile) return 0;
  return myFile.seek(position);
}

// Function to draw pixels to the display
int PNGDraw(PNGDRAW *pDraw) {
  uint16_t lineBuffer[MAX_IMAGE_WIDTH];
  png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
  tft.pushImage(image_xpos, image_ypos + pDraw->y, pDraw->iWidth, 1, lineBuffer);

  return 1;
}

// Load all images for a given folder
void loadAnimation(const char* filePath) {
  imageCount = 0;

  // Scan LittleFS and load any *.png files
  File root = LittleFS.open(filePath, "r");

  // Loads the filepath for each image into the array
  while (File file = root.openNextFile()) {
    String strName = file.path();

    // Serial.print("strName: ");
    // Serial.println(strName);

    // If it is not a directory and filename ends in .png then load it
    if (!file.isDirectory() && strName.endsWith(".png")) {
      imageFiles[imageCount] = strName;
      imageCount++;
    }
  }

  imageCount = 0;
}

void displayImage() {
  int16_t rc = png.open(imageFiles[imageCount].c_str(), myOpen, myClose, myRead, mySeek, PNGDraw);
  imageCount++;

  if (rc == PNG_SUCCESS) {
    rc = png.decode(NULL, 0);
    png.close();
  } else {
    imageCount = 0;
  }
}
