//Main program for executing voice-controlled and manually-controlled...
//...checkers on the Arduino:

//Include statements:
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <DFRobotDFPlayerMini.h>
#include <MaxMatrix.h>
#include <Adafruit_NeoPixel.h>

//Struct to declare players:
struct Player{
  int playerNum;
};

//Declare a player-in-turn object:
Player playerInTurn;

//Create the screen object:
int numRows=4;
int numCols=20;

//LCD for player one:
LiquidCrystal_I2C lcd1(0x27, numCols, numRows); //Initialize the LCD screen

//LCD for player two:
LiquidCrystal_I2C lcd2(0x26, numCols, numRows); //Initialize the LCD screen

//Initialize data for keypad entry:
const byte rowNum=4; 
const byte colNum=4; 
char hexaKeys[rowNum][colNum]={
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

//Pin-out for keypad entry:
byte rowPins[rowNum]={22, 24, 26, 28};
byte colPins[colNum]={30, 32, 34, 36};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, rowNum, colNum); 

//Declare an object to control the DFPlayer:
DFRobotDFPlayerMini myDFPlayer;

//Array that indicates where the audio file is located for each coordinate value:
int audioArray[10]={1, 14, 4, 5, 17, 18, 6, 7, 21, 22};

//This is an integer-based map of the board. If a checker belongs to Player #1, a '1' occupies the space. For Player #2, a "2" occupies...
//...the space. If there is no checker in the space, a '0' is at the coordinate.
int checkerBoard[8][8];

//Selected checker:
int selectedChecker[2]={NULL, NULL};

//Initialize scores:
int playerOneScore=0;
int playerTwoScore=0;

//Flag to determine if the reset signal was received when playing in manual mode:
bool resetSig;

//Initialize two arrays that hold the coordinates that must be called...
//...to set the checker at each corresponding index to a certain color:
String redArray[8][8]{
  {"AA", "10", "AA", "30", "AA", "50", "AA", "70"},
  {"00", "AA", "20", "AA", "40", "AA", "60", "AA"},
  {"AA", "12", "AA", "32", "AA", "52", "AA", "72"},
  {"02", "AA", "22", "AA", "42", "AA", "62", "AA"},
  {"AA", "14", "AA", "34", "AA", "54", "AA", "74"},
  {"04", "AA", "24", "AA", "44", "AA", "64", "AA"},
  {"AA", "16", "AA", "36", "AA", "56", "AA", "76"},
  {"06", "AA", "26", "AA", "46", "AA", "66", "AA"}
};
String blueArray[8][8]{
  {"AA", "11", "AA", "31", "AA", "51", "AA", "71"},
  {"01", "AA", "21", "AA", "41", "AA", "61", "AA"},
  {"AA", "13", "AA", "33", "AA", "53", "AA", "73"},
  {"03", "AA", "23", "AA", "43", "AA", "63", "AA"},
  {"AA", "15", "AA", "35", "AA", "55", "AA", "75"},
  {"05", "AA", "25", "AA", "45", "AA", "65", "AA"},
  {"AA", "17", "AA", "37", "AA", "57", "AA", "77"},
  {"07", "AA", "27", "AA", "47", "AA", "67", "AA"}
};

//Initialize the matrix:
MaxMatrix matrix(11, 10, 13, 1);

//Initialize the LED matrix:
Adafruit_NeoPixel strip(64, 6, NEO_GRB + NEO_KHZ800);

//Array to accurately index the LED matrix:
int LEDMatrix[8][8]={
  {63, 62, 61, 60, 59, 58, 57, 56},
  {48, 49, 50, 51, 52, 53, 54, 55},
  {47, 46, 45, 44, 43, 42, 41, 40},
  {32, 33, 34, 35, 36, 37, 38, 39},
  {31, 30, 29, 28, 27, 26, 25, 24},
  {16, 17, 18, 19, 20, 21, 22, 23},
  {15, 14, 13, 12, 11, 10, 9, 8},
  {0, 1, 2, 3, 4, 5, 6, 7}
};

//Function to update LED board based on checker positions:
void updateBoardLEDs(){
  //Clear the matrix:
  matrix.clear();

  //Iterate through the entire board, setting each piece correctly on the board:
  for(int i=0; i<8; i++){
    for(int j=0; j<8; j++){
      if(redArray[i][j]=="AA"){
        continue;
      }

      //Extract the coordinates from each array:
      int redCoordOne, redCoordTwo, blueCoordOne, blueCoordTwo;
      redCoordOne=(redArray[i][j])[0]-'0';
      redCoordTwo=(redArray[i][j])[1]-'0';
      blueCoordOne=(blueArray[i][j])[0]-'0';
      blueCoordTwo=(blueArray[i][j])[1]-'0';
      
      //Set the lights the appropriate color:
      if(checkerBoard[i][j]==1 || checkerBoard[i][j]==3){
        matrix.setDot(redCoordOne, redCoordTwo, 0xFF);
      }
      else if(checkerBoard[i][j]==2 || checkerBoard[i][j]==4){
        matrix.setDot(blueCoordOne, blueCoordTwo, 0xFF);
      }
    }
  }

  //Clear all LEDs:
  strip.fill(strip.Color(0, 0, 0));

  //Set the correct LEDs:
  for(int i=0; i<8; i++){
    for(int j=0; j<8; j++){
      //Turn spaces off that hold no checkers:
      if(checkerBoard[i][j]==0){
        strip.setPixelColor(LEDMatrix[i][j], strip.Color(0, 0, 0));
      }
      //Set red checkers:
      else if(checkerBoard[i][j]==1){
        strip.setPixelColor(LEDMatrix[i][j], strip.Color(10, 0, 0));
      }
      else if(checkerBoard[i][j]==3){
        strip.setPixelColor(LEDMatrix[i][j], strip.Color(80, 0, 0));
      }
      //Set blue checkers:
      else if(checkerBoard[i][j]==2){
        strip.setPixelColor(LEDMatrix[i][j], strip.Color(0, 0, 10));
      }
      else if(checkerBoard[i][j]==4){
        strip.setPixelColor(LEDMatrix[i][j], strip.Color(0, 0, 80));
      }
    }
  }

  //Update the strip:
  strip.show();
}

//Wait for user keypad input:
char pollForSelection(){
  char customKey=customKeypad.getKey();
  while(customKey==NO_KEY){
    customKey=customKeypad.getKey();
  }
  
  //If the custom key is the reset character, mark the reset flag:
  if(customKey=='*'){
    resetSig=true;
  }
  return customKey;
}

//Initialize the board:
void initializeBoard(){
  //Initialize the checkers with all pieces in the correct position:
  for(int i=0; i<8; i++){
    for(int j=0; j<8; j++){
      int tempRemainder=(i+j)%2;
      if(tempRemainder==1){
        if(i==0 || i==1 || i==2){
          checkerBoard[i][j]=1;
        }
        else if(i==3 || i==4){
          checkerBoard[i][j]=0;
        }
        else{
          checkerBoard[i][j]=2;
        }
      }
      else{
        checkerBoard[i][j]=0;
      }
    }
  }
}

//Function to display winners, and play win sequence:
void winSequence(){
  //Clear both screens:
  lcd1.clear();
  lcd2.clear();
  lcd1.setCursor(0, 0);
  lcd2.setCursor(0, 0);
  lcd1.print("Game over!");
  lcd2.print("Game over!");
  lcd1.setCursor(0, 1);
  lcd2.setCursor(0, 1);

  //DFPlayer:
  myDFPlayer.play(16);
  delay(1500);

  //Print winner:
  if(playerOneScore>playerTwoScore){
    lcd1.print("Player #1 wins!");
    lcd2.print("Player #1 wins!");

    //DFPlayer:
    myDFPlayer.play(13);
  }
  else if(playerOneScore<playerTwoScore){
    lcd1.print("Player #2 wins!");
    lcd2.print("Player #2 wins!");

    //DFPlayer:
    myDFPlayer.play(12);
  }
  else{
    lcd1.print("Tie game!");
    lcd2.print("Tie game!");

    //DFPlayer:
    myDFPlayer.play(20);
  }

  //[ADD LED PATTERN THAT INDICATES WIN]

  //Add short delay:
  delay(2000);
}

//Receive coordinates, or reset signal, from the Raspberry Pi:
int* getPiCoordinates(int* coordArray) {
  //Initialize a variable to count the serial data received:
  int i=0;

  //Wait for serial data to be sent from the Raspberry Pi:
  while(1){
    //If serial data has been received:
    if(Serial1.available()){
      //Iterate through data until all five characters are gathered:
      char tempChar=Serial1.read();
      coordArray[i]=tempChar-'0';

      //Increment the counter:
      i++;

      //Print the number of coordinates received:
      Serial.print("\nValues received: ");
      Serial.print(i);

      //If all five characters for a move are received, break from the loop:
      if(i==5){
        break;
      }
      
    }
  }
}

//Function to execute the voice-controlled game mode:
void voiceControlledGame(){
  //Initialize selected checker and move-spaces:
  int selectedChecker[2];
  int moveSpace[2];

  while(1){
    //Initialize the screens:
    lcd1.clear();
    lcd2.clear();
    lcd1.setCursor(0, 0);
    lcd2.setCursor(0, 0);

    //Display messages according to the player in-turn:
    if(playerInTurn.playerNum==1){
      lcd1.print("Your turn!");
      lcd2.print("Waiting...");

      //DFPlayer:
      myDFPlayer.play(3);
    }
    else{
      lcd2.print("Your turn!");
      lcd1.print("Waiting...");

      //DFPlayer:
      myDFPlayer.play(2);
    }
    //Get coordinates from the Raspberry Pi:
    int coordArray[4];
    getPiCoordinates(coordArray);

    //Check for the reset signal:
    if(resetSig==true){
      break;
    }
   
    //Assign values based on coordinates received:
    selectedChecker[0]=coordArray[0];
    selectedChecker[1]=coordArray[1];
    moveSpace[0]=coordArray[2];
    moveSpace[1]=coordArray[3];

    //If a checker was overtaken, increment the score, and set that checker to zero:
    if(abs(selectedChecker[0]-moveSpace[0])==2){
      if(playerInTurn.playerNum==1){
        playerOneScore++;
      }
      else{
        playerTwoScore++;
      }
    }

    //Make the case for each way that the checker could have overtaken:
    if(selectedChecker[0]-moveSpace[0]>0 && selectedChecker[1]-moveSpace[1]>0){ //Up-left
      checkerBoard[selectedChecker[0]-1][selectedChecker[1]-1]=0;
    }
    else if(selectedChecker[0]-moveSpace[0]<0 && selectedChecker[1]-moveSpace[1]>0){ //Down-left
      checkerBoard[selectedChecker[0]+1][selectedChecker[1]-1]=0;
    }
    else if(selectedChecker[0]-moveSpace[0]>0 && selectedChecker[1]-moveSpace[1]<0){ //Up-right
      checkerBoard[selectedChecker[0]-1][selectedChecker[1]+1]=0;
    }
    else if(selectedChecker[0]-moveSpace[0]<0 && selectedChecker[1]-moveSpace[1]<0){ //Down-right
      checkerBoard[selectedChecker[0]+1][selectedChecker[1]+1]=0;
    }

    //Shut-off the selected checker space:
    checkerBoard[selectedChecker[0]][selectedChecker[1]]=0;
    
    //Light-up the new space accordingly, and alternate the player in-turn:
    if(playerInTurn.playerNum==1){
      //Set the move space:
      checkerBoard[moveSpace[0]][moveSpace[1]]=1;

      //Display move message:
      lcd1.clear();
      lcd1.setCursor(0, 0);
      lcd1.print("Selected: (");
      lcd1.print(selectedChecker[0]);
      lcd1.print(", ");
      lcd1.print(selectedChecker[1]);
      lcd1.print(")");
      lcd1.setCursor(0, 1);
      lcd1.print("Moved to: (");
      lcd1.print(moveSpace[0]);
      lcd1.print(", ");
      lcd1.print(moveSpace[1]);
      lcd1.print(")");
      
      //Alternate player, if no double-jump exists:
      if(coordArray[4]==0){
        playerInTurn.playerNum=2;
      }
    }
    else{
      Serial.print("SET PLAYER 2 CHIP");
      //Set the move space:
      checkerBoard[moveSpace[0]][moveSpace[1]]=2;

      //Display move message:
      lcd2.clear();
      lcd2.setCursor(0, 0);
      lcd2.print("Selected: (");
      lcd2.print(selectedChecker[0]);
      lcd2.print(", ");
      lcd2.print(selectedChecker[1]);
      lcd2.print(")");
      lcd2.setCursor(0, 1);
      lcd2.print("Moved to: (");
      lcd2.print(moveSpace[0]);
      lcd2.print(", ");
      lcd2.print(moveSpace[1]);
      lcd2.print(")");

      //Alternate player, if no double-jump exists:
      if(coordArray[4]==0){
        playerInTurn.playerNum=1;
      }
    }

    //DFPlayer:
    myDFPlayer.play(10);
    delay(1500);
    myDFPlayer.play(audioArray[selectedChecker[0]]);
    delay(1000);
    myDFPlayer.play(audioArray[selectedChecker[1]]);
    delay(1000);
    myDFPlayer.play(15);
    delay(1500);
    myDFPlayer.play(audioArray[moveSpace[0]]);
    delay(1000);
    myDFPlayer.play(audioArray[moveSpace[1]]);
    delay(1000);

    //Check to see if a king-space must be awarded:
    checkForKing();

    //Show the board configuration:
    testBoardConfig();
    
    //Update LED matrix:
    updateBoardLEDs();
  }
}

//Function for testing that prints the configuration of the board after each move:
void testBoardConfig(){
  Serial.print("\nBoard Configuration:");
  for(int i=0; i<8; i++){
    Serial.print("\n");
    for(int j=0; j<8; j++){
      Serial.print(checkerBoard[i][j]);
    }
  }

  //TEST:
  Serial.print("\nPlayer #1 Score: ");
  Serial.print(playerOneScore);
  Serial.print("\nPlayer #2 Score: ");
  Serial.print(playerTwoScore);
}

//Function that scans the board to check if any king pieces must be awarded:
void checkForKing(){
  for(int i=0; i<8; i++){
    //If one of Player #2's checkers is in the first row, it should be awarded king status:
    if(checkerBoard[0][i]==2){
      checkerBoard[0][i]+=2;
    }
    //If one of Player #1's checkers is in the last row, it should be awarded king status:
    else if(checkerBoard[7][i]==1){
      checkerBoard[7][i]+=2;
    }
  }
}

//Ensure that the checker is able to be selected:
void selectChecker(){
  //Declare variables to hold the checker's coordinates:
  char getCoordOne, getCoordTwo;
  int coordOne, coordTwo;

  //Flag to further signal if the move is valid:
  int validFlag=1;

  //Determines if this is the first iteration within the loop:
  int firstFlag=0;

  //If the input coordinate was invalid, ask again:
  while(validFlag>0 || (coordOne+coordTwo)%2==0){
    //If this is not the first iteration of the loop, it means that...
    //the first player input was invalid. Therefore, display both an audio and visual signal indicating this:
    if(firstFlag!=0){
      myDFPlayer.play(29);
      
      //Print the appropriate message according to the player in-turn:
      if(playerInTurn.playerNum==1){
        lcd1.clear();
        lcd1.print("Invalid!");
      }
      else{
        lcd2.clear();
        lcd2.print("Invalid!");
      }
      delay(1500);
    }

    //Increment the flag indicating whether or not the loop is in the first iteration:
    firstFlag++;

    //Print that the user must input additional values, since the first value was invalid. Either the...
    //...coordinate is not present on the board, or a checker is not at that position:

    //Print the initial message:
    if(playerInTurn.playerNum==1){
      lcd1.clear();
      lcd1.print("Selected: (_, _)");
    }
    else{
      lcd2.clear();
      lcd2.print("Selected: (_, _)");
    }

    //Get the first coordinate, as a character:
    getCoordOne=pollForSelection();

    //Check for reset:
    if(resetSig==true){
      return;
    }

    //Display the coordinate:
    if(playerInTurn.playerNum==1){
      lcd1.clear();
      lcd1.setCursor(0, 0);
      lcd1.print("Selected: (");
      lcd1.print(getCoordOne);
      lcd1.print(", _)");
    }
    else{
      lcd2.clear();
      lcd2.setCursor(0, 0);
      lcd2.print("Selected: (");
      lcd2.print(getCoordOne);
      lcd2.print(", _)");
    }

    //Play the corresponding sound:
    coordOne=getCoordOne-'0';
    if(getCoordOne=='#'){
      myDFPlayer.play(28);
    }
    else if(getCoordOne=='A'){
      myDFPlayer.play(23);
    }
    else if(getCoordOne=='B'){
      myDFPlayer.play(24);
    }
    else if(getCoordOne=='C'){
      myDFPlayer.play(25);
    }
    else if(getCoordOne=='D'){
      myDFPlayer.play(26);
    }
    else{
      myDFPlayer.play(audioArray[coordOne]);
    }

    //Repeat for acquiring the second coordinate:
    getCoordTwo=pollForSelection();
    //Check for reset:
    if(resetSig==true){
      return;
    }

    if(playerInTurn.playerNum==1){
      lcd1.clear();
      lcd1.setCursor(0, 0);
      lcd1.print("Selected: (");
      lcd1.print(getCoordOne);
      lcd1.print(", ");
      lcd1.print(getCoordTwo);
      lcd1.print(")");
    }
    else{
      lcd2.clear();
      lcd2.setCursor(0, 0);
      lcd2.print("Selected: (");
      lcd2.print(getCoordOne);
      lcd2.print(", ");
      lcd2.print(getCoordTwo);
      lcd2.print(")");
    }

    coordTwo=getCoordTwo-'0';
    if(getCoordTwo=='#'){
      myDFPlayer.play(28);
    }
    else if(getCoordTwo=='A'){
      myDFPlayer.play(23);
    }
    else if(getCoordTwo=='B'){
      myDFPlayer.play(24);
    }
    else if(getCoordTwo=='C'){
      myDFPlayer.play(25);
    }
    else if(getCoordTwo=='D'){
      myDFPlayer.play(26);
    }
    else{
      myDFPlayer.play(audioArray[coordTwo]);
    }
    delay(1000);

    //Coordinate exceeds board limits:
    if(coordOne<0 || coordOne>7 || coordTwo<0 || coordTwo>7){
      continue;
    }

    //Incorrect player selects piece:
    if((playerInTurn.playerNum==1 && checkerBoard[coordOne][coordTwo]!=1 && checkerBoard[coordOne][coordTwo]!=3) || (playerInTurn.playerNum==2 && checkerBoard[coordOne][coordTwo]!=2 && checkerBoard[coordOne][coordTwo]!=4)){
      continue;
    }
    
    //Depending on the type of checker piece, validation is executed differently:
    if(checkerBoard[coordOne][coordTwo]==1){
      validFlag=checkIfMovePlayerOne(coordOne, coordTwo);
    }
    else if(checkerBoard[coordOne][coordTwo]==2){
      validFlag=checkIfMovePlayerTwo(coordOne, coordTwo);
    }
    else{
      validFlag=checkIfMovePlayerOne(coordOne, coordTwo);
      validFlag+=checkIfMovePlayerTwo(coordOne, coordTwo);
      if(validFlag==1){
        validFlag=0;
      }
    }
  }
  
  //The selected checker is chosen, and begins to blink:
  selectedChecker[0]=coordOne;
  selectedChecker[1]=coordTwo;

  //TEST:
  Serial.print("\nThe selected checker is:\n");
  Serial.print(coordOne);
  Serial.print(coordTwo);
}

//Function to check for a valid move for Player #1:
int checkIfMovePlayerOne(int coordOne, int coordTwo){
  //Find if the move to be made would exceed board limits:
  int coordOneMin, coordOnePlus, coordTwoMin, coordTwoPlus;

  //Coordinate #1 cannot be decremented, because the piece may not move backwards:
  coordOnePlus=coordOne+1;
  coordTwoMin=coordTwo-1;
  coordTwoPlus=coordTwo+1;

  //Account for moves that skip over the opponent's checker:
  int coordOnePlusTwo=coordOne+2;
  int coordTwoMinTwo=coordTwo-2;
  int coordTwoPlusTwo=coordTwo+2;
  
  //Only enter this statement if the checker has possible moves to either side, and is not on the board's edge:
  if(coordOnePlus>=0 && coordOnePlus<8 && coordTwoMin>=0 && coordTwoMin<8 && coordTwoPlus>=0 && coordTwoPlus<8){
    //Now, see if there is a move available to skip over the opposing player's checker:
    int tempFlag=0;

    //Move #1: Skipping over the opposing player's checker with a piece not on an edge:
    //In order, the conditions are as follows:
    if(coordOnePlusTwo>7 || coordTwoMinTwo<0 || (checkerBoard[coordOnePlusTwo][coordTwoMinTwo]==0 && checkerBoard[coordOnePlus][coordTwoMin]!=2 && checkerBoard[coordOnePlus][coordTwoMin]!=4)  || checkerBoard[coordOnePlusTwo][coordTwoMinTwo]!=0){
      tempFlag++;
    }

    //Check to see if the move to the right would be valid:
    if(coordOnePlusTwo>7 || coordTwoPlusTwo>7 || (checkerBoard[coordOnePlusTwo][coordTwoPlusTwo]==0 && checkerBoard[coordOnePlus][coordTwoPlus]!=2 && checkerBoard[coordOnePlus][coordTwoPlus]!=4) || checkerBoard[coordOnePlusTwo][coordTwoPlusTwo]!=0){
      tempFlag++;
    }

    //Check to see if a move forward without skipping over the opponent's checker is available:
    if(checkerBoard[coordOnePlus][coordTwoMin]!=0 && checkerBoard[coordOnePlus][coordTwoPlus]!=0){
      tempFlag++;
    }

    //If all of the above moves are unavailable, the player may not move the piece:
    if(tempFlag==3){
      return 1;
    }
  }

  //The piece is on the left outer-edge, and the diagonal-right move is not available:
  if(coordTwoMin<0 && checkerBoard[coordOnePlus][coordTwoPlus]!=0 && ((checkerBoard[coordOnePlusTwo][coordTwoPlusTwo]==0 && checkerBoard[coordOnePlus][coordTwoPlus]!=2 && checkerBoard[coordOnePlus][coordTwoPlus]!=4) || checkerBoard[coordOnePlusTwo][coordTwoPlusTwo]!=0)){
    return 1;
  }

  //The piece is on the right outer-edge, and the diagonal-left move is not available:
  if((coordTwoPlus>7 && checkerBoard[coordOnePlus][coordTwoMin]!=0) && ((checkerBoard[coordOnePlusTwo][coordTwoMinTwo]==0 && checkerBoard[coordOnePlus][coordTwoMin]!=2 && checkerBoard[coordOnePlus][coordTwoMin]!=4) || checkerBoard[coordOnePlusTwo][coordTwoMinTwo]!=0)){
    return 1;
  }

  return 0;
}

//Function to check for a valid move for Player #2:
int checkIfMovePlayerTwo(int coordOne, int coordTwo){
  //Find if the move to be made would exceed board limits:
  int coordOneMin, coordOnePlus, coordTwoMin, coordTwoPlus;

  //Coordinate #1 cannot be incremented, because the piece may not move backwards:
  coordOneMin=coordOne-1;
  coordTwoMin=coordTwo-1;
  coordTwoPlus=coordTwo+1;

  //Account for moves that skip over the opponent's checker:
  int coordOneMinTwo=coordOne-2;
  int coordTwoMinTwo=coordTwo-2;
  int coordTwoPlusTwo=coordTwo+2;

  //Only enter this statement if the checker has possible moves to either side, and is not on the board's edge:
  if(coordOneMin>=0 && coordOneMin<8 && coordTwoMin>=0 && coordTwoMin<8 && coordTwoPlus>=0 && coordTwoPlus<8){
    //Now, see if there is a move available to skip over the opposing player's checker:
    int tempFlag=0;

    //Check to see if the move to the left would be valid:
    if(coordOneMinTwo>7 || coordTwoMinTwo<0 || (checkerBoard[coordOneMinTwo][coordTwoMinTwo]==0 && checkerBoard[coordOneMin][coordTwoMin]!=1 && checkerBoard[coordOneMin][coordTwoMin]!=3)  || checkerBoard[coordOneMinTwo][coordTwoMinTwo]!=0){
      tempFlag++;
    }

    //Check to see if the move to the right would be valid:
    if(coordOneMinTwo>7 || coordTwoPlusTwo>7 || (checkerBoard[coordOneMinTwo][coordTwoPlusTwo]==0 && checkerBoard[coordOneMin][coordTwoPlus]!=1 && checkerBoard[coordOneMin][coordTwoPlus]!=3) || checkerBoard[coordOneMinTwo][coordTwoPlusTwo]!=0){
      tempFlag++;
    }

    //Check to see if a move forward without skipping over the opponent's checker is available:
    if(checkerBoard[coordOneMin][coordTwoMin]!=0 && checkerBoard[coordOneMin][coordTwoPlus]!=0){
      tempFlag++;
    }

    //If all of the above moves are unavailable, the player may not move the piece:
    if(tempFlag==3){
      return 1;
    }
  }

  //The piece is on the left outer-edge, and the diagonal-right move is not available:
  if(coordTwoMin<0 && checkerBoard[coordOneMin][coordTwoPlus]!=0 && ((checkerBoard[coordOneMinTwo][coordTwoPlusTwo]==0 && checkerBoard[coordOneMin][coordTwoPlus]!=1 && checkerBoard[coordOneMin][coordTwoPlus]!=3) || checkerBoard[coordOneMinTwo][coordTwoPlusTwo]!=0)){
    return 1;
  }

  //The piece is on the right outer-edge, and the diagonal-left move is not available:
  if((coordTwoPlus>7 && checkerBoard[coordOneMin][coordTwoMin]!=0) && ((checkerBoard[coordOneMinTwo][coordTwoMinTwo]==0 && checkerBoard[coordOneMin][coordTwoMin]!=1 && checkerBoard[coordOneMin][coordTwoMin]!=3) || checkerBoard[coordOneMinTwo][coordTwoMinTwo]!=0)){
    return 1;
  }

  return 0;
}

//Function to increment the player's score:
void incrementScore(){
  if(playerInTurn.playerNum==1){
    playerOneScore+=1;
  }
  else{
    playerTwoScore+=1;
  }
}

//Move conditions for Player #1:
int moveConditionsPlayerOne(int* neededPlayer, int coordOne, int coordTwo){
  //Flag to be returned to indicate if the move is valid:
  int retFlag=0;

  //Flag to indicate that the score should be incremented:
  int scoreFlag=0;

  //If the move involves the checker moving forward to either side, and the space is empty, allow the move:
  if(coordOne==selectedChecker[0]+1 && (coordTwo==selectedChecker[1]-1 || coordTwo==selectedChecker[1]+1) && checkerBoard[coordOne][coordTwo]==0){
    retFlag=1;
  }
  //If the move involves skipping the opponent's checker, and the space is empty, allow the move:
  if(coordOne==selectedChecker[0]+2 && (coordTwo==selectedChecker[1]-2 || coordTwo==selectedChecker[1]+2) && checkerBoard[coordOne][coordTwo]==0){
    //For a successful skip to occur, the checker being skipped must belong to the opposing player:
    if(coordTwo==selectedChecker[1]-2 && (checkerBoard[coordOne-1][coordTwo+1]==neededPlayer[0] || checkerBoard[coordOne-1][coordTwo+1]==neededPlayer[1])){
      //If valid, remove that chip from the board, and increment the current player's score:
      checkerBoard[coordOne-1][coordTwo+1]=0;
      //Increment the score, since a checker was removed:
      scoreFlag=1;
      retFlag=1;
    }
    else if(coordTwo==selectedChecker[1]+2 && (checkerBoard[coordOne-1][coordTwo-1]==neededPlayer[0] || checkerBoard[coordOne-1][coordTwo-1]==neededPlayer[1])){
      checkerBoard[coordOne-1][coordTwo-1]=0;
      //Increment the score, since a checker was removed:
      scoreFlag=1;
      retFlag=1;
    }
  }
  
  if(scoreFlag==1){
    incrementScore();
  }

  //Return the return flag:
  return retFlag;
}

//Move conditions for Player #2:
int moveConditionsPlayerTwo(int* neededPlayer, int coordOne, int coordTwo){
  //Flag to be returned to indicate if the move is valid:
  int retFlag=0;

  //Flag to indicate that the score should be incremented:
  int scoreFlag=0;

  //If the move involves the checker moving forward to either side, and the space is empty, allow the move:
  if(coordOne==selectedChecker[0]-1 && (coordTwo==selectedChecker[1]-1 || coordTwo==selectedChecker[1]+1) && checkerBoard[coordOne][coordTwo]==0){
    retFlag=1;
  }
  //If the move involves skipping the opponent's checker, and the space is empty, allow the move:
  if(coordOne==selectedChecker[0]-2 && (coordTwo==selectedChecker[1]-2 || coordTwo==selectedChecker[1]+2) && checkerBoard[coordOne][coordTwo]==0){
    //For a successful skip to occur, the checker being skipped must belong to the opposing player:
    if(coordTwo==selectedChecker[1]-2 && (checkerBoard[coordOne+1][coordTwo+1]==neededPlayer[0] || checkerBoard[coordOne+1][coordTwo+1]==neededPlayer[1])){
      //If valid, remove that chip from the board, and increment the current player's score:
      checkerBoard[coordOne+1][coordTwo+1]=0;
      //Increment the score, since a checker was removed:
      scoreFlag=1;
      retFlag=1;
    }
    else if(coordTwo==selectedChecker[1]+2 && (checkerBoard[coordOne+1][coordTwo-1]==neededPlayer[0] || checkerBoard[coordOne+1][coordTwo-1]==neededPlayer[1])){
      checkerBoard[coordOne+1][coordTwo-1]=0;
      //Increment the score, since a checker was removed:
      scoreFlag=1;
      retFlag=1;
    }
  }
  
  if(scoreFlag==1){
    incrementScore();
  }
  
  //Return the return flag:
  return retFlag;
}

//Select the space to move the checker to:
void moveChecker(){
  //Variables to hold each coordinate:
  char getCoordOne, getCoordTwo;
  int coordOne, coordTwo;

  //Flag to determine if the move is valid:
  int validFlag=0;

  //Player needed for the skip to occur (this includes king-assigned pieces):
  int neededPlayer[2];
  if(playerInTurn.playerNum==1 || playerInTurn.playerNum==3){
    neededPlayer[0]=2;
    neededPlayer[1]=4;
  }
  else{
    neededPlayer[0]=1;
    neededPlayer[1]=3;
  }

  //Flag that indicates whether the loop is in the first iteration:
  int firstFlag=0;

  //If the input coordinate was invalid, ask again:
  while(coordOne<0 || coordOne>7 || coordTwo<0 || coordTwo>7 || (coordOne+coordTwo)%2==0 || checkerBoard[coordOne][coordTwo]!=0 || validFlag==0){
    //If this is not the first iteration of the loop, it means that the user's first input...
    //...was invalid. Display a message indicating this:
    if(firstFlag!=0){
      myDFPlayer.play(29);

      //Display message to appropriate screen:
      if(playerInTurn.playerNum==1){
        lcd1.clear();
        lcd1.print("Invalid!");
      }
      else{
        lcd2.clear();
        lcd2.print("Invalid!");
      }
      delay(1500);
    }

    //Increment the flag to show that the loop is not in the first iteration:
    firstFlag++;

    //Print that the user must input additional values, since the first value was invalid. Either the...
    //...coordinate is not present on the board, or a checker is not at that position:

    //Display the initial message:
    if(playerInTurn.playerNum==1){
      lcd1.clear();
      lcd1.print("Moved to: (_, _)");
    }
    else{
      lcd2.clear();
      lcd2.print("Moved to: (_, _)");
    }

    //Get the first coordinate, as a character:
    getCoordOne=pollForSelection();

    //Check for reset:
    if(resetSig==true){
      return;
    }

    //Update the display with information about the first coordinate:
    if(playerInTurn.playerNum==1){
      lcd1.clear();
      lcd1.setCursor(0, 0);
      lcd1.print("Moved to: (");
      lcd1.print(getCoordOne);
      lcd1.print(", _)");
    }
    else{
      lcd2.clear();
      lcd2.setCursor(0, 0);
      lcd2.print("Moved to: (");
      lcd2.print(getCoordOne);
      lcd2.print(", _)");
    }

    //Play the corresponding sound for the coordinate:
    coordOne=getCoordOne-'0';
    if(getCoordOne=='#'){
      myDFPlayer.play(28);
    }
    else if(getCoordOne=='A'){
      myDFPlayer.play(23);
    }
    else if(getCoordOne=='B'){
      myDFPlayer.play(24);
    }
    else if(getCoordOne=='C'){
      myDFPlayer.play(25);
    }
    else if(getCoordOne=='D'){
      myDFPlayer.play(26);
    }
    else{
      myDFPlayer.play(audioArray[coordOne]);
    }

    //Repeat for second coordinate:
    getCoordTwo=pollForSelection();
    //Check for reset:
    if(resetSig==true){
      return;
    }

    if(playerInTurn.playerNum==1){
      lcd1.clear();
      lcd1.setCursor(0, 0);
      lcd1.print("Moved to: (");
      lcd1.print(getCoordOne);
      lcd1.print(", ");
      lcd1.print(getCoordTwo);
      lcd1.print(")");
    }
    else{
      lcd2.clear();
      lcd2.setCursor(0, 0);
      lcd2.print("Moved to: (");
      lcd2.print(getCoordOne);
      lcd2.print(", ");
      lcd2.print(getCoordTwo);
      lcd2.print(")");
    }

    coordTwo=getCoordTwo-'0';
    if(getCoordTwo=='#'){
      myDFPlayer.play(28);
    }
    else if(getCoordTwo=='A'){
      myDFPlayer.play(23);
    }
    else if(getCoordTwo=='B'){
      myDFPlayer.play(24);
    }
    else if(getCoordTwo=='C'){
      myDFPlayer.play(25);
    }
    else if(getCoordTwo=='D'){
      myDFPlayer.play(26);
    }
    else{
      myDFPlayer.play(audioArray[coordTwo]);
    }
    delay(1000);

    //First, ensure that the coordinates of the desired move are within the proper bounds:
    if(coordOne>=0 && coordOne<8 && coordTwo>=0 && coordTwo<8){

      //Establish move conditions for Player #1's checkers:
      if(checkerBoard[selectedChecker[0]][selectedChecker[1]]==1){
        if(moveConditionsPlayerOne(neededPlayer, coordOne, coordTwo)==1){
          break;
        };
      }
      //Establish move conditions for Player #2's checkers:
      else if(checkerBoard[selectedChecker[0]][selectedChecker[1]]==2){
        if(moveConditionsPlayerTwo(neededPlayer, coordOne, coordTwo)==1){
          break;
        };
      }

      //Establish move conditions for either players' king checkers:
      else{
        if(coordOne>selectedChecker[0]){
          if(moveConditionsPlayerOne(neededPlayer, coordOne, coordTwo)==1){
            break;
          };
        }
        else{
          if(moveConditionsPlayerTwo(neededPlayer, coordOne, coordTwo)==1){
            break;
          };
        }
      }

    }
  }
  
  //Set the new checker space:
  checkerBoard[coordOne][coordTwo]=checkerBoard[selectedChecker[0]][selectedChecker[1]];

  //Display the desired message based on the player in-turn:
  if(playerInTurn.playerNum==1){
    lcd1.clear();
    delay(300);
    lcd1.print("Moved to: (");
    lcd1.print(coordOne);
    lcd1.print(", ");
    lcd1.print(coordTwo);
    lcd1.print(")");
  }
  else{
    lcd2.clear();
    delay(300);
    lcd2.print("Moved to: (");
    lcd2.print(coordOne);
    lcd2.print(", ");
    lcd2.print(coordTwo);
    lcd2.print(")");
  }

  //DFPlayer:
  myDFPlayer.play(15);
  delay(1500);
  myDFPlayer.play(audioArray[coordOne]);
  delay(1000);
  myDFPlayer.play(audioArray[coordTwo]);

  //Set the spot where the selected checker was to an empty space:
  checkerBoard[selectedChecker[0]][selectedChecker[1]]=0;
  
  //"Reset" the selected checker variable:
  selectedChecker[0]=NULL;
  selectedChecker[1]=NULL;
}

//Check to see if the game is over, and award a winner:
bool checkForWinner(){
  //Variables to hold each player's checkers:
  int playerOneNum=0;
  int playerTwoNum=0;

  //Iterate through the board, and if the board only consists of one player's pieces, someone has won:
  for(int i=0; i<8; i++){
    for(int j=0; j<8; j++){
      if(checkerBoard[i][j]==1 || checkerBoard[i][j]==3){
        playerOneNum++;
      }
      else if(checkerBoard[i][j]==2 || checkerBoard[i][j]==4){
        playerTwoNum++;
      }
    }
  }

  //Declare the winner, if there is one:
  if(playerOneNum==0 || playerTwoNum==0){
    return true;
  }
  return false;
}

//Function to execute the manual game mode:
void manualGame(){
  while(1){
    //Clear both screens after a short delay:
    delay(1000);
    lcd1.clear();
    lcd2.clear();
    lcd1.setCursor(0, 0);
    lcd2.setCursor(0, 0);

    //Display messages based on the player in-turn:
    if(playerInTurn.playerNum==1){
      lcd1.print("Your turn!");
      lcd2.print("Waiting...");

      //DFPlayer:
      myDFPlayer.play(3);
    }
    else{
      lcd2.print("Your turn!");
      lcd1.print("Waiting...");
      //DFPlayer:
      myDFPlayer.play(2);
    }
  
    //Select the checker to be moved:
    selectChecker();

    //Check for the reset signal:
    if(resetSig==true){
      break;
    }

    //Display messages based on the player in-turn:
    if(playerInTurn.playerNum==1){
      lcd1.clear();
      delay(500);
      lcd1.print("Selected: (");
      lcd1.print(selectedChecker[0]);
      lcd1.print(", ");
      lcd1.print(selectedChecker[1]);
      lcd1.print(")");
    }
    else{
      lcd2.clear();
      delay(500);
      lcd2.print("Selected: (");
      lcd2.print(selectedChecker[0]);
      lcd2.print(", ");
      lcd2.print(selectedChecker[1]);
      lcd2.print(")");
    }

    //DFPlayer:
    myDFPlayer.play(10);
    delay(1500);
    myDFPlayer.play(audioArray[selectedChecker[0]]);
    delay(1000);
    myDFPlayer.play(audioArray[selectedChecker[1]]);

    //Move the selected checker:
    moveChecker();

    //Check for the reset signal:
    if(resetSig==true){
      break;
    }

    //Check after the move to see if any king pieces need to be set:
    checkForKing();

    //TEST:
    testBoardConfig();

    //Update LED matrix:
    updateBoardLEDs();

    //Change the player in-turn:
    if(playerInTurn.playerNum==1){
      playerInTurn.playerNum=2;
    }
    else if(playerInTurn.playerNum==2){
      playerInTurn.playerNum=1;
    }
  
    //Check to see if there is a winner:
    int isWinner=NULL;
    isWinner=checkForWinner();

    //If there is a winner, based on the score, play the win sequence, and break from the loop:
    if(isWinner==true){
      break;
    }

    //If there isn't a winner, update the LED board accordingly:
    updateBoardLEDs();
  }
}

//Stub game to test win sequences:
void stubManualMode(){
  while(1){
    //Set the game board so that Player #1 has no pieces left:
    //Initialize the checkers with all pieces in the correct position:
    for(int i=0; i<8; i++){
      for(int j=0; j<8; j++){
        checkerBoard[i][j]=1;
      }
    }

    //Set the player score, to expect a desired winner:
    playerTwoScore=1;

    //Check for the win sequence, and expect it to be activated:
    int isWinner=NULL;
    isWinner=checkForWinner();
    if(isWinner==true){
      break;
    }
  }
}

//Set-up the program:
void setup() {
  //TEST:
  Serial.begin(9600);

  //Initialize serial communication with the Raspberry Pi:
  Serial1.begin(9600);

  //Initialize serial communication with the DFPlayer:
  Serial2.begin(9600);
  if(!myDFPlayer.begin(Serial2)){ //Ensure that the DFPlayer is initialized
    while (true);
  }

  //Set the volume of the DFPlayer:
  myDFPlayer.volume(20);

  //Initialize LCD display:
  lcd1.init();
  lcd1.backlight();

  //Initialize the second display:
  lcd2.init();
  lcd2.backlight();

  //Initialize the MAX7219:
  matrix.init();
  matrix.setIntensity(8);  //Set brightness

  //Initialize the LED array:
  strip.begin();  
  strip.show(); 
}

//Begin the game:
void loop() {
  //Initialize the board (same for both game modes):
  initializeBoard();

  //Initialize the LED matrix:
  updateBoardLEDs();

  //TEST:
  testBoardConfig();
  
  //Update LED matrix:
  updateBoardLEDs();

  //Reset player scores:
  playerOneScore=0;
  playerTwoScore=0;

  //Ensure that displays are cleared prior to beginning:
  lcd1.clear();
  lcd2.clear();

  //Display initial messages on the screen to prompt the beginning of a new game:
  lcd1.setCursor(0, 0);
  lcd1.print("Voice-controlled:");
  lcd1.setCursor(0, 1);
  lcd1.print("Enter 'A'");
  lcd2.print("Manual: Enter 'B'");

  //Reset the reset signal flag:
  resetSig=false;

  //Get initial user input:
  char gameModeChar=pollForSelection();
  while(gameModeChar!='A' && gameModeChar!='B'){ //If an invalid character is selected, prompt the user again
    gameModeChar=pollForSelection();
    Serial.print(gameModeChar);
  }

  //Clear both LCD screens.
  lcd1.clear();
  lcd2.clear();
  lcd1.setCursor(0, 0);
  lcd2.setCursor(0, 0);

  //Initialize the first player as Player #1:
  playerInTurn.playerNum=1;

  if(gameModeChar=='A'){
    //DFPlayer:
    myDFPlayer.play(8);

    //Print messages to screens:
    lcd1.print("Selected:");
    lcd1.setCursor(0, 1);
    lcd1.print("Voice-Controlled");
    lcd2.print("Selected:");
    lcd2.setCursor(0, 1);
    lcd2.print("Voice-Controlled");
    delay(2000);

    //Print pre-game messages:
    lcd1.clear();
    lcd2.clear();
    lcd1.setCursor(0, 0);
    lcd2.setCursor(0, 0);

    //DFPlayer:
    myDFPlayer.play(5);
    lcd1.print("3");
    lcd2.print("3");
    delay(1000);
    lcd1.clear();
    lcd2.clear();

    //DFPlayer:
    myDFPlayer.play(4);
    lcd1.print("2");
    lcd2.print("2");
    delay(1000);
    lcd1.clear();
    lcd2.clear();

    //DFPlayer:
    myDFPlayer.play(14);
    lcd1.print("1");
    lcd2.print("1");
    delay(1000);
    lcd1.clear();
    lcd2.clear();

    //DFPlayer:
    myDFPlayer.play(19);
    lcd1.print("Begin!");
    lcd2.print("Begin!");
    delay(1500);

    //Enter the voice-controlled game:
    voiceControlledGame();
  }
  else if(gameModeChar=='B'){
    //DFPlayer:
    myDFPlayer.play(9);

    //Print messages to screens:
    lcd1.print("Selected:");
    lcd1.setCursor(0, 1);
    lcd1.print("Manual");
    lcd2.print("Selected:");
    lcd2.setCursor(0, 1);
    lcd2.print("Manual");
    delay(2000);

    //DFPlayer:
    myDFPlayer.play(30);

    //Print messages to screens:
    lcd1.clear();
    lcd1.setCursor(0, 0);
    lcd1.print("To end the game,");
    lcd1.setCursor(0, 1);
    lcd1.print("press '*'");
    lcd2.clear();
    lcd2.setCursor(0, 0);
    lcd2.print("To end the game,");
    lcd2.setCursor(0, 1);
    lcd2.print("press '*'");
    delay(4000);

    //Print pre-game messages:
    lcd1.clear();
    lcd2.clear();
    lcd1.setCursor(0, 0);
    lcd2.setCursor(0, 0);

    //DFPlayer:
    myDFPlayer.play(5);
    lcd1.print("3");
    lcd2.print("3");
    delay(1000);
    lcd1.clear();
    lcd2.clear();

    //DFPlayer:
    myDFPlayer.play(4);
    lcd1.print("2");
    lcd2.print("2");
    delay(1000);
    lcd1.clear();
    lcd2.clear();

    //DFPlayer:
    myDFPlayer.play(14);
    lcd1.print("1");
    lcd2.print("1");
    delay(1000);
    lcd1.clear();
    lcd2.clear();

    //DFPlayer:
    myDFPlayer.play(19);
    lcd1.print("Begin!");
    lcd2.print("Begin!");
    delay(1500);

    //Enter the manual game:
    manualGame();

    //TEST:
    //stubManualMode();
  }

  //After either game is over, play the win-sequence:
  winSequence();
}
