'''
UART communication on Raspberry Pi using Pyhton
http://www.electronicwings.com
'''
import serial
from time import sleep

timeout = 0.5 #seconds; read() will stop after this duration, no matter the size of data read

def send_data(ser, bin_data):
    """
    ser: serial port, already opened with the serial.Serial function
    bin_data: binary data to send
    
    by erwan
    """
    ser.write("AT+SBDWB="+str(sizeof(bin_data))) #Write binary message to the ISU
    
    response = ser.read(64) #reads 64 bytes or until timeout
    
    if not 'READY' in response:
        return "Error: no READY response"
    
    ser.write(bin_data)
    
    response = ser.read(8) #reads 8 bytes or until timeout
    
    if not '0' in response:
        return "Error: no 0 response"
    
    ser.write("AT+SBDI") #Perform SBD session
    
    response = ser.read(64) #reads 64 bytes or until timeout
    
    if not 'SBDI' in response:
        return "Error: no +SBDI response"
    
    """
    TODO if needed:
        extract infos: from response (see format in documentation)
    """
    
    ser.write("AT+SBDD0") #Clear the MO message buffer
    
        response = ser.read(16) #reads 64 bytes or until timeout
    
    if not 'OK' in response:
        return "Error: no OK response"

'''
ser = serial.Serial ("/dev/ttyS0", 9600)    #Open port with baud rate
while True:
    received_data = ser.read()              #read serial port
    sleep(0.03)
    data_left = ser.inWaiting()             #check for remaining byte
    received_data += ser.read(data_left)
    print (received_data)                   #print received data
    ser.write(received_data)                #transmit data serially 
'''