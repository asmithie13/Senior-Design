#Imports:
import serial
import time

#Initialize Serial object to connect to HC-05 Bluetooth module:
bluetoothObject=serial.Serial("/dev/rfcomm0", 9600, timeout=1) #Ensure that the HC-05 module is connected to "rfcomm0"

#Ensure that the connection is established:
time.sleep(2)

#Send the data:
tempData="A1"
bluetoothObject.write(tempData.encode())