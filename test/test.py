#!/bin/env python3

'''
 test.py
 2022 October 21

 Description:
  This is the python script that directly uses a command-line tool, spi-tools, to control the LTC2418 ADC.
  The reading will be written to a file determined by `filename`. There is a size limit of this. Once it reaches the limit, the first line of the file is deleted.

 Copyright 2022 Xiaosheng An

 Use of this source code is governed by an MIT-style
 license that can be found in the LICENSE file or at
 https://opensource.org/licenses/MIT.
'''

import subprocess
from subprocess import Popen, PIPE
from shlex import split
import sys
import time

filename = "testout.csv"
sizeLimit = 5000 # 5kb

sleepTime = 0.5 # seconds

address = [
        '10000', '11000', '10001', '11001', '10010', '11010', '10011', '11011', '10100', '11100', '10101', '11101', '10110', '11110', '10111', '11111'
        ]

def adc_read(channel : int):
    channel = int(channel)
    command = r'printf "\x{}\x00\x00\x00" | spi-pipe -d /dev/spidev1.0 -s 19200 -b 4 | xxd -ps'.format(hex(int('0b101{}'.format(address[channel]),2))[2:].upper())
    commands = command.split('|')
    print(command)

    def get_output():
        p1 = Popen(split(commands[0]), stdout=PIPE)
        p2 = Popen(split(commands[1]), stdin=p1.stdout, stdout=PIPE)
        _out = subprocess.check_output(split(commands[2]), stdin=p2.stdout)
        
        #for line in sys.stdin:
        #       _out = line
        
        return format(int(_out[:8], 16), '032b')

    out = get_output()

    while address[channel] != out[-6:-1]:
        print(f"expected: {address[channel]}, real: {out[-6:-1]}. Get the output in the next read...")
        time.sleep(0.17)
        out = get_output()

    out_str = "Channel {} (Parity {}): {}{}".format(address.index(out[-6:-1]), 'OK' if out.count('1') % 2 == 0 else 'FAIL' , '+' if out[2] is '1' else '-', int(out[3:26], 2))

    if out[-6:-1] == address[channel]:
        print(out_str)
        if out.count('1') % 2 == 0:
            with open(filename, "a") as f:
                f.write("{}{}\n".format('+' if out[2] is '1' else '-', int(out[3:26], 2)))
                if f.tell() > sizeLimit:
                    with open(filename, "r") as fr:
                        data = fr.read().splitlines(True)
                    with open(filename, "w") as fw:
                        fw.writelines(data[1:])

#
if __name__ == '__main__':
    if '--rotate' in sys.argv[1:]:
        _internal_condition = True
        while _internal_condition:
            _internal_condition = True if '--loop' in sys.argv[1:] else False
            for i in range(16):
                adc_read(i)
                time.sleep(sleepTime)
    elif '--loop' in sys.argv[1:]:
        while True:
            adc_read(sys.argv[1])
            time.sleep(sleepTime)
        pass
    else:
        adc_read(sys.argv[1])
