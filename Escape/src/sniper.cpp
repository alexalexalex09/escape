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

/*************************************
 * CUSTOMIZE THIS NUMBER TO SET YOUR CODES WITH NO LEADING ZEROES
 */
const int code[] = {1050, 50, 7}; // Different codes beginning with the first stage
const uint8_t levels = 3;         // Total number of stages

/**
 * @brief PIN number of an LED.
 *
 * The default `2` is the blue LED on ESP-12E.
 */
static const int BUILTINLED = 2;

// Pins for the 7-segment display
#define CLK_PIN 4 // D2
#define DIO_PIN 5 // D1

// Pins for the buttons
#define BUTTON_A_PIN 12 // D6
#define BUTTON_B_PIN 13 // D7
#define BUTTON_C_PIN 14 // D5

// Pin for speaker
#define SOUND_PIN 15

// Delay between frames in milliseconds
#define FRAME_DELAY 100

const uint8_t circleFrames[] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G};
const uint8_t allON[] = {0xff, 0xff, 0xff, 0xff};
const uint8_t allOFF[] = {0x00, 0x00, 0x00, 0x00};
const uint8_t blank[] = {0x00};
const uint8_t yes[] = {
    SEG_B | SEG_C | SEG_D | SEG_F | SEG_G,            // y
    SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,            // E
    SEG_A | SEG_C | SEG_D | SEG_F | SEG_G};           // S
const uint8_t wrong[] = {SEG_G, SEG_G, SEG_G, SEG_G}; // - - - -

TM1637Display display(CLK_PIN, DIO_PIN);

int blinkNow = 0; // used to decide whether to display the currently selected number
int testA = 1;
int digitIndex = 3; // Never select the rightmost digit
int digitValue = 0;
int currentDigit[4] = {0, 0, 0, 0};

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

void updateAll()
{
  for (int i = 0; i < 4; i++)
  {
    digitIndex = i;
    updateDisplay();
  }
}

void refreshAll()
{
  for (int i = 0; i < 4; i++)
  {
    digitIndex = i;
    display.showNumberDecEx(currentDigit[digitIndex], 0, false, 1, digitIndex); // Display the digit with leading zero
  }
}

void handleButtonA()
{
  increaseDigit();
  updateDisplay();
  Serial.println(digitValue);
}

void handleButtonB()
{
  digitIndex++;
  if (digitIndex > 3)
  {
    digitIndex = 0;
  }
  digitValue = currentDigit[digitIndex];
  Serial.println(digitIndex);
}

int getDigits()
{
  Serial.println("****************************");
  Serial.println(currentDigit[0]);
  Serial.println(currentDigit[1]);
  Serial.println(currentDigit[2]);
  Serial.println(currentDigit[3]);
  Serial.println(currentDigit[0] * 1000 + currentDigit[1] * 100 + currentDigit[2] * 10 + currentDigit[3]);
  Serial.println("****************************");
  return currentDigit[0] * 1000 + currentDigit[1] * 100 + currentDigit[2] * 10 + currentDigit[3];
}

void success()
{
  display.setSegments(yes);
  delay(2000);
  display.showNumberDecEx(getDigits(), 0, true, 4, 0); // Use getDigits to get current digits, then display them
}

void failure()
{
  display.setSegments(wrong);
  delay(2000);
  display.showNumberDecEx(getDigits(), 0, true, 4, 0);
}

void reinitialize()
{
  digitIndex = 3;
  for (int i = 0; i < 4; i++)
  {
    currentDigit[i] = 0;
  }
  digitValue = 0;
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

void handleButtonC()
{
  int digits = getDigits();
  Serial.print("Confirming: ");
  Serial.print(code[codeStage]);
  Serial.print(" ?= ");
  Serial.println(digits);
  if (code[codeStage] == digits)
  {              // Check if the correct code for the current codeStage is being displayed
    codeStage++; // If so, advance the stage
    success();
    Serial.print("Yes! New stage:");
    Serial.println(codeStage);
    Serial.print("Next answer: ");
    Serial.println(code[codeStage]);
    reinitialize();
    updateAll();
    if (codeStage == levels) // If that was the last stage, turn every segment on
    {
      display.setSegments(allON);
      laserSound();
      codeStage = 0;
    }
  }
  else
  {
    Serial.println("No");
    failure();
  }
}

void setup()
{
  Serial.begin(115200);
  display.setBrightness(7); // Adjust brightness (0-7)

  // Clear the display
  display.clear();

  pinMode(BUTTON_A_PIN, INPUT);
  pinMode(BUTTON_B_PIN, INPUT);
  pinMode(BUTTON_C_PIN, INPUT);
  pinMode(SOUND_PIN, OUTPUT);
}

void loop()
{
  yield();
  blinkNow++;
  if (blinkNow == 40000)
  {
    display.setSegments(blank, 1, digitIndex);
  }
  if (blinkNow == 50000)
  {
    blinkNow = 0;
    int tempIndex = digitIndex;
    refreshAll();
    digitIndex = tempIndex;
  }
  if (digitalRead(BUTTON_A_PIN) == HIGH)
  {
    handleButtonA();
    delay(200); // Debounce delay
  }

  // Check if button B is pressed
  if (digitalRead(BUTTON_B_PIN) == HIGH)
  {
    handleButtonB();
    delay(200); // Debounce delay
  }

  // Check if button B is pressed
  if (digitalRead(BUTTON_C_PIN) == HIGH)
  {
    handleButtonC();
    delay(200); // Debounce delay
  }
}