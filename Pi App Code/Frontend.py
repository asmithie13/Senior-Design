from PyQt6.QtWidgets import QApplication, QMainWindow
from PyQt6 import uic
import sys
from Backend import Backend

class Frontend(QMainWindow):
    def __init__(self):
        super().__init__()
        uic.loadUi('Pi App Code\Frontend_UI.ui', self)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = Frontend()
    backend = Backend()

    #window.lastMove.connect(backend.latestMove)

    window.show()
    sys.exit(app.exec())