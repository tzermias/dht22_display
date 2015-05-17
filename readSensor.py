#!/usr/bin/env python

""" readSensor.py

Read sensor data from stdin and print them in RRD format
"""

import json
import sys
from time import time

def main():
    data = json.load(sys.stdin)
    print "%s:%s:%s:%s" % (int(time()), data['value']['temperature'],
    data['value']['humidity'], data['value']['heatIndex'])

if __name__ == '__main__':
    main()
