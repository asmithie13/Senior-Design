import serial
import time
from Objects.GamePiece import *
from Objects.enums import *

class GameBoard():
    def __init__(self):
        self.serialObject = self.connectArduino()   # Bluetooth object to Arduino
        self.tiles = [[None] * 8 for _ in range(8)] # Fill the tiles
        self.currentPlayer = Player.BLACK   # black pieces always move first
        self.redPieces = 12 # red pieces totals
        self.bluePieces = 12   # black pieces totals
        self.redKings = []  # red king coords
        self.blueKings = [] # blue king coords
        self.canDoubleJumpFlag = False  # bool to track if double jump is possinle
        self.doubleJumpNextMoves = []   # moves that need to be selected if there is a valid double jump

        # initialize all orginal pieces
        gamePieces = [
            GamePiece(Player.BLACK, [0, 1]),
            GamePiece(Player.BLACK, [0, 3]),
            GamePiece(Player.BLACK, [0, 5]),
            GamePiece(Player.BLACK, [0, 7]),
            GamePiece(Player.BLACK, [1, 0]),  
            GamePiece(Player.BLACK, [1, 2]),
            GamePiece(Player.BLACK, [1, 4]),
            GamePiece(Player.BLACK, [1, 6]),
            GamePiece(Player.BLACK, [2, 1]),
            GamePiece(Player.BLACK, [2, 3]),
            GamePiece(Player.BLACK, [2, 5]),
            GamePiece(Player.BLACK, [2, 7]),
            GamePiece(Player.BLUE, [5, 0]),
            GamePiece(Player.BLUE, [5, 2]),
            GamePiece(Player.BLUE, [5, 4]),
            GamePiece(Player.BLUE, [5, 6]),
            GamePiece(Player.BLUE, [6, 1]),  
            GamePiece(Player.BLUE, [6, 3]),
            GamePiece(Player.BLUE, [6, 5]),
            GamePiece(Player.BLUE, [6, 7]),
            GamePiece(Player.BLUE, [7, 0]),
            GamePiece(Player.BLUE, [7, 2]),
            GamePiece(Player.BLUE, [7, 4]),
            GamePiece(Player.BLUE, [7, 6])
        ]

        
        for piece in gamePieces:
            self.tiles[piece.location[0]][piece.location[1]] = piece
    
    def connectArduino(self):
        try:
            serialObject=serial.Serial("/dev/ttyUSB0", 9600, timeout=10)
            time.sleep(2)
            print("Connection to Arduino successful.")
            return serialObject
        except serial.SerialException as e:
            print(f"Failed to connect to Arduino: {e}")
            return None
        except Exception as e:
            print(f"An unexpected error occurred: {e}")
            return None
        
    def sendMoveToArduino(self, move, moveType):
        try:
            tempData = str(move.start[0]) + str(move.start[1]) + str(move.end[0]) + str(move.end[1]) + str(moveType.value)
            self.serialObject.write(tempData.encode())
            return True
        except Exception as e:
            print(f"An unexpected error occurred: {e}")
            return False
        
    def sendResetToArduino(self):
        try:
            tempData = "*"
            self.serialObject.write(tempData.encode())
            return True
        except Exception as e:
            print(f"An unexpected error occurred: {e}")
            return False
        

    def handleReset(self):
        self.sendResetToArduino()
        self.__init__()
        return True, self.returnWinner()

    def validateMove(self, move):
        # Out of bounds check
        if move.start[0] > 7 or move.start[0] < 0 or move.start[1] > 7 or move.start[1] < 0:
            return False, MoveError.START_OUT_OF_BOUNDS
        if move.end[0] > 7 or move.end[0] < 0 or move.end[1] > 7 or move.end[1] < 0:
            return False, MoveError.END_OUT_OF_BOUNDS
        
        # Check if start tile is empty
        startPiece = self.tiles[move.start[0]][move.start[1]]
        if startPiece is None:
            return False, MoveError.START_TILE_EMPTY
        
        # Check if end tile is NOT empty
        endPiece = self.tiles[move.end[0]][move.end[1]]
        if endPiece is not None:
            return False, MoveError.END_TILE_OCCUPIED
        
        # Check starting tile is current players piece
        if startPiece.player != self.currentPlayer:
            return False, MoveError.WRONG_PLAYER_PIECE
        
        # Check for king move
        kingResult = self.kingMoveCheck(move)
        if not kingResult[0]: return kingResult

        # Diagonal Move Check
        xDistance = abs(move.start[0] - move.end[0])
        yDistance = abs(move.start[1] - move.end[1])
        if xDistance != yDistance:
            return False, MoveError.NOT_DIAGONAL
        
        # Check if distance moved is 1 or 2
        if xDistance != 1 and xDistance != 2:
            return False, MoveError.INVALID_DISTANCE
        
        # Move Cases...
        # Force double jumps
        if self.canDoubleJumpFlag:
            moveIn = False
            for nextMove in self.doubleJumpNextMoves:
                if move.start == nextMove.start and move.end == nextMove.end:
                    moveIn = True
                    break

            if not moveIn: return False, MoveError.MUST_DOUBLE_JUMP
            
        # Regular Move
        if xDistance == 1:
            self.regularMove(move)
            if self.tiles[move.end[0]][move.end[1]] and self.tiles[move.end[0]][move.end[1]].isKing: self.updateKingArrs() 
            self.switchPlayers()
            self.sendMoveToArduino(move, MoveType.NON_DOUBLE_JUMP_MOVE)
            return True, MoveSuccess.NORMAL_MOVE
    
        # One jump case
        midPoint = self.returnMidpoint(move)
        
        if self.tiles[midPoint[0]][midPoint[1]] is None:
            return False, MoveError.NO_PIECE_TO_CAPTURE
        elif self.tiles[midPoint[0]][midPoint[1]].player == self.currentPlayer:
            return False, MoveError.FRIENDLY_FIRE

        # Over take piece
        self.overtakeMove(move, midPoint)
        if self.redPieces == 0 or self.bluePieces == 0:
            self.handleReset()
            #self.updateKingArrs() 
            return True, self.returnWinner()
        
        # Check double jump
        self.canDoubleJump(move)
        if self.canDoubleJumpFlag:
            self.sendMoveToArduino(move, MoveType.DOUBLE_JUMP_MOVE)
            if self.tiles[move.end[0]][move.end[1]] and self.tiles[move.end[0]][move.end[1]].isKing: self.updateKingArrs() 
            return True, MoveSuccess.DOUBLE_JUMP
        
        self.sendMoveToArduino(move, MoveType.NON_DOUBLE_JUMP_MOVE)
        if self.tiles[move.end[0]][move.end[1]] and self.tiles[move.end[0]][move.end[1]].isKing: self.updateKingArrs()
        self.switchPlayers()
        self.canDoubleJumpFlag = False
        self.doubleJumpNextMoves = []
        return True, MoveSuccess.CAPTURE_PIECE


    def regularMove(self, move):
        self.tiles[move.end[0]][move.end[1]] = self.tiles[move.start[0]][move.start[1]]
        self.tiles[move.end[0]][move.end[1]].location = [move.end[0], move.end[1]]
        self.checkKing(move)
        self.tiles[move.start[0]][move.start[1]] = None
    
    def overtakeMove(self, move, midPoint):
        self.tiles[move.end[0]][move.end[1]] = self.tiles[move.start[0]][move.start[1]]
        self.tiles[move.end[0]][move.end[1]].location = [move.end[0], move.end[1]]
        self.checkKing(move)
        self.tiles[move.start[0]][move.start[1]] = None
        self.tiles[midPoint[0]][midPoint[1]] = None

        if self.currentPlayer == Player.RED:
            self.redPieces -= 1
        else:
            self.bluePieces -= 1

    def returnMidpoint(self, move):
        return [(move.start[0] + move.end[0]) // 2, (move.start[1] + move.end[1]) // 2]
    
    def switchPlayers(self):
        if self.currentPlayer == Player.BLUE:
            self.currentPlayer = Player.RED
        elif self.currentPlayer == Player.RED:
            self.currentPlayer = Player.BLUE
    
    def kingMoveCheck(self, move):
        startPiece = self.tiles[move.start[0]][move.start[1]]
        # Direction checks
        if not startPiece.isKing:
            # Direction check for nonking RED
            if self.currentPlayer == Player.RED:
                if move.start[0] > move.end[0]:
                    return False, MoveError.RED_WRONG_DIRECTION
            # Direction check for nonking BLUE
            if self.currentPlayer == Player.BLUE:
                if move.start[0] < move.end[0]:
                    return False, MoveError.BLUE_WRONG_DIRECTION
                
        return True, None
    
    def checkKing(self, move):
        if self.tiles[move.end[0]][move.end[1]].location[0] == 7 and self.tiles[move.end[0]][move.end[1]].player == Player.RED:
            self.tiles[move.end[0]][move.end[1]].isKing = True
        elif self.tiles[move.end[0]][move.end[1]].location[0] == 0 and self.tiles[move.end[0]][move.end[1]].player == Player.BLUE:
            self.tiles[move.end[0]][move.end[1]].isKing = True

        if self.tiles[move.end[0]][move.end[1]].isKing and self.currentPlayer == Player.RED:
            self.blueKings.append(move.end)

        if self.tiles[move.end[0]][move.end[1]].isKing and self.currentPlayer == Player.BLUE:
            self.redKings.append(move.end)
    
    def canDoubleJump(self, oldMove):
        potentialMoves = []
        actualNextMoves = []

        if oldMove.end[0] + 2 <= 7 and oldMove.end[0] + 2 >= 0 and oldMove.end[1] + 2 <= 7 and oldMove.end[1] + 2 >= 0:
            if self.kingMoveCheck(Move(oldMove.end, [oldMove.end[0] + 2, oldMove.end[1] + 2]))[0]: 
                potentialMoves.append(Move(oldMove.end, [oldMove.end[0] + 2, oldMove.end[1] + 2]))
        if oldMove.end[0] - 2 <= 7 and oldMove.end[0] - 2 >= 0 and oldMove.end[1] + 2 <= 7 and oldMove.end[1] + 2 >= 0:
            if self.kingMoveCheck(Move(oldMove.end, [oldMove.end[0] - 2, oldMove.end[1] + 2]))[0]:
                potentialMoves.append(Move(oldMove.end, [oldMove.end[0] - 2, oldMove.end[1] + 2]))
        if oldMove.end[0] + 2 <= 7 and oldMove.end[0] + 2 >= 0 and oldMove.end[1] - 2 <= 7 and oldMove.end[1] - 2 >= 0:
            if self.kingMoveCheck(Move(oldMove.end, [oldMove.end[0] + 2, oldMove.end[1] - 2]))[0]:
                potentialMoves.append(Move(oldMove.end, [oldMove.end[0] + 2, oldMove.end[1] - 2]))
        if oldMove.end[0] - 2 <= 7 and oldMove.end[0] - 2 >= 0 and oldMove.end[1] - 2 <= 7 and oldMove.end[1] - 2 >= 0:
            if self.kingMoveCheck(Move(oldMove.end, [oldMove.end[0] - 2, oldMove.end[1] - 2]))[0]:
                potentialMoves.append(Move(oldMove.end, [oldMove.end[0] - 2, oldMove.end[1] - 2]))

        for potentialMove in potentialMoves:
            midpoint = self.returnMidpoint(potentialMove)
            if (
                self.tiles[midpoint[0]][midpoint[1]] is not None 
                and self.tiles[midpoint[0]][midpoint[1]].player != self.currentPlayer 
                and self.tiles[potentialMove.end[0]][potentialMove.end[1]] is None
            ):
                actualNextMoves.append(potentialMove)

        self.doubleJumpNextMoves = actualNextMoves
        if not self.doubleJumpNextMoves:
            self.canDoubleJumpFlag = False
        else:
            self.canDoubleJumpFlag = True
    
    def returnWinner(self):
        if self.bluePieces == self.redPieces: return MoveSuccess.TIED_GAME
        if self.bluePieces > self.redPieces: return MoveSuccess.BLUE_WINS
        return MoveSuccess.RED_WINS
    
    def updateKingArrs(self):
        tempArrRed = []
        tempArrBlue = []
        for i in range(8):
            for j in range(8):
                if self.tiles[i][j] and self.tiles[i][j].isKing and self.currentPlayer == Player.RED:
                    tempArrRed.append([i, j])
                if self.tiles[i][j] and self.tiles[i][j].isKing and self.currentPlayer == Player.BLACK:
                    tempArrBlue.append([i, j])
        self.redKings = tempArrRed
        self.blueKings = tempArrBlue
  