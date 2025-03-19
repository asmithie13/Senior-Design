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
    
        self.backend.latestMove.connect(self.updateLastMove)

        # Signals
        self.backend.lastAction.connect(self.updateLastAction)
        self.backend.resetSignal.connect(self.updateOnReset)
        self.backend.redPieces.connect(self.updateRedPieces)
        self.backend.bluePieces.connect(self.updateBluePieces)
        self.backend.redKings.connect(self.updateRedKings)
        self.backend.blueKings.connect(self.updateBlueKings)
        self.backend.redMoves.connect(self.updateRedMove)
        self.backend.blueMoves.connect(self.updateBlueMove)
        self.backend.redChance.connect(self.updateRedChance)
        self.backend.blueChance.connect(self.updateBlueChance)

    def updateLastAction(self, action):
        return
    
    def updateOnReset(self):
        self.lastActionLabel.setText("Last Action:")
        self.playerRedLabel.setText("Player RED:")
        self.playerBlueLabel.setText("Player BLUE:")
        self.redPiecesLeft.setText("Pieces Left:")
        self.bluePiecesLeft.setText("Pieces Left:")
        self.redKingLabel.setText("King Pieces:")
        self.blueKingLabel.setText("King Pieces:")
        self.redMoveLabel.setText("Last Move:")
        self.blueMoveLabel.setText("Last Move:")
        self.redWinLabel.setText("Win %:")
        self.blueWinLabel.setText("Win %:")
    
    def updateRedPieces(self, num):
        return
    
    def updateBluePieces(self, num):
        return
    
    def updateRedKings(self, num):
        return
    
    def updateBlueKings(self, num):
        return
    
    def updateRedMove(self, move):
        return
    
    def updateBlueMove(self, move):
        return
    
    def updateRedChance(self, chance):
        return
    
    def updateBlueChance(self, chance):
        return
    
    def updateLastMove(self, text):
        self.label.setText(text)
        print(text)
 
if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = Frontend()
    window.show()

    sys.exit(app.exec())