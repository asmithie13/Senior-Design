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

//Variables to store each player's score:
int playerOneScore=0; 
int playerTwoScore=0;

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
  //TEST:
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
  Serial.print("\nPlayer #1 Score: ");
  Serial.print(playerOneScore);
  Serial.print("\nPlayer #2 Score: ");
  Serial.print(playerTwoScore);
}

//Function that scans the board to check if any king pieces must be awarded:
void checkForKing(){
  for(int i=0; i<8; i++){
    if(checkerBoard[0][i]==2){
      checkerBoard[0][i]++;
    }
    else if(checkerBoard[7][i]==1){
      checkerBoard[7][i]++;
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

    //Coordinate exceeds board limits:
    if(coordOne<0 || coordOne>7 || coordTwo<0 || coordTwo>7){
      continue;
    }
    
    if(checkerBoard[coordOne][coordTwo]==1){
      validFlag=checkIfMovePlayerOne(coordOne, coordTwo);
    }
    else if(checkerBoard[coordOne][coordTwo]==2){
      validFlag=checkIfMovePlayerTwo(coordOne, coordTwo);
    }
    else{
      validFlag=checkIfMoveKing(coordOne, coordTwo);
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
    Serial.print("A");
    //Now, see if there is a move available to skip over the opposing player's checker:
    int tempFlag=0;

    //Check to see if the move to the left would be valid:
    if(coordOnePlusTwo>7 || coordTwoMinTwo<0 || checkerBoard[coordOnePlus][coordTwoMin]!=2 || checkerBoard[coordOnePlusTwo][coordTwoMinTwo]!=0){
      tempFlag++;
    }

    //Check to see if the move to the right would be valid:
    if(coordOnePlusTwo>7 || coordTwoPlusTwo>7 || checkerBoard[coordOnePlus][coordTwoPlus]!=2 || checkerBoard[coordOnePlusTwo][coordTwoPlusTwo]!=0){
      tempFlag++;
    }

    //Check to see if a move forward without skipping over the opponent's checker is available:
    if(checkerBoard[coordOne+1][coordTwo-1]!=0 && checkerBoard[coordOne+1][coordTwo+1]!=0){
      tempFlag++;
    }

    //If all of the above moves are unavailable, the player may not move the piece:
    if(tempFlag==3){
      return 1;
    }
  }

  //The piece is on the left outer-edge, and the diagonal-right move is not available:
  if((coordTwoMin<0 && checkerBoard[coordOnePlus][coordTwoPlus]!=0) && (coordTwoMin<0 && (checkerBoard[coordOnePlusTwo][coordTwoPlusTwo]!=0 || checkerBoard[coordOnePlus][coordTwoPlus]!=2))){
    Serial.print("B");
    return 1;
  }

  //The piece is on the right outer-edge, and the diagonal-left move is not available:
  if((coordTwoPlus>7 && checkerBoard[coordOnePlus][coordTwoMin]!=0) && (coordTwoPlus>7 && (checkerBoard[coordOnePlusTwo][coordTwoMinTwo]!=0 || checkerBoard[coordOnePlus][coordTwoMin]!=2))){
    Serial.print("C");
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
    if(coordOneMinTwo<0 || coordTwoMinTwo<0 || checkerBoard[coordOneMin][coordTwoMin]!=1 || checkerBoard[coordOneMinTwo][coordTwoMinTwo]!=0){
      tempFlag++;
    }

    //Check to see if the move to the right would be valid:
    if(coordOneMinTwo<0 || coordTwoPlusTwo>7 || checkerBoard[coordOneMin][coordTwoPlus]!=1 || checkerBoard[coordOneMinTwo][coordTwoPlusTwo]!=0){
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
  if((coordTwoMin<0 && checkerBoard[coordOneMin][coordTwoPlus]!=0) && (coordTwoMin<0 && (checkerBoard[coordOneMinTwo][coordTwoPlusTwo]!=0 || checkerBoard[coordOneMin][coordTwoPlus]!=1))){
    return 1;
  }

  //The piece is on the right outer-edge, and the diagonal-left move is not available:
  if(((coordTwoPlus>7 && checkerBoard[coordOneMin][coordTwoMin]!=0)) && (coordTwoPlus>7 && (checkerBoard[coordOneMinTwo][coordTwoMinTwo]!=0 || checkerBoard[coordOneMin][coordTwoMin]!=1))){
    return 1;
  }

  return 0;
}

//Function to check if the checker being selected has any available moves (this function is only to be used if the...
//...checker that is attempting to be selected is a king piece for either player):
int checkIfMoveKing(int coordOne, int coordTwo){
  //Find if the move to be made would exceed board limits:
  int coordOneMin, coordOnePlus, coordTwoMin, coordTwoPlus;
  coordOneMin=coordOne-1;
  coordOnePlus=coordOne+1;
  coordTwoMin=coordTwo-1;
  coordTwoPlus=coordTwo+1;

  if(coordOneMin>=0 && coordOneMin<8 && coordOnePlus>=0 && coordOnePlus<8 && coordTwoMin>=0 && coordTwoMin<8 && coordTwoPlus>=0 && coordTwoPlus<8){
    if(checkerBoard[coordOne-1][coordTwo-1]!=0 && checkerBoard[coordOne-1][coordTwo+1]!=0 && checkerBoard[coordOne+1][coordTwo-1]!=0 && checkerBoard[coordOne+1][coordTwo+1]!=0){
      return 1;
    }
  }

  if(coordOneMin<0 && coordTwoMin<0 && checkerBoard[coordOne+1][coordTwo+1]!=0){
    return 1;
  }

  if(coordOnePlus>7 && coordTwoPlus>7 && checkerBoard[coordOne-1][coordTwo-1]!=0){
    return 1;
  }

  if(coordOneMin<0 && checkerBoard[coordOne+1][coordTwo+1]!=0 && checkerBoard[coordOne+1][coordTwo-1]!=0){
    return 1;
  }

  if(coordOnePlus>7 && checkerBoard[coordOne-1][coordTwo+1]!=0 && checkerBoard[coordOne-1][coordTwo-1]!=0){
    return 1;
  }

  if(coordTwoMin<0 && checkerBoard[coordOne-1][coordTwo+1]!=0 && checkerBoard[coordOne+1][coordTwo+1]!=0){
    return 1;
  }

  if(coordTwoPlus>7 && checkerBoard[coordOne-1][coordTwo-1]!=0 && checkerBoard[coordOne+1][coordTwo-1]!=0){
    return 1;
  }

  return 0;
}

//Function to increment the player's score:
void incrementScore(){
  if(playerInTurn.playerNum==1){
    playerOneScore+=2;
  }
  else{
    playerTwoScore+=2;
  }
}

//Select the space to move the checker to:
void moveChecker(){
  //Variables to hold each coordinate:
  int coordOne, coordTwo;

  //Flag to determine if the move is valid:
  bool validFlag=false;

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

  //If the input coordinate was invalid, ask again:
  while(coordOne<0 || coordOne>7 || coordTwo<0 || coordTwo>7 || (coordOne+coordTwo)%2==1 || checkerBoard[coordOne][coordTwo]!=0 || validFlag==false){
    //Print that the user must input additional values, since the first value was invalid. Either the...
    //...coordinate is not present on the board, or a checker is not at that position:
    coordOne=pollForSelection()-'0';
    coordTwo=pollForSelection()-'0';

    //First, ensure that the coordinates of the desired move are within the proper bounds:
    if(coordOne>=0 && coordOne<8 && coordTwo>=0 && coordTwo<8){

      //Establish move conditions for Player #1's checkers:
      if(checkerBoard[selectedChecker[0]][selectedChecker[1]]==1){
        //If the move involves the checker moving forward to either side, and the space is empty, allow the move:
        if(coordOne==selectedChecker[0]+1 && (coordTwo==selectedChecker[1]-1 || coordTwo==selectedChecker[1]+1) && checkerBoard[coordOne][coordTwo]==0){
          break;
        }
        //If the move involves skipping the opponent's checker, and the space is empty, allow the move:
        if(coordOne==selectedChecker[0]+2 && (coordTwo==selectedChecker[1]-2 || coordTwo==selectedChecker[1]+2) && checkerBoard[coordOne][coordTwo]==0){
          //For a successful skip to occur, the checker being skipped must belong to the opposing player:
          if(coordTwo==selectedChecker[1]-2 && (checkerBoard[coordOne-1][coordTwo+1]==neededPlayer[0] || checkerBoard[coordOne-1][coordTwo+1]==neededPlayer[1])){
            //If valid, remove that chip from the board, and increment the current player's score:
            checkerBoard[coordOne-1][coordTwo+1]=0;
            //Increment the score, since a checker was removed:
            incrementScore();
            break;
          }
          else if(coordTwo==selectedChecker[1]+2 && (checkerBoard[coordOne-1][coordTwo-1]==neededPlayer[0] || checkerBoard[coordOne-1][coordTwo-1]==neededPlayer[1])){
            checkerBoard[coordOne-1][coordTwo-1]=0;
            //Increment the score, since a checker was removed:
            incrementScore();
            break;
          }
        }
      }

      //Establish move conditions for Player #2's checkers:
      else if(checkerBoard[selectedChecker[0]][selectedChecker[1]]==2){
        //If the move involves the checker moving forward to either side, and the space is empty, allow the move:
        if(coordOne==selectedChecker[0]-1 && (coordTwo==selectedChecker[1]-1 || coordTwo==selectedChecker[1]+1) && checkerBoard[coordOne][coordTwo]==0){
          break;
        }
        //If the move involves skipping the opponent's checker, and the space is empty, allow the move:
        if(coordOne==selectedChecker[0]-2 && (coordTwo==selectedChecker[1]-2 || coordTwo==selectedChecker[1]+2) && checkerBoard[coordOne][coordTwo]==0){
          //For a successful skip to occur, the checker being skipped must belong to the opposing player:
          if(coordTwo==selectedChecker[1]-2 && (checkerBoard[coordOne+1][coordTwo+1]==neededPlayer[0] || checkerBoard[coordOne+1][coordTwo+1]==neededPlayer[1])){
            //If valid, remove that chip from the board, and increment the current player's score:
            checkerBoard[coordOne+1][coordTwo+1]=0;
            //Increment the score, since a checker was removed:
            incrementScore();
            break;
          }
          else if(coordTwo==selectedChecker[1]+2 && (checkerBoard[coordOne+1][coordTwo-1]==neededPlayer[0] || checkerBoard[coordOne+1][coordTwo-1]==neededPlayer[1])){
            checkerBoard[coordOne+1][coordTwo-1]=0;
            //Increment the score, since a checker was removed:
            incrementScore();
            break;
          }
        }
      }

      //Establish move conditions for either players' king checkers:
    }
  }
  
  //Set the new checker space:
  if(playerInTurn.playerNum==1){
    checkerBoard[coordOne][coordTwo]=1;
  }
  else{
    checkerBoard[coordOne][coordTwo]=2;
  }

  //Set the spot where the selected checker was to an empty space:
  checkerBoard[selectedChecker[0]][selectedChecker[1]]=0;
  
  //"Reset" the selected checker variable:
  selectedChecker[0]=NULL;
  selectedChecker[1]=NULL;

  //Check after the move to see if any king pieces need to be set:
  checkForKing();

  //TEST:
  testBoardConfig();
}

