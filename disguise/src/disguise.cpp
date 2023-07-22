/**
 * @file disguise.cpp
 * @author Alex Becker
 * @brief Input a shape to get the correct answer
 * @version 0.1
 * @date 2023-07-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <LedControl.h>

#define DATA_PIN 13 // D7
#define CS_PIN 12   // D6
#define CLK_PIN 14  // D5

#define TOP_BUTTON 5    // D1
#define BOTTOM_BUTTON 4 // D2

LedControl display = LedControl(DATA_PIN, CLK_PIN, CS_PIN, 1);

byte squarePattern[8] = {
    B11111111,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B11111111};

int currentPattern = 0;

int patternCount = 12; // Should be same as size of first dimension in patterns

int buttonPressed = 0;

byte patterns[12][4] = { // first dimension should be same as patternCount
    {
        B00011000,
        B00100100,
        B01000010,
        B10000001,
    },
    {
        B10000001,
        B01000010,
        B00100100,
        B00011000,
    },
    {
        B00011000,
        B00100100,
        B00100100,
        B00011000,
    },
    {
        B11111111,
        B10000001,
        B10000001,
        B10000001,
    },
    {
        B10000001,
        B10000001,
        B10000001,
        B11111111,
    },
    {
        B00011000,
        B00100100,
        B01000010,
        B11111111,
    },
    {
        B11111111,
        B01000010,
        B00100100,
        B00011000,
    },
    {
        B00000000,
        B11111111,
        B00000000,
        B11111111,
    },
    {
        B01010101,
        B01010101,
        B01010101,
        B01010101,
    },
    {
        B00100110,
        B01000010,
        B10000010,
        B01000010,
    },
    {
        B11100000,
        B10000011,
        B10011001,
        B10001001,
    },
    {
        B01000100,
        B11000111,
        B00110000,
        B00010000,
    }};

void allOn()
{
  for (int row = 0; row < 8; row++)
  {
    display.setRow(0, row, B11111111);
  }
}

void loopPatterns()
{
  for (int pat = 0; pat < 12; pat++)
  {
    for (int row = 0; row < 4; row++)
    {
      display.setRow(0, row, patterns[pat][row]);
      display.setRow(0, row + 4, patterns[pat][row]);
    }
    delay(500);
  }
}

void displayPattern(int startRow, int patternIndex)
{
  display.clearDisplay(0);
  int endRow = startRow + 4;
  for (int row = startRow; row < endRow; row++)
  {
    display.setRow(0, row, patterns[patternIndex][row]);
  }
}

void setup()
{
  pinMode(TOP_BUTTON, INPUT_PULLUP);
  pinMode(BOTTOM_BUTTON, INPUT_PULLUP);
  display.shutdown(0, false); // Turn on the display
  display.setIntensity(0, 8); // Set the brightness (0-15)
  display.clearDisplay(0);    // Clear the display
  display.clearDisplay(0);
  Serial.println("Clear");
  delay(100);
  allOn();
  Serial.println("On");
  delay(2000);
  loopPatterns();
}

void loop()
{
  // Catch top button press
  if (digitalRead(TOP_BUTTON) == LOW)
  {
    buttonPressed = 1;
  }
  // Catch bottom button press
  if (digitalRead(BOTTOM_BUTTON) == LOW)
  {
    buttonPressed = 2;
  }
  // Check if any button was pressed
  if (buttonPressed > 0)
  {
    currentPattern++;                  // Advance to next pattern
    if (currentPattern > patternCount) // Wrap to beginning if at the end
    {
      currentPattern = 0;
    }
    // Start at row 1-1*4 = 0 if top button, or 2-1*4=4 if bottom button
    displayPattern((buttonPressed - 1) * 4, currentPattern);
    buttonPressed = 0; // Reset button press detection
    delay(200);        // debounce
  }
}
