//Simple application to test the functionality of the I2C-LCD screen:
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Create the screen object:
int numRows=4;
int numCols=20;

//LCD for player one:
LiquidCrystal_I2C lcd1(0x27, numCols, numRows); //Initialize the LCD screen

//LCD for player two:
LiquidCrystal_I2C lcd2(0x26, numCols, numRows); //Initialize the LCD screen

void setup() {
  //Initialize LCD display:
  lcd1.init();
  lcd1.backlight();

  //Initialize the second display:
  lcd2.init();
  lcd2.backlight();
}

void loop() {
  char testTextOne[]="It is your turn, Player #1! Make your move!";
  char testTextTwo[]="It is your turn, Player #2! Make your move!";

  lcd2.print("Waiting for turn...");
  makeScroll(lcd1, 0, testTextOne);

  lcd1.clear();
  lcd2.clear();

  lcd1.print("Waiting for turn...");
  makeScroll(lcd2, 0, testTextTwo);

  lcd1.clear();
  lcd2.clear();
}

//Function to make the LCD screen text scroll:
void makeScroll(LiquidCrystal_I2C lcd, int lineNum, char *wordArg){
  //Declare an array to hold a portion of the argument word:
  char wordWindow[30];

  //If there is no need to "scroll", just print the whole word:
  if(strlen(wordArg)<=20){
    lcd.setCursor(0, lineNum);
    lcd.print(wordArg);
  }
  //Otherwise, scroll:
  else{
    //Initialize a pointer:
    int beginPointer=0;
    while(beginPointer+19<strlen(wordArg)){
      //Clear existing message:
      lcd.clear();

      //Copy twenty characters of the word to a window to be displayed:
      for(int i=0; i<20; i++){
        wordWindow[i]=wordArg[beginPointer+i];
      }

      lcd.setCursor(0, lineNum);
      lcd.print(wordWindow);

      //Temporarily hold at the beginning of the text before scrolling:
      if(beginPointer==0){
        delay(2000);
      }
      else{
        delay(500);
      }

      //Increment the pointer:
      beginPointer+=1;
    }
    //Temporarily hold at the end of the word:
    delay(2000);
  }
}
