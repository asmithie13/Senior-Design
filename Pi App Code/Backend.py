from flask import Flask, request, jsonify
from Objects.GameBoard import *
from Objects.GamePiece import *
from Objects.enums import *
from PyQt6.QtCore import QObject, pyqtSignal, QThread
from PyQt6.QtWidgets import QApplication 
import sys

class Backend(QObject):
    latestMove = pyqtSignal(str)

    lastAction = pyqtSignal(object) # TODO
    redPieces = pyqtSignal(int)
    bluePieces = pyqtSignal(int)
    redKings = pyqtSignal(int) # TODO
    blueKings = pyqtSignal(int) # TODO
    redMoves = pyqtSignal(Move)
    blueMoves = pyqtSignal(Move)
    redChance = pyqtSignal(float) # TODO
    blueChance = pyqtSignal(float) # TODO

    def __init__(self):
        super().__init__()
        self.app = Flask(__name__)
        self.gb = GameBoard()
        self._registerRoutes()

    def _registerRoutes(self):
        @self.app.route('/sendData', methods=['POST'])
        def sendData():
            # Try to decode data
            try:
                data = request.data.decode('utf-8')
                print(f"Received data: {data}")
            except Exception as e:
                print(f"{PhoneError.DECODE_ERROR.value}: {e}")
                response = {
                'status': False,
                'message': PhoneError.DECODE_ERROR.value
                }
                return jsonify(response)
        
            # Reset case
            if data == "*":
                print("Reset Signal Sent")
                result = self.gb.handleReset()
                print(result[0])
                print(result[1].value)

                response = {
                'status': result[0],
                'message': result[1]
                }
                return jsonify(response)
            
            # Normal move case
            try:
                result = self.gb.validateMove(Move([int(data[0]), int(data[1])], [int(data[2]), int(data[3])]))
                self.latestMove.emit(data)
                print(result[0])
                print(result[1].value)
                response = {
                'status': result[0],
                'message': result[1]
                }

                # Update red piece count
                if result[0]:
                    self.redPieces.emit(self.gb.redPieces)
                    self.bluePieces.emit(self.gb.blackPieces)
                    # self.redKings
                    # self.blueKings
                    if self.gb.currentPlayer == Player.RED:
                        self.redMoves.emit(Move([int(data[0]), int(data[1])], [int(data[2]), int(data[3])]))
                    elif self.gb.currentPlayer == Player.BLACK:
                        self.blueMoves.emit(Move([int(data[0]), int(data[1])], [int(data[2]), int(data[3])]))


                return jsonify(response)
            
            except Exception as e:
                print(f"{PhoneError.INVALID_SIGNAL.value}: {e}")
                response = {
                'status': False,
                'message': PhoneError.INVALID_SIGNAL.value
                }
                return jsonify(response)
            
class FlaskThread(QThread):
    def __init__(self, backend):
        super().__init__()
        self.backend = backend

    def run(self):
        self.backend.app.run(host='0.0.0.0', port=5000, threaded=True) 

if __name__ == "__main__":
    app = QApplication(sys.argv)
    backend = Backend()
    flaskThread = FlaskThread(backend)
    flaskThread.start()
    sys.exit(app.exec())