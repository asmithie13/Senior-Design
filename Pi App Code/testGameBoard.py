import unittest
from gameBoard import *
from gamePiece import *

class testGameBoard(unittest.TestCase):
    
    def setUp(self):
        self.gb = gameBoard()

    # validateMove tests
    def testBoundsCheck(self):
        move1 = move([-1, -1], [2, 2])
        self.assertEqual(self.gb.validateMove(move1), (False, "Invalid move: Start position out of bounds."))
        move2 = move([2, 2], [-1, -1])
        self.assertEqual(self.gb.validateMove(move2), (False, "Invalid move: End position out of bounds."))

    def testEmptyStartTile(self):
        move1 = move([0, 0], [1, 2])
        self.assertEqual(self.gb.validateMove(move1), (False, "Invalid move: Start tile is empty."))

    def testNonEmptyEndTile(self):
        move1 = move([2, 3], [1, 2])
        self.assertEqual(self.gb.validateMove(move1), (False, "Invalid move: End tile is occupied."))

    def testCurrentPiece(self):
        move1 = move([5, 0], [4, 1])
        self.assertEqual(self.gb.validateMove(move1), (False, "Invalid move: You can only move your own pieces."))
    
    def testNonKingPieces(self):
        move1 = move([2, 7], [3, 6])    # moves black down
        move2 = move([5, 0], [4, 1])    # moves red up
        move3 = move([3, 6], [2, 7])    # moves black up
        move4 = move([4, 1], [5, 0])    # moves red down

        self.gb.validateMove(move1)
        self.gb.validateMove(move2)

        self.assertEqual(self.gb.validateMove(move3), (False, "Invalid move: BLACK pieces must move downward."))

if __name__ == '__main__':
    unittest.main()