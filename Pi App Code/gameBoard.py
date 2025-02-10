from gamePiece import *
class gameBoard():
    def __init__(self):
        # initialize all orginal pieces
        gamePieces = [
            gamePiece(player.WHITE, [0, 1]),
            gamePiece(player.WHITE, [0, 3]),
            gamePiece(player.WHITE, [0, 5]),
            gamePiece(player.WHITE, [0, 7]),
            gamePiece(player.WHITE, [1, 0]),  
            gamePiece(player.WHITE, [1, 2]),
            gamePiece(player.WHITE, [1, 4]),
            gamePiece(player.WHITE, [1, 6]),
            gamePiece(player.WHITE, [2, 1]),
            gamePiece(player.WHITE, [2, 3]),
            gamePiece(player.WHITE, [2, 5]),
            gamePiece(player.WHITE, [2, 7]),
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
        
