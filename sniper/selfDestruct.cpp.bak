/**
 * @file selfDestruct.cpp
 * @author Alex Becker
 * @brief Remove the correct wire to begin self destruct!
 * @version 0.1
 * @date 2023-06-05
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

/*************************************
 * CUSTOMIZE THIS NUMBER TO SELECT THE CORRECT PIN FOR YOUR ANSWER
 */
static const int ANSWER = 5; // The pin that the correct wire is connected to
static const int WRONG = 12; // All incorrect pins connect here
static const int GREEN_LED = 13;
static const int RED_LED = 14;

/**
 * @brief PIN number of an LED.
 *
 * The default `2` is the blue LED on ESP-12E.
 */
static const int BUILTINLED = 2;

// Pin for speaker
#define SOUND_PIN 15

// Win condition
void success()
{
  for (int i = 0; i < 2; i++)
  {
    digitalWrite(GREEN_LED, HIGH);
    delay(1000);
    digitalWrite(GREEN_LED, LOW);
    delay(1000);
  }
  for (int i = 0; i < 4; i++)
  {
    digitalWrite(GREEN_LED, HIGH);
    delay(500);
    digitalWrite(GREEN_LED, LOW);
    delay(500);
  }
  for (int i = 0; i < 8; i++)
  {
    digitalWrite(GREEN_LED, HIGH);
    delay(250);
    digitalWrite(GREEN_LED, LOW);
    delay(250);
  }
  for (int i = 0; i < 32; i++)
  {
    digitalWrite(GREEN_LED, HIGH);
    delay(50);
    digitalWrite(GREEN_LED, LOW);
    delay(50);
  }
}

// Lose condition
void failure()
{
  digitalWrite(RED_LED, HIGH);
  delay(5000);
  digitalWrite(RED_LED, LOW);
}

void setup()
{
  Serial.begin(115200);
  pinMode(ANSWER, INPUT);
  pinMode(WRONG, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
}

void loop()
{
  yield();
  if (digitalRead(ANSWER) == LOW)
  {
    success();
  }
  if (digitalRead(WRONG) == LOW)
  {
    failure();
  }
}