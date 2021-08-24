#!/usr/bin/env python3
#
# pip3 install crccheck

import sys
from crccheck.crc import Crc16Ibm3740

input = sys.argv[1]
print('input=\'{}\''.format(input))

# CRC-16/IBM-3740
crc = Crc16Ibm3740.calc(str.encode(input))
print('CRC-16/IBM-3740=0x{:X} ({})'.format(crc, crc))
