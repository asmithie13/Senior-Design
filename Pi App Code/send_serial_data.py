#Imports:
import serial
import time

#Initialize Serial object to connect to HC-05 Bluetooth module:
#bluetoothObject=serial.Serial("/dev/rfcomm0", 9600, timeout=1) #Ensure that the HC-05 module is connected to "rfcomm0"

#Uncomment the line below, and comment-out line 6 if testing with Windows device instead of Pi:
bluetoothObject=serial.Serial("COM7", 9600, timeout=1) 

#Ensure that the connection is established:
time.sleep(2)

#Send the data:
tempData="2031"
bluetoothObject.write(tempData.encode())