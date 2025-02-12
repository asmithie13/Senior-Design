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

        self.gb.validateMove(move1) # Move pieces to move back
        self.gb.validateMove(move2)

        result1 = self.gb.validateMove(move3) # invalid BLACK move
        self.gb.validateMove(move([3, 6], [4, 7])) # valid move to change the turn
        result2 = self.gb.validateMove(move4)

        self.assertEqual(result1, (False, moveError.BLACK_WRONG_DIRECTION))
        self.assertEqual(result2, (False, moveError.RED_WRONG_DIRECTION))

    def testDiagonalMove(self):
        move1 = move([2, 7], [3, 7])
        self.assertEqual(self.gb.validateMove(move1), (False, moveError.NOT_DIAGONAL))

    def testInvalidDistance(self):
        move1 = move([0, 1], [3, 4])
        self.assertEqual(self.gb.validateMove(move1), (False, moveError.INVALID_DISTANCE))

    def testRegularMoveCase(self):
        move1 = move([2, 7], [3, 6])
        self.assertEqual(self.gb.currentPlayer, player.BLACK)
        result1 = self.gb.validateMove(move1)
        self.assertEqual(result1, (True, moveSuccess.NORMAL_MOVE))
        self.assertEqual(self.gb.currentPlayer, player.RED)

    def testRegularMoveFunction(self):
        move1 = move([2, 7], [3, 6])
        gpiece1 = gamePiece(player.BLACK, [2, 7])
        startPiece = self.gb.tiles[move1.start[0]][move1.start[1]]
        
        # before move
        self.assertEqual(gpiece1.player, startPiece.player)
        self.assertEqual(gpiece1.location, startPiece.location)
        self.assertEqual(gpiece1.isKing, startPiece.isKing)
        self.assertIsNone(self.gb.tiles[move1.end[0]][move1.end[1]])
        
        self.gb.regularMove(move1)
        # after move
        gpiece2 = gamePiece(player.BLACK, [3, 6])
        endPiece = self.gb.tiles[move1.end[0]][move1.end[1]]

        self.assertEqual(gpiece2.player, endPiece.player)
        self.assertEqual(gpiece2.location, endPiece.location)
        self.assertEqual(gpiece2.isKing, endPiece.isKing)
        self.assertIsNone(self.gb.tiles[move1.start[0]][move1.start[1]])

    def testNoPieceToCapture(self):
        move1 = move([2, 7], [4, 5])
        result1 = self.gb.validateMove(move1)
        self.assertEqual(result1, (False, moveError.NO_PIECE_TO_CAPTURE))

    def testFriendlyFire(self):
        move1 = move([1, 2], [3, 4])
        result1 = self.gb.validateMove(move1)
        self.assertEqual(result1, (False, moveError.FRIENDLY_FIRE))

    def testCapturePiece(self):
        move1 = move([2, 7], [3, 6])
        move2 = move([5, 4], [4, 5])
        move3 = move([3, 6], [5, 4])
        self.gb.validateMove(move1)
        self.gb.validateMove(move2)
        result1 = self.gb.validateMove(move3)
        self.assertEqual(result1, (True, moveSuccess.CAPTURE_PIECE))

    def testOvertakeMoveFunction(self):
        move1 = move([2, 7], [3, 6])
        move2 = move([5, 4], [4, 5])
        move3 = move([3, 6], [5, 4])

        self.gb.validateMove(move1)
        self.gb.validateMove(move2)

        gpiece1 = gamePiece(player.BLACK, [3, 6])
        startPiece1 = self.gb.tiles[move1.end[0]][move1.end[1]]
        gpiece2 = gamePiece(player.RED, [4, 5])
        startPiece2 = self.gb.tiles[move2.end[0]][move2.end[1]]
        
        # before jump
        self.assertEqual(gpiece1.player, startPiece1.player)
        self.assertEqual(gpiece1.location, startPiece1.location)
        self.assertEqual(gpiece1.isKing, startPiece1.isKing)
        
        self.assertEqual(gpiece2.player, startPiece2.player)
        self.assertEqual(gpiece2.location, startPiece2.location)
        self.assertEqual(gpiece2.isKing, startPiece2.isKing)

        self.gb.overtakeMove(move3, [4, 5])
        gpiece3 = gamePiece(player.BLACK, [5, 4])
        endPiece = self.gb.tiles[move3.end[0]][move3.end[1]]

        # after jump
        self.assertEqual(gpiece3.player, endPiece.player)
        self.assertEqual(gpiece3.location, endPiece.location)
        self.assertEqual(gpiece3.isKing, endPiece.isKing)

        self.assertIsNone(self.gb.tiles[4][5])

        self.assertEqual(12, self.gb.blackPieces)
        self.assertEqual(11, self.gb.redPieces)

    def testKingMove(self):
        move1 = move([2, 7], [3, 6])    # moves black down
        move2 = move([5, 0], [4, 1])    # moves red up
        move3 = move([3, 6], [2, 7])    # moves black up
        move4 = move([4, 1], [5, 0])    # moves red down

        # Hardcode Kings
        self.gb.tiles[move1.start[0]][move1.start[1]].isKing = True
        self.gb.tiles[move2.start[0]][move2.start[1]].isKing = True

        self.gb.validateMove(move1) # Move pieces to move back
        self.gb.validateMove(move2)

        result1 = self.gb.validateMove(move3) # valid BLACK move
        result2 = self.gb.validateMove(move4)

        self.assertEqual(result1, (True, moveSuccess.NORMAL_MOVE))
        self.assertEqual(result2, (True, moveSuccess.NORMAL_MOVE))

if __name__ == '__main__':
    unittest.main()