# -*- coding: utf-8 -*-
"""
Created on Thu Apr  7 20:58:19 2022

@author: quent
"""

import serial
from time import *

msg = 'ROOOOOOND'

ser = serial.Serial("/dev/ttyUSB0", baudrate=115200, timeout=2)

csq = 0

print('##### CHECK SIGNAL #####\n')

while(csq < 4):
    response = ''
    ser.write(b'ATI+CSQ\r\n')
    response = ser.read(1000).decode()
    while (response == ''):
        response = ser.read(1000).decode()
        sleep(0.1)

    while (response != ''):
        signal = response
        response = ser.read(1000).decode()
        sleep(0.1)
        
    if (len(signal) == 16):
        csq = int(signal[7])
    elif (len(signal) == 8):
        print("Failed to connect with IRIDIUM\nTrying again...")
    elif (len(signal) == 24):
        csq = int(signal[15])
    else:
        print(len(signal))
    print('CSQ = ' + str(csq))
    sleep(1)
    
    
    
# print('##### GET THE MODEM READY FOR A TEXT MESSAGE #####\n')
# ser.write(('AT+SBDWB=' + msg + '\r\n').encode())

# response = ser.read(1000).decode()
# while (response == ''):
#     response = ser.read(1000).decode()
#     sleep(0.01)
# while (response != ''):
#     print(response)
#     response = ser.read(1000).decode()
#     sleep(0.01)

print('##### GET INFO ABOUT THE SAT #####\n')
ser.write(b'AT+SBDGW\r\n')
response = ser.read(1000).decode()
r1 = ''
while (response == ''):
    response = ser.read(1000).decode()
    sleep(0.01)
r1 = response
while (response != ''):
    print(response)
    response = ser.read(1000).decode()
    sleep(0.01)

print('##### GET THE MODEM READY FOR A BYTE MESSAGE (checks) #####\n')
ser.write(b'AT+SBDWT\r\n')
response = ser.read(1000).decode()
r1 = ''
while (response == ''):
    response = ser.read(1000).decode()
    sleep(0.01)
r1 = response
while (response != ''):
    print(response)
    response = ser.read(1000).decode()
    sleep(0.01)

#if (r1 == '\r\nREADY\r\n'):
if (True):
    print('##### SEND MESSAGE TO MODEM #####\n')
    sleep(1)
    
    checksum = 0
    for letter in msg:
        checksum += ord(letter)
    checksum = hex(checksum)
    bits = checksum[2:5]
    
    ser.write((msg + '\r\n').encode())
    # ser.write(checksum.encode())
    response = ser.read(1000).decode()
    while (response == ''):
        response = ser.read(1000).decode()
        sleep(0.01)
    while (response != ''):
        print(response)
        response = ser.read(1000).decode()
        sleep(0.01)


# print('##### GET THE MODEM READY FOR A BYTE MESSAGE #####\n')
# ser.write(b'AT+SBDWB=' + str(len(msg.encode())).encode() + b'\r\n')
# response = ser.read(1000).decode()
# r1 = ''
# while (response == ''):
#     response = ser.read(1000).decode()
#     sleep(0.01)
# r1 = response
# while (response != ''):
#     print(response)
#     response = ser.read(1000).decode()
#     sleep(0.01)

# if (r1 == '\r\nREADY\r\n'):
#     print('##### SEND MESSAGE TO MODEM #####\n')
#     sleep(1)
    
#     checksum = 0
#     for letter in msg:
#         checksum += ord(letter)
#     checksum = hex(checksum)
#     bits = checksum[2:5]
    
#     ser.write(msg.encode())
#     ser.write(checksum.encode())
#     response = ser.read(1000).decode()
#     while (response == ''):
#         response = ser.read(1000).decode()
#         sleep(0.01)
#     while (response != ''):
#         print(response)
#         response = ser.read(1000).decode()
#         sleep(0.01)
    
    sleep(1)    
    print('##### FEEDBACK FROM THE SAT #####\n')    
    ser.write(b'AT+SBDI\r\n')
    response = ser.read(1000).decode()
    while (response == ''):
        response = ser.read(1000).decode()
        sleep(0.01)
    while (response != ''):
        print(response)
        response = ser.read(1000).decode()
        sleep(0.01)

ser.close()