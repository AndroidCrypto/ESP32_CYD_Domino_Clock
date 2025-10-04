/*
  This sketch displays a clock with 'Domino' clock face on the 320*240 display
  of the ESP32-CYD 2.8 inch TFT display.

  The faces are stored as '.h' files in PROGMEM.

  The time is from an NTP server with Time Zone correction.

  The Clock face images are taken from https://github.com/judge2005/EleksTubeIPS
  see folder: more_faces | dom2
  BMPs are 8 Color Depth and the original size is 135 x 240 pixels
*/

/*
Version Management

04.10.2025 V02 unchanged main sketch but this version is using domino-style clock faces
31.07.2025 V02 This version is writing the clock face sprites directly to the tft. Simple Reason:
               out of heap error when displaying the 2 year digits on display-sprite
31.07.2025 V01 Initial programming

*/

// ----------------------------------------------------------------
// Sketch and Board information
const char* PROGRAM_VERSION = "ESP32 CYD Domino Clock V02";
const char* PROGRAM_TITLE = "ESP32 CYD Domino Clock";

// ----------------------------------------------------------------
// WiFi / NTP

const char* WIFI_SSID = "change to your router";
const char* WIFI_PASSWORD = "change to your password";

// Time management
// Configuration of NTP
// choose the best fitting NTP server pool for your country
//#define MY_NTP_SERVER "at.pool.ntp.org"
#define MY_NTP_SERVER "pool.ntp.org"

// choose your time zone from this list
// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
//#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"
#define MY_TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"

#include <time.h>
#include <WiFi.h>

// Time Globals
time_t now;  // this are the seconds since Epoch (1970) - UTC
tm tm;       // the structure tm holds time information in a more convenient way *
uint8_t hh, mm, ss;
uint8_t dy, mt;  // day and month
uint16_t yr;     // year

// ----------------------------------------------------------------
// TFT Display

#include <Arduino.h>
#include "SPI.h"
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite ts[10] = { TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft) };

#define DISPLAY_ORITENTATION_LANDSCAPE 1  // 1 = USB terminal right side, 2 = USB Top, 3 = USB terminal left side
#define DISPLAY_HEIGHT 240
#define DISPLAY_WIDTH 320

#include "0.h";
#include "1.h";
#include "2.h";
#include "3.h";
#include "4.h";
#include "5.h";
#include "6.h";
#include "7.h";
#include "8.h";
#include "9.h";

const uint16_t DISPLAY_BACKGROUND_COLOR = TFT_BLACK;
const uint16_t DISPLAY_TITLE_COLOR = TFT_SKYBLUE;

const uint16_t CLOCK_FACE_WIDTH = 50;
const uint16_t CLOCK_FACE_HEIGHT = 88;

const uint16_t TITLE_POS_Y = 10;
const uint16_t CLOCK_POS_X = 10;
const uint16_t CLOCK_POS_Y = 50;
const uint16_t DIGITS_WIDTH = CLOCK_FACE_WIDTH;
const uint16_t CALENDER_POS_Y = 140;

// ----------------------------------------------------------------
// variables

unsigned long ms = millis();

void setupNtpTime() {
  // Obtain current time and set variables for the Second, Minute and Hour
  configTime(0, 0, MY_NTP_SERVER);  // 0, 0 because we will use TZ in the next line
  setenv("TZ", MY_TIMEZONE, 1);     // Set environment variable with your time zone
  tzset();
}

void setupWiFi() {
  // start network
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void setupDisplay() {
  // initial setup of display unit
  tft.init();  // initialise the display unit
  //tft.begin();
  tft.setRotation(DISPLAY_ORITENTATION_LANDSCAPE);
  tft.fillScreen(DISPLAY_BACKGROUND_COLOR);  // fill screen with white background
  // initialize the clock face sprites to get true colors
  for (int i = 0; i < 10; i++) {
    ts[i].setSwapBytes(true);
    ts[i].setColorDepth(8);
    ts[i].createSprite(CLOCK_FACE_WIDTH, CLOCK_FACE_HEIGHT);
  }
}

void createDisplayTimeCharacters() {
  for (int i = 0; i < 10; i++) {
    ts[i].setColorDepth(8);
    ts[i].setSwapBytes(true);
    ts[i].createSprite(CLOCK_FACE_WIDTH, CLOCK_FACE_HEIGHT);
  }
  ts[0].pushImage(0, 0, CLOCK_FACE_WIDTH, CLOCK_FACE_HEIGHT, _0);
  ts[1].pushImage(0, 0, CLOCK_FACE_WIDTH, CLOCK_FACE_HEIGHT, _1);
  ts[2].pushImage(0, 0, CLOCK_FACE_WIDTH, CLOCK_FACE_HEIGHT, _2);
  ts[3].pushImage(0, 0, CLOCK_FACE_WIDTH, CLOCK_FACE_HEIGHT, _3);
  ts[4].pushImage(0, 0, CLOCK_FACE_WIDTH, CLOCK_FACE_HEIGHT, _4);
  ts[5].pushImage(0, 0, CLOCK_FACE_WIDTH, CLOCK_FACE_HEIGHT, _5);
  ts[6].pushImage(0, 0, CLOCK_FACE_WIDTH, CLOCK_FACE_HEIGHT, _6);
  ts[7].pushImage(0, 0, CLOCK_FACE_WIDTH, CLOCK_FACE_HEIGHT, _7);
  ts[8].pushImage(0, 0, CLOCK_FACE_WIDTH, CLOCK_FACE_HEIGHT, _8);
  ts[9].pushImage(0, 0, CLOCK_FACE_WIDTH, CLOCK_FACE_HEIGHT, _9);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println(PROGRAM_VERSION);

  setupNtpTime();
  setupWiFi();
  setupDisplay();

  tft.setTextColor(DISPLAY_TITLE_COLOR, DISPLAY_BACKGROUND_COLOR);
  tft.drawCentreString(PROGRAM_TITLE, DISPLAY_WIDTH / 2, TITLE_POS_Y, 4);

  createDisplayTimeCharacters();
}


void updateClock() {
  // there are each 2 digits for the hour, minutes and seconds
  // hour
  if (hh < 10) {
    //display.pushSprite(0, 0, TFT_TRANSPARENT);
    ts[0].pushSprite(CLOCK_POS_X, CLOCK_POS_Y, TFT_TRANSPARENT);
    ts[hh].pushSprite(CLOCK_POS_X + DIGITS_WIDTH, CLOCK_POS_Y, TFT_TRANSPARENT);
  } else if ((hh > 9) && (hh < 20)) {
    ts[1].pushSprite(CLOCK_POS_X, CLOCK_POS_Y, TFT_TRANSPARENT);
    ts[hh - 10].pushSprite(CLOCK_POS_X + DIGITS_WIDTH, CLOCK_POS_Y, TFT_TRANSPARENT);
  } else {
    ts[2].pushSprite(CLOCK_POS_X, CLOCK_POS_Y, TFT_TRANSPARENT);
    ts[hh - 20].pushSprite(CLOCK_POS_X + DIGITS_WIDTH, CLOCK_POS_Y, TFT_TRANSPARENT);
  }
  // minutes
  if (mm < 10) {
    ts[0].pushSprite(CLOCK_POS_X + (2 * DIGITS_WIDTH), CLOCK_POS_Y, TFT_TRANSPARENT);
    ts[mm].pushSprite(CLOCK_POS_X + (3 * DIGITS_WIDTH), CLOCK_POS_Y, TFT_TRANSPARENT);
  } else if (mm > 9) {
    int mmFirst = mm / 10;
    ts[mmFirst].pushSprite(CLOCK_POS_X + (2 * DIGITS_WIDTH), CLOCK_POS_Y, TFT_TRANSPARENT);
    ts[mm - (mmFirst * 10)].pushSprite(CLOCK_POS_X + (3 * DIGITS_WIDTH), CLOCK_POS_Y, TFT_TRANSPARENT);
  }
  // seconds
  if (ss < 10) {
    ts[0].pushSprite(CLOCK_POS_X + (4 * DIGITS_WIDTH), CLOCK_POS_Y, TFT_TRANSPARENT);
    ts[ss].pushSprite(CLOCK_POS_X + (5 * DIGITS_WIDTH), CLOCK_POS_Y, TFT_TRANSPARENT);
  } else if (ss > 9) {
    int ssFirst = ss / 10;
    ts[ssFirst].pushSprite(CLOCK_POS_X + (4 * DIGITS_WIDTH), CLOCK_POS_Y, TFT_TRANSPARENT);
    ts[ss - (ssFirst * 10)].pushSprite(CLOCK_POS_X + (5 * DIGITS_WIDTH), CLOCK_POS_Y, TFT_TRANSPARENT);
  }
}

void updateCalender() {
  // there are each 2 digits for the day, month and year
  // day
  if (dy < 10) {
    ts[0].pushSprite(CLOCK_POS_X, CALENDER_POS_Y, TFT_TRANSPARENT);
    ts[dy].pushSprite(CLOCK_POS_X + DIGITS_WIDTH, CALENDER_POS_Y, TFT_TRANSPARENT);
  } else if ((dy > 9) && (dy < 20)) {  
    ts[1].pushSprite(CLOCK_POS_X, CALENDER_POS_Y, TFT_TRANSPARENT);
    ts[dy - 10].pushSprite(CLOCK_POS_X + DIGITS_WIDTH, CALENDER_POS_Y, TFT_TRANSPARENT);
  } else if ((dy > 19) && (dy < 30)) {
    ts[2].pushSprite(CLOCK_POS_X, CALENDER_POS_Y, TFT_TRANSPARENT);
    ts[dy - 20].pushSprite(CLOCK_POS_X + DIGITS_WIDTH, CALENDER_POS_Y, TFT_TRANSPARENT);
  } else {
    ts[3].pushSprite(CLOCK_POS_X, CALENDER_POS_Y, TFT_TRANSPARENT);
    ts[dy - 30].pushSprite(CLOCK_POS_X + DIGITS_WIDTH, CALENDER_POS_Y, TFT_TRANSPARENT);
  }

  // month
  if (mt < 10) {
    ts[0].pushSprite(CLOCK_POS_X + (2 * DIGITS_WIDTH), CALENDER_POS_Y, TFT_TRANSPARENT);
    ts[mt].pushSprite(CLOCK_POS_X + (3 * DIGITS_WIDTH), CALENDER_POS_Y, TFT_TRANSPARENT);
  } else {
    //Serial.printf("mmFirst: %d mmLast: %d\n", mmFirst, mm - (mmFirst * 10));
    ts[1].pushSprite(CLOCK_POS_X + (2 * DIGITS_WIDTH), CALENDER_POS_Y, TFT_TRANSPARENT);
    ts[mt - 10].pushSprite(CLOCK_POS_X + (3 * DIGITS_WIDTH), CALENDER_POS_Y, TFT_TRANSPARENT);
  }
 
  // year, just the last 2 digits
  ts[(yr - 2000) / 10].pushSprite(CLOCK_POS_X + (4 * DIGITS_WIDTH), CALENDER_POS_Y, TFT_TRANSPARENT);
  ts[(yr / 1U) % 10].pushSprite(CLOCK_POS_X + (5 * DIGITS_WIDTH), CALENDER_POS_Y, TFT_TRANSPARENT);
}

void printCurrentTime() {
  time(&now);              // read the current time
  localtime_r(&now, &tm);  // update the structure tm with the current time
  Serial.print("year:");
  Serial.print(tm.tm_year + 1900);  // years since 1900
  Serial.print("\tmonth:");
  Serial.print(tm.tm_mon + 1);  // January = 0 (!)
  Serial.print("\tday:");
  Serial.print(tm.tm_mday);  // day of month
  Serial.print("\thour:");
  Serial.print(tm.tm_hour);  // hours since midnight 0-23
  Serial.print("\tmin:");
  Serial.print(tm.tm_min);  // minutes after the hour 0-59
  Serial.print("\tsec:");
  Serial.print(tm.tm_sec);  // seconds after the minute 0-61*
  Serial.print("\twday");
  Serial.print(tm.tm_wday);  // days since Sunday 0-6
  if (tm.tm_isdst == 1)      // Daylight Saving Time flag
    Serial.print("\tDST");
  else
    Serial.print("\tstandard");
  Serial.println();
}

void manageDisplay() {
  // Only update the screen once every second and at the start
  if (millis() - ms >= 1000) {
    hh = tm.tm_hour;
    mm = tm.tm_min;
    ss = tm.tm_sec;
    dy = tm.tm_mday;
    mt = tm.tm_mon + 1;
    yr = tm.tm_year + 1900;

    printCurrentTime();
    ms = millis();
    if (++ss > 59) {
      ss = 0;
      if (++mm > 59) {
        mm = 0;
        if (++hh > 23) hh = 0;
      }
    }
    // 6 digits
    updateClock();
    updateCalender();
  }
}

void loop() {
  manageDisplay();  // manage clock display
}
