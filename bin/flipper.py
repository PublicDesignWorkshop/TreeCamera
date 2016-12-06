#!/usr/bin/python

import RPi.GPIO as GPIO
import os, time

GPIO.setmode(GPIO.BCM)
GPIO.setup(27, GPIO.OUT)
GPIO.output(27, GPIO.LOW)
