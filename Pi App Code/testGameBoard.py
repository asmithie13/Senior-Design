import unittest
from gameBoard import *
from gamePiece import *

class testGameBoard(unittest.TestCase):
    
    def setUp(self):
        self.gb = gameBoard()

    # validateMove tests
    def testBoundsCheck(self):
        move1 = move([-1, -1], [2, 2])
        self.assertEqual(self.gb.validateMove(move1), (False, moveError.START_OUT_OF_BOUNDS))
        move2 = move([2, 2], [-1, -1])
        self.assertEqual(self.gb.validateMove(move2), (False, moveError.END_OUT_OF_BOUNDS))

    def testEmptyStartTile(self):
        move1 = move([0, 0], [1, 2])
        self.assertEqual(self.gb.validateMove(move1), (False, moveError.START_TILE_EMPTY))

    def testNonEmptyEndTile(self):
        move1 = move([2, 3], [1, 2])
        self.assertEqual(self.gb.validateMove(move1), (False, moveError.END_TILE_OCCUPIED))

    def testCurrentPiece(self):
        move1 = move([5, 0], [4, 1])
        self.assertEqual(self.gb.validateMove(move1), (False, moveError.WRONG_PLAYER_PIECE))
    
    def testNonKingPieces(self):
        move1 = move([2, 7], [3, 6])    # moves black down
        move2 = move([5, 0], [4, 1])    # moves red up
        move3 = move([3, 6], [2, 7])    # moves black up
        move4 = move([4, 1], [5, 0])    # moves red down

        self.gb.validateMove(move1)
        self.gb.validateMove(move2)

        self.assertEqual(self.gb.validateMove(move3), (False, moveError.BLACK_WRONG_DIRECTION))

if __name__ == '__main__':
    unittest.main()