# Necessary to run:
# pip install evdev
# pip install RPi.GPIO

import sys
import evdev
import RPi.GPIO as GPIO
from time import sleep
import pygame

# Configure the GPIO pin for the diode
DIODE_PIN = 4
SPEAKER_PIN = 17
GPIO.setmode(GPIO.BCM)
GPIO.setup(DIODE_PIN, GPIO.OUT, initial=GPIO.LOW)
GPIO.setup(SPEAKER_PIN, GPIO.OUT)  # Replace 17 with the GPIO pin connected to the key

def play_tone(frequency, duration):
    pwm = GPIO.PWM(SPEAKER_PIN, frequency)
    pwm.start(50)  # 50% duty cycle
    sleep(duration)
    pwm.stop()

def play_arpeggio(duration): # Plays C E G C G E C
    play_tone(261.63, duration/7)
    play_tone(329.63, duration/7)
    play_tone(392, duration/7)
    play_tone(523.25, duration/7)
    play_tone(392, duration/7)
    play_tone(329.63, duration/7)
    play_tone(261.63, duration/7)

def play_low_c(duration):
    play_tone(130.81, duration/3)
    play_tone(65.41, duration/3)
    play_tone(65.41, duration/3)

# Define the keyboard input code to trigger the diode
TARGET_CODE = "123425"

# Function to search for an item in a list
def search_item(item, my_list):
    for i in my_list:
        print(i)
        if isinstance(i, tuple) and i[0].startswith(item):
            return i
    return -1

# Function to handle keyboard4 events
def handle_keyboard_events():
    # Find the USB keyboard device
    devices = [evdev.InputDevice(fn) for fn in evdev.list_devices()]
    keyboard = None
    for device in devices:
        index = search_item('KEY_KP1', device.capabilities(verbose=True)[('EV_KEY',1)])
        if index != -1:
            keyboard = device
            break

    if not keyboard:
        print("USB keyboard not found.")
        sys.exit(1)

    print("Waiting for input...")
    input_buffer = ""
    for event in keyboard.read_loop():
        if event.type == evdev.ecodes.EV_KEY:
            key_event = evdev.categorize(event)
            if key_event.keystate == key_event.key_down:
                key_value = key_event.keycode.split("_")[-1]
                kp = key_value.startswith('KP')
                if (kp):
                    key_value = key_value[2:]
                print(key_value + " entered.")
                if key_value == "ENTER":
                    # Check if the input code matches the target code
                    if input_buffer != TARGET_CODE:
                        # Turn off the diode and clear the input buffer
                        GPIO.output(DIODE_PIN, GPIO.LOW)
                        play_low_c(1)
                        input_buffer = ""
                        print("Code cleared. Diode turned off.")
                    else:
                        # Clear the input buffer without turning off the diode
                        GPIO.output(DIODE_PIN, GPIO.HIGH)
                        play_arpeggio(3)
                        input_buffer = ""
                        print("Success!")
                        sleep(5)
                        GPIO.output(DIODE_PIN, GPIO.LOW)

                else:
                    # Append the pressed key to the input buffer
                    input_buffer += key_value
                    print("Buffer: " + input_buffer)


# Run the program
try:
    handle_keyboard_events()
except KeyboardInterrupt:
    # Clean up the GPIO pin on program exit
    GPIO.output(DIODE_PIN, GPIO.LOW)
    GPIO.cleanup()