//Program to implement checkers logic (used within manual operation):
#include <Keypad.h>

//---
//TO-DO:
//Allow for double-jumps if a player skips over a checker and another skip is available
//---

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

//TEST:
int tempSelectedChecker[2]={NULL, NULL};

//Variables to store each player's score:
int playerOneScore=0; 
int playerTwoScore=0;

//Flag that determines if the player just jumped, meaning that there is a possibility to double-jump:
bool inJump=false;

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

//Wait for user keypad input:
char pollForSelection(){
  char customKey=customKeypad.getKey();
  while(customKey==NULL){
    customKey=customKeypad.getKey();
  }
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
void selectChecker(int prevCoordOne=-1, int prevCoordTwo=-1){
  //Declare variables to hold the checker's coordinates:
  int coordOne, coordTwo;

  //Flag to further signal if the move is valid:
  int validFlag=1;

  //Flag to determine the number of times that the loop was iterated through:
  int numIt=0;

  //If the input coordinate was invalid, ask again:
  while(validFlag>0 || (coordOne+coordTwo)%2==1){ //"(coordOne+coordTwo)%2==1" signals that the input coordinate is on an un-used square in the game
    //Print that the user must input additional values, since the first value was invalid. Either the...
    //...coordinate is not present on the board, or a checker is not at that position:
    
    //TEST:
    if(inJump==true && numIt==0){
      coordOne=prevCoordOne;
      coordTwo=prevCoordTwo;
    }
    else if(inJump==true && numIt==1){
      if(playerInTurn.playerNum==1){
        playerInTurn.playerNum=2;
      }
      else{
        playerInTurn.playerNum=1;
      }
      inJump=false;
      coordOne=pollForSelection()-'0';
      coordTwo=pollForSelection()-'0';
    }
    else{
      coordOne=pollForSelection()-'0';
      coordTwo=pollForSelection()-'0';
    }
    //Iterate:
    numIt++;

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
      if(validFlag<2){
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

  //If moving forward is out-of-bounds, no move is possible:
  if(coordOnePlus>7){
    return 1;
  }
  
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
    if(inJump==false && checkerBoard[coordOnePlus][coordTwoMin]!=0 && checkerBoard[coordOnePlus][coordTwoPlus]!=0){
      tempFlag+=3;
    }

    //If all of the above moves are unavailable, the player may not move the piece:
    if(tempFlag==5 || (inJump==true && tempFlag==2)){
      return 1;
    }
  }

  //The piece is on the left outer-edge, and the diagonal-right move is not available. If...
  //...the piece is in the middle of a jump, then it will always be invalid:
  if(coordTwoMin<0 && (inJump==true || (checkerBoard[coordOnePlus][coordTwoPlus]!=0)) && ((checkerBoard[coordOnePlusTwo][coordTwoPlusTwo]==0 && checkerBoard[coordOnePlus][coordTwoPlus]!=2 && checkerBoard[coordOnePlus][coordTwoPlus]!=4) || checkerBoard[coordOnePlusTwo][coordTwoPlusTwo]!=0)){
    return 1;
  }

  //The piece is on the right outer-edge, and the diagonal-left move is not available. If...
  //...the piece is in the middle of a jump, then it will always be invalid:
  if(coordTwoPlus>7 && (inJump==true || (checkerBoard[coordOnePlus][coordTwoMin]!=0)) && ((checkerBoard[coordOnePlusTwo][coordTwoMinTwo]==0 && checkerBoard[coordOnePlus][coordTwoMin]!=2 && checkerBoard[coordOnePlus][coordTwoMin]!=4) || checkerBoard[coordOnePlusTwo][coordTwoMinTwo]!=0)){ 
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

  //If moving forward is out-of-bounds, no move is possible:
  if(coordOneMin<0){
    return 1;
  }

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
    if(inJump==false && checkerBoard[coordOneMin][coordTwoMin]!=0 && checkerBoard[coordOneMin][coordTwoPlus]!=0){
      tempFlag+=3;
    }
   
    //If all of the above moves are unavailable, the player may not move the piece:
    if(tempFlag==5 || (inJump==true && tempFlag==2)){
      return 1;
    }
  }

  //The piece is on the left outer-edge, and the diagonal-right move is not available:
  if(coordTwoMin<0 && (inJump==true || (checkerBoard[coordOneMin][coordTwoPlus]!=0)) && ((checkerBoard[coordOneMinTwo][coordTwoPlusTwo]==0 && checkerBoard[coordOneMin][coordTwoPlus]!=1 && checkerBoard[coordOneMin][coordTwoPlus]!=3) || checkerBoard[coordOneMinTwo][coordTwoPlusTwo]!=0)){
    return 1;
  }

  //The piece is on the right outer-edge, and the diagonal-left move is not available:
  if(coordTwoPlus>7 && (inJump==true || (checkerBoard[coordOneMin][coordTwoMin]!=0)) && ((checkerBoard[coordOneMinTwo][coordTwoMinTwo]==0 && checkerBoard[coordOneMin][coordTwoMin]!=1 && checkerBoard[coordOneMin][coordTwoMin]!=3) || checkerBoard[coordOneMinTwo][coordTwoMinTwo]!=0)){
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
  if(inJump==false && (coordOne==selectedChecker[0]+1 && (coordTwo==selectedChecker[1]-1 || coordTwo==selectedChecker[1]+1) && checkerBoard[coordOne][coordTwo]==0)){
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

      //Change the in-jump flag, to prepare for a double-jump, and set the new position as the selected checker:
      inJump=true;
      tempSelectedChecker[0]=coordOne;
      tempSelectedChecker[1]=coordTwo;
    }
    else if(coordTwo==selectedChecker[1]+2 && (checkerBoard[coordOne-1][coordTwo-1]==neededPlayer[0] || checkerBoard[coordOne-1][coordTwo-1]==neededPlayer[1])){
      checkerBoard[coordOne-1][coordTwo-1]=0;
      //Increment the score, since a checker was removed:
      scoreFlag=1;
      retFlag=1;
      
      //Change the in-jump flag, to prepare for a double-jump, and set the new position as the selected checker:
      inJump=true;
      tempSelectedChecker[0]=coordOne;
      tempSelectedChecker[1]=coordTwo;
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
  if(inJump==false && (coordOne==selectedChecker[0]-1 && (coordTwo==selectedChecker[1]-1 || coordTwo==selectedChecker[1]+1) && checkerBoard[coordOne][coordTwo]==0)){
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

      //Change the in-jump flag, to prepare for a double-jump, and set the new position as the selected checker:
      inJump=true;
      tempSelectedChecker[0]=coordOne;
      tempSelectedChecker[1]=coordTwo;
    }
    else if(coordTwo==selectedChecker[1]+2 && (checkerBoard[coordOne+1][coordTwo-1]==neededPlayer[0] || checkerBoard[coordOne+1][coordTwo-1]==neededPlayer[1])){
      checkerBoard[coordOne+1][coordTwo-1]=0;
      //Increment the score, since a checker was removed:
      scoreFlag=1;
      retFlag=1;

      //Change the in-jump flag, to prepare for a double-jump, and set the new position as the selected checker:
      inJump=true;
      tempSelectedChecker[0]=coordOne;
      tempSelectedChecker[1]=coordTwo;
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

  //If the input coordinate was invalid, ask again:
  while(coordOne<0 || coordOne>7 || coordTwo<0 || coordTwo>7 || (coordOne+coordTwo)%2==1 || checkerBoard[coordOne][coordTwo]!=0 || validFlag==0){
    //Print that the user must input additional values, since the first value was invalid. Either the...
    //...coordinate is not present on the board, or a checker is not at that position:
    coordOne=pollForSelection()-'0';
    coordTwo=pollForSelection()-'0';

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

  //Set the spot where the selected checker was to an empty space:
  checkerBoard[selectedChecker[0]][selectedChecker[1]]=0;

  //Now, set the selected checker to the checker just moved:
  selectedChecker[0]=tempSelectedChecker[0];
  selectedChecker[1]=tempSelectedChecker[1];
  
  //"Reset" the selected checker variable:
  if(inJump==false){
    selectedChecker[0]=NULL;
    selectedChecker[1]=NULL;
  }

  //Check after the move to see if any king pieces need to be set:
  checkForKing();

  //Check to see if there is a winner:
  checkForWinner();

  //TEST:
  testBoardConfig();
}

//Check to see if the game is over, and award a winner:
void checkForWinner(){
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
  if(playerOneNum==0){
    Serial.print("\nWINNER: Player #2");
    //Enter an infinite loop, that is only exited if the player presses the button indicating a new game:
    //Also, consider playing a win sequence here
  }
  else if(playerTwoNum==0){
    Serial.print("\nWINNER: Player #1");
  }
}

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

//Execute the program:
void loop(){
  //Select the checker to be moved:
  selectChecker(selectedChecker[0], selectedChecker[1]);

  //Move the selected checker:
  moveChecker();

  //Change the player in-turn:
  if(inJump==false){
    if(playerInTurn.playerNum==1){
      playerInTurn.playerNum=2;
    }
    else if(playerInTurn.playerNum==2){
      playerInTurn.playerNum=1;
    }
  }
}