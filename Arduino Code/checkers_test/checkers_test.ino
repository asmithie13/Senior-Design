//Program to implement checkers logic (used within manual operation):
#include <Keypad.h>

//Struct to declare players:
struct Player{
  int playerNum;
};

//Establish the player in-turn:
Player playerInTurn;

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

//This is an integer-based map of the board. If a checker belongs to Player #1, a '1' occupies the space. For Player #2, a "2" occupies...
//...the space. If there is no checker in the space, a '0' is at the coordinate.
int checkerBoard[8][8];

//Selected checker:
int selectedChecker[2] = {NULL, NULL};

//Set-up the initial board, with each player's pieces:
void setup(){
  //TEST:
  Serial.begin(9600);

  //The first player in-turn is Player #1:
  playerInTurn.playerNum=1;

  //Initialize the checkers with all pieces in the correct position:
  for(int i=0; i<8; i++){
    for(int j=0; j<8; j++){
      int tempRemainder=(i+j)%2;
      if(tempRemainder==0){
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
  //Display the initial board configuration:
  testBoardConfig();
}

void loop(){
  //Select the checker to be moved:
  selectChecker();

  //Move the selected checker:
  moveChecker();

  //Change the player in-turn:
  if(playerInTurn.playerNum==1){
    playerInTurn.playerNum=2;
  }
  else if(playerInTurn.playerNum==2){
    playerInTurn.playerNum=1;
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
}

//Wait for user keypad input:
char pollForSelection(){
  char customKey=customKeypad.getKey();
  while(customKey==NULL){
    customKey=customKeypad.getKey();
  }
}

//Ensure that the checker is able to be selected:
void selectChecker(){
  //Declare variables to hold the checker's coordinates:
  int coordOne, coordTwo;

  //Flag to further signal if the move is valid:
  int validFlag=1;

  //If the input coordinate was invalid, ask again:
  while(validFlag==1 || (coordOne+coordTwo)%2==1 || checkerBoard[coordOne][coordTwo]!=playerInTurn.playerNum){
    //Print that the user must input additional values, since the first value was invalid. Either the...
    //...coordinate is not present on the board, or a checker is not at that position:
    coordOne=pollForSelection()-'0';
    coordTwo=pollForSelection()-'0';
    validFlag=checkIfMove(coordOne, coordTwo);
  }
  
  //The selected checker is chosen, and begins to blink:
  selectedChecker[0]=coordOne;
  selectedChecker[1]=coordTwo;

  //TEST:
  Serial.print("\nThe selected checker is:\n");
  Serial.print(coordOne);
  Serial.print(coordTwo);
}

//Function to check if the checker being selected has any available moves:
int checkIfMove(int coordOne, int coordTwo){
  //Coordinate exceeds board limits:
  if(coordOne<0 || coordOne>7 || coordTwo<0 || coordTwo>7){
    return 1;
  }

  //Find if the move to be made would exceed board limits:
  int coordOneMin, coordOnePlus, coordTwoMin, coordTwoPlus;
  coordOneMin=coordOne-1;
  coordOnePlus=coordOne+1;
  coordTwoMin=coordTwo-1;
  coordTwoPlus=coordTwo+1;

  if(coordOneMin>=0 && coordOneMin<8 && coordOnePlus>=0 && coordOnePlus<8 && coordTwoMin>=0 && coordTwoMin<8 && coordTwoPlus>=0 && coordTwoPlus<8){
    if(checkerBoard[coordOne-1][coordTwo-1]!=0 && checkerBoard[coordOne-1][coordTwo+1]!=0 && checkerBoard[coordOne+1][coordTwo-1]!=0 && checkerBoard[coordOne+1][coordTwo+1]!=0){
      Serial.print("1");
      return 1;
    }
  }

  if(coordOneMin<0 && coordTwoMin<0 && checkerBoard[coordOne+1][coordTwo+1]!=0){
    Serial.print("2");
    return 1;
  }

  if(coordOnePlus>7 && coordTwoPlus>7 && checkerBoard[coordOne-1][coordTwo-1]!=0){
    Serial.print("3");
    return 1;
  }

  if(coordOneMin<0 && checkerBoard[coordOne+1][coordTwo+1]!=0 && checkerBoard[coordOne+1][coordTwo-1]!=0){
    Serial.print("4");
    return 1;
  }

  if(coordOnePlus>7 && checkerBoard[coordOne-1][coordTwo+1]!=0 && checkerBoard[coordOne-1][coordTwo-1]!=0){
    Serial.print("5");
    return 1;
  }

  if(coordTwoMin<0 && checkerBoard[coordOne-1][coordTwo+1]!=0 && checkerBoard[coordOne+1][coordTwo+1]!=0){
    Serial.print("6");
    return 1;
  }

  if(coordTwoPlus>7 && checkerBoard[coordOne-1][coordTwo-1]!=0 && checkerBoard[coordOne+1][coordTwo-1]!=0){
    Serial.print("7");
    return 1;
  }

  return 0;
}

//Select the space to move the checker to:
void moveChecker(){
  //Get the coordinates of the checker to be moved, and convert these values to integers:
  int coordOne=pollForSelection()-'0';
  int coordTwo=pollForSelection()-'0';

  bool validFlag;
  if((coordOne==selectedChecker[0]-1 || coordOne==selectedChecker[0]+1) && (coordTwo==selectedChecker[0]-1 || coordTwo==selectedChecker[0]+1)){
    validFlag=true;
  }
  else{
    validFlag=false;
  }

  //If the input coordinate was invalid, ask again:
  while(coordOne<0 || coordOne>7 || coordTwo<0 || coordTwo>7 || (coordOne+coordTwo)%2==1 || checkerBoard[coordOne][coordTwo]!=0 || validFlag==false){
    //Print that the user must input additional values, since the first value was invalid. Either the...
    //...coordinate is not present on the board, or a checker is not at that position:
    coordOne=pollForSelection()-'0';
    coordTwo=pollForSelection()-'0';

    //Re-evaluate the "valid" flag:
    if((coordOne==selectedChecker[0]-1 || coordOne==selectedChecker[0]+1) && (coordTwo==selectedChecker[0]-1 || coordTwo==selectedChecker[0]+1)){
      validFlag=true;
    }
    else{
      validFlag=false;
    }
  }
  
  //Set the new checker space:
  if(playerInTurn.playerNum==1){
    checkerBoard[coordOne][coordTwo]=1;
  }
  else{
    checkerBoard[coordOne][coordTwo]=2;
  }
  //[ILLUMINATE ON BOARD]

  //TEST:
  Serial.print("\nThere is now a checker at:\n");
  Serial.print(coordOne);
  Serial.print(coordTwo);
  Serial.print("\n");
  Serial.print(checkerBoard[coordOne][coordTwo]);

  //Set the spot where the selected checker was to an empty space:
  checkerBoard[selectedChecker[0]][selectedChecker[1]]=0;
  //[SHUT-OFF SELECTED CHECKER]
  
  //"Reset" the selected checker variable:
  selectedChecker[0]=NULL;
  selectedChecker[1]=NULL;

  testBoardConfig();
}

