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

int currentTop = 0;

int currentBottom = 0;

int patternCount = 12; // Should be same as size of first dimension in patterns

int buttonPressed = 0;

int correct = 0;

int correctTimestamp = 0;

byte patterns[13][4] = { // first dimension should be same as patternCount
    {
        // 0
        B00000000,
        B00000000,
        B00000000,
        B00000000,
    },
    {
        // 1
        B00011000,
        B00100100,
        B01000010,
        B10000001,
    },
    {
        // 2
        B10000001,
        B01000010,
        B00100100,
        B00011000,
    },
    {
        // 3
        B00011000,
        B00100100,
        B00100100,
        B00011000,
    },
    {
        // 4
        B11111111,
        B10000001,
        B10000001,
        B10000001,
    },
    {
        // 5
        B10000001,
        B10000001,
        B10000001,
        B11111111,
    },
    {
        // 6
        B00011000,
        B00100100,
        B01000010,
        B11111111,
    },
    {
        // 7
        B11111111,
        B01000010,
        B00100100,
        B00011000,
    },
    {
        // 8
        B00000000,
        B11111111,
        B00000000,
        B11111111,
    },
    {
        // 9
        B01010101,
        B01010101,
        B01010101,
        B01010101,
    },
    {
        // 10
        B00100110,
        B01000010,
        B10000010,
        B01000010,
    },
    {
        // 11
        B11100000,
        B10000011,
        B10011001,
        B10001001,
    },
    {
        // 12
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

void allOff()
{
  for (int row = 0; row < 8; row++)
  {
    display.setRow(0, row, B00000000);
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
    delay(50);
  }
  allOff();
}

void displayHalfPattern(int startRow, int patternIndex)
{
  display.clearDisplay(0);
  int endRow = startRow + 4;
  for (int row = startRow; row < endRow; row++)
  {
    display.setRow(0, row, patterns[patternIndex][row]);
  }
}

void displayPattern(int topPattern, int bottomPattern)
{
  display.clearDisplay(0);
  for (int row = 0; row < 4; row++)
  {
    display.setRow(0, row, patterns[topPattern][row]);
    display.setRow(0, row + 4, patterns[bottomPattern][row]);
  }
}

void advancePattern(int bottom)
{
  if (bottom == 0)
  {
    currentTop++;
    if (currentTop > patternCount)
    {
      currentTop = 0;
    }
  }
  else
  {
    currentBottom++;
    if (currentBottom > patternCount)
    {
      currentBottom = 0;
    }
  }
}

void flashCorrect()
{
  for (int i = 0; i < 5; i++)
  {
    displayPattern(currentTop, currentBottom);
    delay(500);
    display.clearDisplay(0);
    delay(500);
  }
}

void setup()
{
  Serial.println("Startup");
  pinMode(TOP_BUTTON, INPUT);
  pinMode(BOTTOM_BUTTON, INPUT);
  display.shutdown(0, false); // Turn on the display
  display.setIntensity(0, 8); // Set the brightness (0-15)
  display.clearDisplay(0);    // Clear the display
  display.clearDisplay(0);
  Serial.println("Clear");
  delay(100);
  allOn();
  Serial.println("On");
  delay(100);
  loopPatterns();
}

void loop()
{
  // Catch top button press
  if (digitalRead(TOP_BUTTON) == HIGH)
  {
    Serial.println("Top");
    advancePattern(0);
    displayPattern(currentTop, currentBottom);
    delay(200);
  }
  // Catch bottom button press
  if (digitalRead(BOTTOM_BUTTON) == HIGH)
  {
    Serial.println("Bottom");
    advancePattern(1);
    displayPattern(currentTop, currentBottom);
    delay(200);
  }
  if (currentTop == 6 && currentBottom == 7 || currentTop == 1 && currentBottom == 7 || currentTop == 6 && currentBottom == 2) // or 1, 7 or 6, 2
  {
    if (correct == 0)
    {
      Serial.println("Correct solution, will we wait long enough?");
      correct = millis();
    }
  }
  else
  {
    correct = 0;
  }
  if (correct > 0)
  {
    Serial.println("Checking if correct");
    if (millis() - correct > 1000)
    {
      Serial.println("Correct");
      flashCorrect();
      currentTop = 0;
      currentBottom = 0;
      correct = 0;
    }
  }
}
