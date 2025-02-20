#Imports:
import serial
import time

#Initialize Serial object:
ser=serial.Serial("/dev/ttyACM0", 9600, timeout=10) #Ensure that module is connected

#Uncomment the line below, and comment-out line 6 if testing with Windows device instead of Pi:
#ser=serial.Serial("COM4", 9600, timeout=1) 

#Ensure that the connection is established:
time.sleep(2)

#Send the data:
tempData="A1"
ser.write(tempData.encode())
