//Simple application to get the move coordinates from the Raspberry Pi 4:

//Simple application to test the functionality of the I2C-LCD screen:
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Create the screen object:
int numRows=4;
int numCols=20;

//LCD for player one:
LiquidCrystal_I2C lcd1(0x26, numCols, numRows); //Initialize the LCD screen

//Initialize serial communication:
void setup() {
  //Set Baud rate:
  Serial.begin(9600);

  //Initialize LCD display:
  lcd1.init();
  lcd1.backlight();
}

//Check for serial input:
void loop() {
  int* piCoord=getPiCoordinates();
}

//Receive coordinates, or reset signal, from the Raspberry Pi:
int* getPiCoordinates() {
  //Initialize an array to hold the selected checker and move coordinates:
  int coordArray[4];

  //Declare variable to count how many characters have been received:
  int countVar=0;

  //Check if serial data is received:
  while(countVar<4){
    if(Serial.available()){
      char data=Serial.read(); //Read data, and convert it to an integer:
      lcd1.clear();
      lcd1.println(data);
      delay(500);

      //If the reset signal is received, break from the loop:
      if(data=='*'){
        lcd1.clear();
        lcd1.print("\nReset signal received.");
        delay(500);
        return;
      }

      //Otherwise, append the data to the return array, and increment the counter:
      coordArray[countVar]=data-'0';
      countVar++;
    }
  }

  //Print that necessary characters are recieved:
  lcd1.clear();
  lcd1.print("Done");
  delay(500);

  //Return the coordinate array:
  return coordArray; 
}

