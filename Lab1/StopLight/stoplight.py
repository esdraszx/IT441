import RPi.GPIO as GPIO

import time

print("stop light")

GPIO.setmode(GPIO.BCM)
GPIO.setup(23, GPIO.OUT)
GPIO.output(23, True)
time.sleep(2)

GPIO.output(23, False)
GPIO.setup(24, GPIO.OUT)
GPIO.output(24, True)
time.sleep(1)

GPIO.output(24, False)
GPIO.setup(25, GPIO.OUT)
GPIO.output(25, True)
time.sleep(2)

GPIO.cleanup()
