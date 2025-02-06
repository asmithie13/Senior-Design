#Imports:
import serial

#Initialize Serial object to connect to HC-05 Bluetooth module:
bluetoothObject=serial.Serial('COM6', 9600, timeout=1) #Change COM depending on assigned port

#TEST:
tempData="A1"
bluetoothObject.write(tempData.encode())