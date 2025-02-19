import unittest
from GameBoard import *
from GamePiece import *

class TestGameBoard(unittest.TestCase):
    
    def setUp(self):
        self.gb = GameBoard()

    # validateMove tests
    def testBoundsCheck(self):
        move1 = Move([-1, -1], [2, 2])
        self.assertEqual(self.gb.validateMove(move1), (False, MoveError.START_OUT_OF_BOUNDS))
        move2 = Move([2, 2], [-1, -1])
        self.assertEqual(self.gb.validateMove(move2), (False, MoveError.END_OUT_OF_BOUNDS))

    def testEmptyStartTile(self):
        move1 = Move([0, 0], [1, 2])
        self.assertEqual(self.gb.validateMove(move1), (False, MoveError.START_TILE_EMPTY))

    def testNonEmptyEndTile(self):
        move1 = Move([2, 3], [1, 2])
        self.assertEqual(self.gb.validateMove(move1), (False, MoveError.END_TILE_OCCUPIED))

    def testCurrentPiece(self):
        move1 = Move([5, 0], [4, 1])
        self.assertEqual(self.gb.validateMove(move1), (False, MoveError.WRONG_PLAYER_PIECE))
    
    def testNonKingPieces(self):
        move1 = Move([2, 7], [3, 6])    # moves black down
        move2 = Move([5, 0], [4, 1])    # moves red up
        move3 = Move([3, 6], [2, 7])    # moves black up
        move4 = Move([4, 1], [5, 0])    # moves red down

        self.gb.validateMove(move1) # Move pieces to move back
        self.gb.validateMove(move2)

        result1 = self.gb.validateMove(move3) # invalid BLACK move
        self.gb.validateMove(Move([3, 6], [4, 7])) # valid move to change the turn
        result2 = self.gb.validateMove(move4)

        self.assertEqual(result1, (False, MoveError.BLACK_WRONG_DIRECTION))
        self.assertEqual(result2, (False, MoveError.RED_WRONG_DIRECTION))

    def testDiagonalMove(self):
        move1 = Move([2, 7], [3, 7])
        self.assertEqual(self.gb.validateMove(move1), (False, MoveError.NOT_DIAGONAL))

    def testInvalidDistance(self):
        move1 = Move([0, 1], [3, 4])
        self.assertEqual(self.gb.validateMove(move1), (False, MoveError.INVALID_DISTANCE))

    def testRegularMoveCase(self):
        move1 = Move([2, 7], [3, 6])
        self.assertEqual(self.gb.currentPlayer, Player.BLACK)
        result1 = self.gb.validateMove(move1)
        self.assertEqual(result1, (True, MoveSuccess.NORMAL_MOVE))
        self.assertEqual(self.gb.currentPlayer, Player.RED)

    def testRegularMoveFunction(self):
        move1 = Move([2, 7], [3, 6])
        gpiece1 = GamePiece(Player.BLACK, [2, 7])
        startPiece = self.gb.tiles[move1.start[0]][move1.start[1]]
        
        # before move
        self.assertEqual(gpiece1.player, startPiece.player)
        self.assertEqual(gpiece1.location, startPiece.location)
        self.assertEqual(gpiece1.isKing, startPiece.isKing)
        self.assertIsNone(self.gb.tiles[move1.end[0]][move1.end[1]])
        
        self.gb.regularMove(move1)
        # after move
        gpiece2 = GamePiece(Player.BLACK, [3, 6])
        endPiece = self.gb.tiles[move1.end[0]][move1.end[1]]

        self.assertEqual(gpiece2.player, endPiece.player)
        self.assertEqual(gpiece2.location, endPiece.location)
        self.assertEqual(gpiece2.isKing, endPiece.isKing)
        self.assertIsNone(self.gb.tiles[move1.start[0]][move1.start[1]])

    def testNoPieceToCapture(self):
        move1 = Move([2, 7], [4, 5])
        result1 = self.gb.validateMove(move1)
        self.assertEqual(result1, (False, MoveError.NO_PIECE_TO_CAPTURE))

    def testFriendlyFire(self):
        move1 = Move([1, 2], [3, 4])
        result1 = self.gb.validateMove(move1)
        self.assertEqual(result1, (False, MoveError.FRIENDLY_FIRE))

    def testCapturePiece(self):
        move1 = Move([2, 7], [3, 6])
        move2 = Move([5, 4], [4, 5])
        move3 = Move([3, 6], [5, 4])
        self.gb.validateMove(move1)
        self.gb.validateMove(move2)
        result1 = self.gb.validateMove(move3)
        self.assertEqual(result1, (True, MoveSuccess.CAPTURE_PIECE))

    def testOvertakeMoveFunction(self):
        move1 = Move([2, 7], [3, 6])
        move2 = Move([5, 4], [4, 5])
        move3 = Move([3, 6], [5, 4])

        self.gb.validateMove(move1)
        self.gb.validateMove(move2)

        gpiece1 = GamePiece(Player.BLACK, [3, 6])
        startPiece1 = self.gb.tiles[move1.end[0]][move1.end[1]]
        gpiece2 = GamePiece(Player.RED, [4, 5])
        startPiece2 = self.gb.tiles[move2.end[0]][move2.end[1]]
        
        # before jump
        self.assertEqual(gpiece1.player, startPiece1.player)
        self.assertEqual(gpiece1.location, startPiece1.location)
        self.assertEqual(gpiece1.isKing, startPiece1.isKing)
        
        self.assertEqual(gpiece2.player, startPiece2.player)
        self.assertEqual(gpiece2.location, startPiece2.location)
        self.assertEqual(gpiece2.isKing, startPiece2.isKing)

        self.gb.overtakeMove(move3, [4, 5])
        gpiece3 = GamePiece(Player.BLACK, [5, 4])
        endPiece = self.gb.tiles[move3.end[0]][move3.end[1]]

        # after jump
        self.assertEqual(gpiece3.player, endPiece.player)
        self.assertEqual(gpiece3.location, endPiece.location)
        self.assertEqual(gpiece3.isKing, endPiece.isKing)

        self.assertIsNone(self.gb.tiles[4][5])

        self.assertEqual(12, self.gb.blackPieces)
        self.assertEqual(11, self.gb.redPieces)

    def testKingMove(self):
        move1 = Move([2, 7], [3, 6])    # moves black down
        move2 = Move([5, 0], [4, 1])    # moves red up
        move3 = Move([3, 6], [2, 7])    # moves black up
        move4 = Move([4, 1], [5, 0])    # moves red down

        # Hardcode Kings
        self.gb.tiles[move1.start[0]][move1.start[1]].isKing = True
        self.gb.tiles[move2.start[0]][move2.start[1]].isKing = True

        self.gb.validateMove(move1) # Move pieces to move back
        self.gb.validateMove(move2)

        result1 = self.gb.validateMove(move3) # valid BLACK move
        result2 = self.gb.validateMove(move4)

        self.assertEqual(result1, (True, MoveSuccess.NORMAL_MOVE))
        self.assertEqual(result2, (True, MoveSuccess.NORMAL_MOVE))

    def testReset(self):
        move1 = Move([2, 7], [3, 6])
        gpiece1 = GamePiece(Player.BLACK, [2, 7])
        startPiece = self.gb.tiles[move1.start[0]][move1.start[1]]
        
        # before move
        self.assertEqual(gpiece1.player, startPiece.player)
        self.assertEqual(gpiece1.location, startPiece.location)
        self.assertEqual(gpiece1.isKing, startPiece.isKing)
        self.assertIsNone(self.gb.tiles[move1.end[0]][move1.end[1]])
        
        self.gb.regularMove(move1)
        # after move
        gpiece2 = GamePiece(Player.BLACK, [3, 6])
        endPiece = self.gb.tiles[move1.end[0]][move1.end[1]]

        self.assertEqual(gpiece2.player, endPiece.player)
        self.assertEqual(gpiece2.location, endPiece.location)
        self.assertEqual(gpiece2.isKing, endPiece.isKing)
        self.assertIsNone(self.gb.tiles[move1.start[0]][move1.start[1]])

        # reset
        self.gb.handleReset()
        tile1 = self.gb.tiles[move1.start[0]][move1.start[1]]
        tile2 = self.gb.tiles[move1.end[0]][move1.end[1]]
        
        self.assertIsNotNone(tile1)
        self.assertIsNone(tile2)

    def testSingleDoubleJump(self):
        move1 = Move([2, 5], [3, 4])
        move2 = Move([5, 0], [4, 1])
        move3 = Move([3, 4], [4, 3])
        move4 = Move([5, 6], [4, 7])
        move5 = Move([1, 6], [2, 5])
        move6 = Move([5, 2], [3, 4])
        move7 = Move([3, 4], [1, 6])    # double jump

        result1 = self.gb.validateMove(move1)
        result2 = self.gb.validateMove(move2)
        result3 = self.gb.validateMove(move3)
        result4 = self.gb.validateMove(move4)
        result5 = self.gb.validateMove(move5)

        self.assertEqual(result1, (True, MoveSuccess.NORMAL_MOVE))
        self.assertEqual(result2, (True, MoveSuccess.NORMAL_MOVE))
        self.assertEqual(result3, (True, MoveSuccess.NORMAL_MOVE))
        self.assertEqual(result4, (True, MoveSuccess.NORMAL_MOVE))
        self.assertEqual(result5, (True, MoveSuccess.NORMAL_MOVE))

        # perform double jump
        self.assertEqual(self.gb.currentPlayer, Player.RED)
        result6 = self.gb.validateMove(move6)
        self.assertEqual(self.gb.currentPlayer, Player.RED)
        result7 = self.gb.validateMove(move7)

        self.assertEqual(result6, (True, MoveSuccess.DOUBLE_JUMP))
        self.assertEqual(result7, (True, MoveSuccess.CAPTURE_PIECE))
        self.assertEqual(self.gb.currentPlayer, Player.BLACK)

    def testWinCondition(self):
        self.gb.tiles = [[None] * 8 for _ in range(8)]
        self.gb.tiles[3][3] = GamePiece(Player.BLACK, [3, 3])
        self.gb.tiles[4][4] = GamePiece(Player.RED, [4, 4])
        move1 = Move([3, 3], [5, 5])

        self.gb.blackPieces = 1 # hard code 1 piece each and placements
        self.gb.redPieces = 1

        result1 = self.gb.validateMove(move1)
        self.assertEqual(result1, (True, MoveSuccess.GAME_OVER))

if __name__ == '__main__':
    unittest.main()