import requests
from GameBoard import *
from GamePiece import *
from PiServer import *

class Backend:
    def __init__(self):
        self.gb = GameBoard()
        self.server = PiServer(__name__, self.gb)

if __name__ == "__main__":
    backend = Backend()
    backend.server.run()