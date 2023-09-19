// 7/13/19

#include <FastLED.h>          // include neopixel library
#include <MCUFRIEND_kbv.h>    // include TFT display library
#include <Adafruit_GFX.h>     // include shape drawing library
#include <SoftwareSerial.h>   // include software serial for TFT
#include <SD.h>               // include SD card library
#include <SPI.h>              // include library for SDI communication for SD card
#include <avr/pgmspace.h>     // include library for storing data in MEMORY

#define numLeds 8             // number of neopixels
#define ledPin 49             // pin for neopixel communication
#define LCD_RESET A4          // TFT reset pin
#define LCD_CS A3             // TFT chip select
#define SD_CS 10              // SD card pin on shield MAYBE
#define LCD_CD A2             // TFT command / data pin
#define LCD_WR A1             // TFT write
#define LCD_RD A0             // TFT read
#define winPin 30             // input signal to start program

// https://ee-programming-notepad.blogspot.com/2016/10/16-bit-color-generator-picker.html
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define PIZZA   0xF560
#define CRUST   0x7285

SoftwareSerial ss(A9, A8);     // voice module (RX, TX) was 50, 52
CRGB led [numLeds];            // creates instance of CRGB
MCUFRIEND_kbv tft;             // instance of TFT

byte wordLength;               // global variable for word length
byte syllables;                // estimated number of syllables for the word
bool letterSpell = false;      // true if spelling is in progress
byte ledStep = 0;              // which step of 8 for LED cycle
byte ledDelay;                // delay between each led in talk cycle

void setup() {
  pinMode(winPin, INPUT_PULLUP);
  Serial.begin(9600);          // open serial monitor
  ss.begin(9600);             // begin software serial
  resetScreen();
  tft.begin(0x9486);          // begin 9486 TFT
  if (!SD.begin(SD_CS)) {
  Serial.print("failed!");
  return;
  }
  tft.fillScreen(BLACK);      // black screen
  tft.setRotation(3);         // rotates so USB is on left
  FastLED.addLeds<NEOPIXEL, ledPin>(led, numLeds); // begin fastLED
  for (byte i = 0 ; i < numLeds; i++) {
    led[i] = CRGB(0, 0, 0);   // turn all neopixels off
  }
  FastLED.show();
  tft.setCursor(0, 30);
}

void loop() {
  fullProgram();
  //checkInput();
}

void checkInput() {
  Serial.print("winPin");
  Serial.println(digitalRead(winPin));
}

void fullProgram() {
  Serial.println(digitalRead(winPin));
  if (digitalRead(winPin) == LOW) {
    speakSentence("Hooray! You can find your impossible selfie at...");
    for (int i=100; i<280; i+=30) {
      tft.setCursor(75, i);
      speakSentence("www.pizzaslice.tk");
    }
  delay(15000);
  resetScreen();
  }
}

void neoSpeechIntegrated() {
  byte delayTime = 25;
  if (ledStep == 0) {
    led[3] = CRGB(150, 50, 0);
    led[4] = CRGB(150, 50, 0);
  }
  if (ledStep == 1) {
    led[2] = CRGB(0, 255, 0);
    led[5] = CRGB(0, 255, 0);
  }
  if (ledStep == 2) {
    led[1] = CRGB(0, 0, 255);
    led[6] = CRGB(0, 0, 255);
  }
  if (ledStep == 3) {
    led[0] = CRGB(255, 0, 255);
    led[7] = CRGB(255, 0, 255);
  }
  /*if (ledStep <= 3) {
    led[ledStep + 4] = CRGB(255, green, 0);
    led[3 - ledStep] = CRGB(255, green, 0);
  }*/
  if (ledStep > 3) {
    led[ledStep - 4] = CRGB(0, 0, 0);
    led[11 - ledStep] = CRGB(0, 0, 0);
  }
  ledStep++;
  if (ledStep == 8) {
    ledStep = 0;
  }
  FastLED.show();
  delay(25);
}

void screen1() {
  byte border = 0;             // border thickness
  tft.drawRect(border, border, tft.width() - 2 * border, tft.height() - 2 * border, PIZZA);
  border = 1;                   // border thickness
  tft.drawRect(border, border, tft.width() - 2 * border, tft.height() - 2 * border, PIZZA);
  border = 2;             // border thickness
  tft.drawRect(border, border, tft.width() - 2 * border, tft.height() - 2 * border, PIZZA);

}

void resetScreen() {
  tft.fillScreen(BLACK);        // clears screen
  tft.setCursor(0, 30);          //  sets cursor to top left
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
}

void testNeoPixels() {
  led[7] = CRGB(0, 0, 1);   // turn all neopixels off
  led[0] = CRGB(0, 0, 1);   // turn all neopixels off
  FastLED.show();
}

void readChip() {              // reads hex data from voice chip
  while (ss.available() ) {
    Serial.println(ss.read(), HEX);
  }
}

void waitForSpeech(unsigned long timeout = 60000) {
  unsigned long start = millis();
  bool done = false;
  while (! done && (millis() - start) < timeout) {
    while (ss.available()) {
      if (ss.read() == 0x4F) {
        done = true;
        break;

      }
    }
  }
}

void speakSentence(char textMsg[]) {
  wordLength = strlen(textMsg);                   // pass string length to global variable
  if (wordLength < 5) {
    syllables = 1;
  }
  if (wordLength >= 5 && wordLength < 8) {
    syllables = 2;
  }
  if (wordLength = 8) {
    syllables = 3;
  }
  if (wordLength > 8) {
    syllables = 4;
  }
  byte delayTime = 0;
  char* style = "[f1][x0][g2][h2][v10][m51][s8][t6]";
  ss.write(0xFD);
  ss.write((byte)0x0);
  ss.write(2 + strlen(textMsg) + strlen(style));  //size
  ss.write(0x01);             //command type
  ss.write((unsigned char)0x0);
  ss.write(style);
  ss.write(textMsg);
  delay(500);
  for (byte i = 0; i < strlen(textMsg); i++) {
    tft.print(textMsg[i]);
    for (byte j = 1; j < 8*syllables / wordLength; j++) {
      neoSpeechIntegrated();
    }
    if (i == strlen(textMsg) - 1) {
      for (byte k = ledStep; k<8; k++) {
        neoSpeechIntegrated();
      }
    }
    delay(delayTime);
  }
  waitForSpeech();
}
