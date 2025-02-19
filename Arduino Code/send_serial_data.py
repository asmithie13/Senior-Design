#Imports:
import serial
import time

#Initialize Serial object to connect to HC-05 Bluetooth module:
bluetoothObject=serial.Serial("/dev/rfcomm0", 9600, timeout=10) #Ensure that the HC-05 module is connected to "rfcomm0"

#Uncomment the line below, and comment-out line 6 if testing with Windows device instead of Pi:
#bluetoothObject=serial.Serial("COM4", 9600, timeout=1) 

#Ensure that the connection is established:
time.sleep(2)

#Send the data:
tempData="A1"
bluetoothObject.write(tempData.encode())

'''Device 98:D3:71:F7:0D:84 Name: HC-05
[CHG] Device 98:D3:71:F7:0D:84 Alias: HC-05'''