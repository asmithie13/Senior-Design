from enum import Enum

class GamePiece():
    def __init__(self, player, location):
        self.player = player
        self.location = location
        self.isKing = False

class Move():
    def __init__(self, start, end):
        self.start = start
        self.end = end