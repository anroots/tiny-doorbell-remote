#!/usr/bin/env python

import RPi.GPIO as GPIO
from time import sleep

GPIO.setmode(GPIO.BCM)
GPIO.setup(21, GPIO.OUT)

GPIO.output(21, GPIO.LOW)
sleep(6)
GPIO.output(21, GPIO.HIGH)

GPIO.cleanup()