from PyQt6.QtWidgets import QApplication, QMainWindow
from PyQt6 import uic  # Used to load .ui file dynamically
import sys
from Backend import Backend  # Import backend worker

class Frontend(QMainWindow):
    def __init__(self):
        super().__init__()
        uic.loadUi('Pi App Code\Frontend_UI.ui', self)

def main():
    app = QApplication(sys.argv)
    window = Frontend()
    window.show()
    sys.exit(app.exec())

if __name__ == "__main__":
    main()