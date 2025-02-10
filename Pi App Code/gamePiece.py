from enum import Enum

class gamePiece():
    def __init__(self, player, location):
        self.player = player
        self.isKing = False

class player(Enum):
    WHITE = 1
    RED = 2