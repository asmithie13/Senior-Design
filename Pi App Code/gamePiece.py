from enum import Enum

class gamePiece():
    def __init__(self, player, location):
        self.player = player
        self.location = location
        self.isKing = False

class player(Enum):
    BLACK = 1
    RED = 2

class move():
    def __init__(self, start, end):
        self.start = start
        self.end = end