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
    
        self.backend.latestMove.connect(self.update_label)
    
    def updateLabel(self, text):
        """Slot that updates the label with received data."""
        print(text)
 
if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = Frontend()
    window.show()

    sys.exit(app.exec())