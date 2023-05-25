# Necessary to run:
# pip install evdev
# pip install RPi.GPIO

import sys
import evdev
import RPi.GPIO as GPIO

# Configure the GPIO pin for the diode
DIODE_PIN = 17
GPIO.setmode(GPIO.BCM)
GPIO.setup(DIODE_PIN, GPIO.OUT)

# Define the keyboard input code to trigger the diode
TARGET_CODE = "493759#"

# Initialize the input buffer
input_buffer = ""

# Function to handle keyboard events
def handle_keyboard_events():
    # Find the USB keyboard device
    devices = [evdev.InputDevice(fn) for fn in evdev.list_devices()]
    keyboard = None
    for device in devices:
        if "keyboard" in device.name.lower():
            keyboard = device
            break

    if not keyboard:
        print("USB keyboard not found.")
        sys.exit(1)

    print("Waiting for input...")
    for event in keyboard.read_loop():
        if event.type == evdev.ecodes.EV_KEY:
            key_event = evdev.categorize(event)
            if key_event.keystate == key_event.key_down:
                key_value = key_event.keycode.split("_")[-1]
                print(key_value)
                if key_value == "#":
                    # Check if the input code matches the target code
                    if input_buffer != TARGET_CODE:
                        # Turn off the diode and clear the input buffer
                        GPIO.output(DIODE_PIN, GPIO.LOW)
                        input_buffer = ""
                        print("Code cleared. Diode turned off.")
                    else:
                        # Clear the input buffer without turning off the diode
                        GPIO.output(DIODE_PIN, GPIO.HIGH)
                        input_buffer = ""
                        print("Success!")

                else:
                    # Append the pressed key to the input buffer
                    input_buffer += key_value

# Run the program
try:
    handle_keyboard_events()
except KeyboardInterrupt:
    # Clean up the GPIO pin on program exit
    GPIO.cleanup()
