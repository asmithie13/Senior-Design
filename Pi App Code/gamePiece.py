from enum import Enum

class gamePiece():
    def __init__(self, player, location):
        self.player = player
        self.location = location
        self.isKing = False

class player(Enum):
    BLACK = 1
    RED = 2

class moveError(Enum, str):
    START_OUT_OF_BOUNDS = "Invalid move: Start position out of bounds."
    END_OUT_OF_BOUNDS = "Invalid move: End position out of bounds."
    START_TILE_EMPTY = "Invalid move: Start tile is empty."
    END_TILE_OCCUPIED = "Invalid move: End tile is occupied."
    WRONG_PLAYER_PIECE = "Invalid move: You can only move your own pieces."
    BLACK_WRONG_DIRECTION = "Invalid move: BLACK pieces must move downward."
    RED_WRONG_DIRECTION = "Invalid move: RED pieces must move upward."
    NOT_DIAGONAL = "Invalid move: Pieces must move diagonally."
    INVALID_DISTANCE = "Invalid move: Distance moved not 1 or 2 across."
    NO_PIECE_TO_CAPTURE = "Invalid move: No piece is being taken with this 2 distance move."
    FRIENDLY_FIRE = "Invalid move: Cannot overtake one's own piece."

class moveSuccess(Enum, str):
    NORMAL_MOVE = "Normal move is made"
    CAPTURE_PIECE = "Piece is overtaken"

class move():
    def __init__(self, start, end):
        self.start = start
        self.end = end