from PyQt6.QtWidgets import QApplication, QMainWindow
from PyQt6 import uic
import sys
from Backend import *

class Frontend(QMainWindow):
    def __init__(self):
        super().__init__()
        uic.loadUi('Pi App Code/Frontend_UI.ui', self)

        self.backend = Backend()
        self.backendThread = FlaskThread(self.backend)
        self.backendThread.start()
    
        # Signals
        self.backend.lastAction.connect(self.updateLastAction)
        self.backend.resetSignal.connect(self.updateOnReset)
        self.backend.playerTurn.connect(self.updatePlayerTurn)
        self.backend.redPieces.connect(self.updateRedPieces)
        self.backend.bluePieces.connect(self.updateBluePieces)
        self.backend.redKings.connect(self.updateRedKings)
        self.backend.blueKings.connect(self.updateBlueKings)
        self.backend.redMoves.connect(self.updateRedMove)
        self.backend.blueMoves.connect(self.updateBlueMove)

    def updateLastAction(self, action):
        defaultStr = "Last Action: "
        defaultStr += action[1].value
        self.lastActionLabel.setText(defaultStr)
    
    def updateOnReset(self):
        self.lastActionLabel.setText("Last Action:")
        self.playerRedLabel.setText("Player RED: Your Turn!")
        self.playerBlueLabel.setText("Player BLUE:")
        self.redPiecesLeft.setText("Pieces Left: 12")
        self.bluePiecesLeft.setText("Pieces Left: 12")
        self.redKingLabel.setText("King Pieces:")
        self.blueKingLabel.setText("King Pieces:")
        self.redMoveLabel.setText("Last Move:")
        self.blueMoveLabel.setText("Last Move:")

    def updatePlayerTurn(self, player):
        if player == Player.RED:
            self.playerRedLabel.setText("Player RED: Your Turn!")
            self.playerBlueLabel.setText("Player BLUE:")
        else:
            self.playerRedLabel.setText("Player RED:")
            self.playerBlueLabel.setText("Player BLUE: Your Turn!")
    
    def updateRedPieces(self, num):
        defaultStr = "Pieces Left: "
        defaultStr += str(num)
        self.redPiecesLeft.setText(defaultStr)
    
    def updateBluePieces(self, num):
        defaultStr = "Pieces Left: "
        defaultStr += str(num)
        self.bluePiecesLeft.setText(defaultStr)
    
    def updateRedKings(self, arr):
        if not arr: return
        defaultStr = "King Pieces: "
        defaultStr += str(arr)
        self.redKingLabel.setText(defaultStr)
    
    def updateBlueKings(self, arr):
        if not arr: return
        defaultStr = "King Pieces: "
        defaultStr += str(arr)
        self.blueKingLabel.setText(defaultStr)
    
    def updateRedMove(self, move):
        defaultStr = "Last Move: "
        defaultStr += str(move.start[0]) + str(move.start[1]) + " to " + str(move.end[0]) + str(move.end[1])
        self.redMoveLabel.setText(defaultStr)

    def updateBlueMove(self, move):
        defaultStr = "Last Move: "
        defaultStr += str(move.start[0]) + str(move.start[1]) + " to " + str(move.end[0]) + str(move.end[1])
        self.blueMoveLabel.setText(defaultStr)
        
 
if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = Frontend()
    window.show()

    sys.exit(app.exec())