/**
 * @file sniper.cpp
 * @author Alex Becker
 * @brief Calibrate a sniper rifle. Three settings: Distance (###), Caliber (##), Heat Adjustment (#). Make a laser sound when correct.
 * @version 0.1
 * @date 2023-05-24
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <TM1637Display.h>
#include <vector> // Needed for a dynamically-allocated peer array

/**
 * @brief PIN number of an LED.
 *
 * The default `2` is the blue LED on ESP-12E.
 */
static const int BUILTINLED = 2;

// Pins for the 7-segment display
#define CLK_PIN 5
#define DIO_PIN 6

// Delay between frames in milliseconds
#define FRAME_DELAY 100

const uint8_t circleFrames[] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G};

TM1637Display display(CLK_PIN, DIO_PIN);

void setup()
{
  display.setBrightness(7); // Adjust brightness (0-7)

  // Clear the display
  display.clear();
}

void loop()
{
  for (uint8_t i = 0; i < sizeof(circleFrames) / sizeof(circleFrames[0]); i++)
  {
    display.setSegments(&circleFrames[i], 1, 0);
    delay(FRAME_DELAY);
  }
}