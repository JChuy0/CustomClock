
## Custom Alarm Clock
A custom-built physical alarm clock that displays real-time air quality data and animated visuals.


## Motivation - Why I built it
I wanted a clock themed around Meisho Doto from Umamusume. But she has very little merchandise. So, I thought I'd take the chance to learn new tech and build my own while making it fully customizable.


## Project Status - In progress


## Features
- Customizable alarm
- Real-time air quality monitoring (temperature, humidity, air pollution)
- Animated display modes


## Hardware
- ESP32
- 3.5" IPS Display
- BME680 Air Quality Sensor
- DS1307 Real Time Clock (RTC) Module
- Rotary Encoder
- Speaker
- Custom 3D-printed case
- USB-C power


## Software setup
- C++ (Arduino framework)

Download the code repo, unzip it, open in the Arduino IDE. At the top of the screen, open the dropdown menu and select your microcontroller, then press the upload button.
To flash the images to the microcontroller, open the command pallete (ctrl + shift + P) and select 'Upload LittleFS to pico/ESP8266/ESP32'.

## Dependancies
- Adafruit_BME680 v2.0.5
- TFT_eSPI v2.5.43
- RTClib v2.1.4
- PNGdec 1.1.6
- LittleFS


## Tools Required
- Soldering Iron
- 3D printer

Photos and schematics coming soon

## Usage Instructions
Press the encoder button to swap between display mode, time edit mode, alarm edit mode, and to turn the alarm on/off. When in edit mode, twist the encoder to change the time. Alarm will continue ringing until the encoder button is pressed.


## Future Plans
- Animation changes based on the current state of the clock
- Night mode with automatic brightness adjustment
- Replace current alarm chime with an audio file
- Move animations from internal memory to SD card
- Send air data to external database for graphing


## Credits
Animation was taken from the mobile game 'Umamusume Pretty Derby' developed by Cygames.
