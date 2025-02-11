from gamePiece import *
class gameBoard():
    def __init__(self):
        # black pieces always move first
        self.currentPlayer = player.BLACK

        # pieces totals
        self.redPieces = 12
        self.blackPieces = 12

        # initialize all orginal pieces
        gamePieces = [
            gamePiece(player.BLACK, [0, 1]),
            gamePiece(player.BLACK, [0, 3]),
            gamePiece(player.BLACK, [0, 5]),
            gamePiece(player.BLACK, [0, 7]),
            gamePiece(player.BLACK, [1, 0]),  
            gamePiece(player.BLACK, [1, 2]),
            gamePiece(player.BLACK, [1, 4]),
            gamePiece(player.BLACK, [1, 6]),
            gamePiece(player.BLACK, [2, 1]),
            gamePiece(player.BLACK, [2, 3]),
            gamePiece(player.BLACK, [2, 5]),
            gamePiece(player.BLACK, [2, 7]),
            gamePiece(player.RED, [5, 0]),
            gamePiece(player.RED, [5, 2]),
            gamePiece(player.RED, [5, 4]),
            gamePiece(player.RED, [5, 6]),
            gamePiece(player.RED, [6, 1]),  
            gamePiece(player.RED, [6, 3]),
            gamePiece(player.RED, [6, 5]),
            gamePiece(player.RED, [6, 7]),
            gamePiece(player.RED, [7, 0]),
            gamePiece(player.RED, [7, 2]),
            gamePiece(player.RED, [7, 4]),
            gamePiece(player.RED, [7, 6])
        ]

        # Fill the tiles
        self.tiles = [[None] * 8 for _ in range(8)]
        for piece in gamePieces:
            self.tiles[piece.location[0]][piece.location[1]] = piece
        
    def validateMove(self, move):
        # Out of bounds check
        if move.start[0] > 7 or move.start[0] < 0 or move.start[1] > 7 or move.start[1] < 0:
            return False, "Invalid move: Start position out of bounds."
        if move.end[0] > 7 or move.end[0] < 0 or move.end[1] > 7 or move.end[1] < 0:
            return False, "Invalid move: End position out of bounds."
        
        # Check if start tile is empty
        startPiece = self.tiles[move.start[0]][move.start[1]]
        if startPiece is None:
            return False, "Invalid move: Start tile is empty."
        
        # Check if end tile is NOT empty
        endPiece = self.tiles[move.end[0]][move.end[1]]
        if endPiece is not None:
            return False, "Invalid move: End tile is occupied."
        
        # Check starting tile is current players piece
        if startPiece.player != self.currentPlayer:
            return False, "Invalid move: You can only move your own pieces."
        
        # Direction checks
        if not startPiece.isKing:
            # Direction check for nonking BLACK
            if self.currentPlayer == player.BLACK:
                if move.start[0] > move.end[0]:
                    return False, "Invalid move: BLACK pieces must move downward."
            # Direction check for nonking RED
            if self.currentPlayer == player.RED:
                if move.start[0] < move.end[0]:
                    return False, "Invalid move: RED pieces must move upward."

        # Diagonal Move Check
        xDistance = abs(move.start[0] - move.end[0])
        yDistance = abs(move.start[1] - move.end[1])
        if xDistance != yDistance:
            return False, "Invalid move: Pieces must move diagonally."
        
        # Check if distance moved is 1 or 2
        if xDistance != 1 and xDistance != 2:
            return False, "Invalid move: Distance moved not 1 or 2 across."
        
        # Move Cases...
        # Regular Move
        if xDistance == 1:
            self.regularMove(move)
            self.switchPlayers()
            return True, "Normal move is made"
        else:
            # TODO double jump case

            # One jump case
            midPoint = [(move.start[0] + move.end[0]) / 2, (move.start[1] + move.end[1]) / 2]
            
            if self.tiles[midPoint[0]][midPoint[1]] is None:
                return False, "Invalid move: No piece is being taken with this 2 distance move"
            elif self.tiles[midPoint[0]][midPoint[1]].player == self.currentPlayer:
                return False, "Invalid move: Cannot overtake one's own piece"
            else:
                self.overtakeMove(move, midPoint)
                self.switchPlayers()
                return True, "Piece is overtaken"


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

        if self.currentPlayer == player.BLACK:
            self.redPieces -= 1
        else:
            self.blackPieces -= 1
    
    def switchPlayers(self):
        if self.currentPlayer == player.BLACK:
            self.currentPlayer = player.RED
        elif self.currentPlayer == player.RED:
            self.currentPlayer = player.BLACK
    
    def checkKing(self, move):
        if self.tiles[move.end[0]][move.end[1]].location[0] == 7 and self.tiles[move.end[0]][move.end[1]].player == player.BLACK:
            self.tiles[move.end[0]][move.end[1]].isKing = True
        elif self.tiles[move.end[0]][move.end[1]].location[0] == 0 and self.tiles[move.end[0]][move.end[1]].player == player.RED:
            self.tiles[move.end[0]][move.end[1]].isKing = True
