from enum import Enum

class Player(Enum):
    RED = 1
    BLUE = 2

class MoveType(Enum):
    NON_DOUBLE_JUMP_MOVE = 0
    DOUBLE_JUMP_MOVE = 1

class MoveError(str, Enum):
    START_OUT_OF_BOUNDS = "Invalid move: Start position out of bounds."
    END_OUT_OF_BOUNDS = "Invalid move: End position out of bounds."
    START_TILE_EMPTY = "Invalid move: Start tile is empty."
    END_TILE_OCCUPIED = "Invalid move: End tile is occupied."
    WRONG_PLAYER_PIECE = "Invalid move: You can only move your own pieces."
    RED_WRONG_DIRECTION = "Invalid move: RED pieces must move downward."
    BLUE_WRONG_DIRECTION = "Invalid move: BLUE pieces must move upward."
    NOT_DIAGONAL = "Invalid move: Pieces must move diagonally."
    INVALID_DISTANCE = "Invalid move: Distance moved not 1 or 2 across."
    NO_PIECE_TO_CAPTURE = "Invalid move: No piece is being taken with this 2 distance move."
    FRIENDLY_FIRE = "Invalid move: Cannot overtake one's own piece."
    MUST_DOUBLE_JUMP = "Invalid move: A double jump must be performed"

class MoveSuccess(str, Enum):
    NORMAL_MOVE = "Normal move is made"
    CAPTURE_PIECE = "Piece is overtaken"
    DOUBLE_JUMP = "Piece is overtaken. Must double jump."
    GAME_OVER = "A player has no more pieces to play."
    TIED_GAME = "Game is tied. Red pieces = Blue pieces"
    RED_WINS = "Red wins. Red pieces > Blue pieces"
    BLUE_WINS = "Blue wins. Blue pieces > Red pieces"

class PhoneError(str, Enum):
    DECODE_ERROR = "Failed to decode data sent from phone"
    INVALID_SIGNAL = "Signal received is not valid for reset or move"