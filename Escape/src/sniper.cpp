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

/*************************************
 * CUSTOMIZE THIS NUMBER TO SET YOUR CODES
 */
const uint8_t code[] = {1050, 0050, 0007}; // Different codes beginning with the first stage
const uint8_t levels = 3;                  // Total number of stages

/**
 * @brief PIN number of an LED.
 *
 * The default `2` is the blue LED on ESP-12E.
 */
static const int BUILTINLED = 2;

// Pins for the 7-segment display
#define CLK_PIN D5
#define DIO_PIN D6

// Pins for the buttons
#define BUTTON_A_PIN D2
#define BUTTON_B_PIN D3
#define BUTTON_C_PIN D4

// Pin for speaker
#define SOUND_PIN 11

// Delay between frames in milliseconds
#define FRAME_DELAY 100

const uint8_t circleFrames[] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G};
const uint8_t allON[] = {0xff, 0xff, 0xff, 0xff};
const uint8_t allOFF[] = {0x00, 0x00, 0x00, 0x00};
const uint8_t yes[] = {
    SEG_B | SEG_C | SEG_D | SEG_F | SEG_G,            // y
    SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,            // E
    SEG_A | SEG_C | SEG_D | SEG_F | SEG_G};           // S
const uint8_t wrong[] = {SEG_G, SEG_G, SEG_G, SEG_G}; // - - - -

TM1637Display display(CLK_PIN, DIO_PIN);

int digitIndex = 1; // Never select the leftmost digit
int digitValue = 0;
uint8_t currentDigit[4] = {0, 0, 0, 0};

int codeStage = 0;

// Increases value of selected digit
void increaseDigit()
{
  digitValue++;
  if (digitValue > 9)
  {
    digitValue = 0;
  }
}

void updateDisplay()
{
  display.showNumberDecEx(digitValue, 0, false, 1, digitIndex); // Display the digit with leading zero
  currentDigit[digitIndex] = digitValue;                        // Update the digit tracker
}

void handleButtonA()
{
  increaseDigit();
  updateDisplay();
}

void handleButtonB()
{
  digitIndex++;
  if (digitIndex > 3)
  {
    digitIndex = 1;
  }
}

uint8_t getDigits()
{
  return currentDigit[0] * 1000 + currentDigit[1] * 100 + currentDigit[2] * 10 + currentDigit[3];
}

void handleButtonC()
{
  if (code[codeStage] == getDigits())
  {              // Check if the correct code for the current codeStage is being displayed
    codeStage++; // If so, advance the stage
    success();
    digitValue = 0; // clear all the numbers
    for (int i = 0; i < 4; i++)
    {
      digitIndex = i;
      updateDisplay();
    }
    if (codeStage == levels) // If that was the last stage, turn every segment on
    {
      display.setSegments(allON);
      laserSound();
    }
    else
    {
      digitIndex = codeStage + 1;
    }
  }
  else
  {
    failure();
  }
}

void success()
{
  display.setSegments(yes);
  delay(1000);
  display.showNumberDecEx(getDigits(), 0, true, 4, 0); // Use getDigits to get current digits, then display them
}

void failure()
{
  display.setSegments(wrong);
  delay(1000);
  display.showNumberDecEx(getDigits(), 0, true, 4, 0);
}

void laserSound()
{
  for (int j = 0; j < 3; j++)
  {
    for (int i = 0; i < 200; i++)
    {
      digitalWrite(SOUND_PIN, HIGH);
      delayMicroseconds(i);
      digitalWrite(SOUND_PIN, LOW);
      delayMicroseconds(i);
    }
  }
}

void setup()
{
  display.setBrightness(7); // Adjust brightness (0-7)

  // Clear the display
  display.clear();

  pinMode(BUTTON_A_PIN, INPUT_PULLUP);
  pinMode(BUTTON_B_PIN, INPUT_PULLUP);
  pinMode(BUTTON_C_PIN, INPUT_PULLUP);
  pinMode(SOUND_PIN, OUTPUT);
}

void loop()
{
  if (digitalRead(BUTTON_A_PIN) == LOW)
  {
    handleButtonA();
    delay(100); // Debounce delay
  }

  // Check if button B is pressed
  if (digitalRead(BUTTON_B_PIN) == LOW)
  {
    handleButtonB();
    delay(100); // Debounce delay
  }

  // Check if button B is pressed
  if (digitalRead(BUTTON_C_PIN) == LOW)
  {
    handleButtonC();
    delay(100); // Debounce delay
  }
}