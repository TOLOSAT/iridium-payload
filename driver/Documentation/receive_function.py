'''
UART communication on Raspberry Pi using Pyhton
http://www.electronicwings.com
'''
import serial
from time import sleep

timeout = 0.5 #seconds; read() will stop after this duration, no matter the size of data read

def receive_data(ser):
    """
    ser: serial port, already opened with the serial.Serial function
    bin_data: binary data to send
    
    by erwan
    """
    ser.write("AT+SBDREG?") #Query the Transceiver registration status

    
    response = ser.read(9) #reads 9 bytes or until timeout
    
    if not 'SBDREG:2' in response: #Transceiver is registered
        return "Error: no SBDREG:2 response"
    
    response = ser.read(16) #reads 16 bytes or until timeout
    
    if not 'SBDRING' in response: #Transceiver indicates an incoming message
        return "Error: no SBDRING response"
    
    ser.write("AT+SBDIXA") #Initiate SBD session in answer to automatic notification

    
    response = ser.read(64) #reads 64 bytes or until timeout
    
    if not 'SBDXAI' in response:
        return "Error: no +SBDXAI response"
    
    """
    TODO if needed:
        extract infos: from response (see format in documentation)
    """
    
    bin_data = ser.read(256) #reads 256 bytes or until timeout
    
    return bin_data

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