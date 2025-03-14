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
    
    def updateLastMove(self, text):
        self.label.setText(text)
        print(text)
 
if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = Frontend()
    window.show()

    sys.exit(app.exec())