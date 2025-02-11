from gamePiece import *
class gameBoard():
    def __init__(self):
        # black pieces always move first
        self.currentPlayer = player.BLACK

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

        # Out of bounds check
        if move.start[0] > 7 or move.start[0] < 0 or move.start[1] > 7 or move.start[1] < 0:
            return False, "Invalid move: Start position out of bounds."
        if move.end[0] > 7 or move.end[0] < 0 or move.end[1] > 7 or move.end[1] < 0:
            return False, "Invalid move: End position out of bounds."
        
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
        
        # Move Cases...
        # TODO