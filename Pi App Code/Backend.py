from flask import Flask, request, jsonify
from Objects.GameBoard import *
from Objects.GamePiece import *
from Objects.enums import *

class Backend:
    def __init__(self):
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
                print(result[0])
                print(result[1].value)
                response = {
                'status': result[0],
                'message': result[1]
                }
                return jsonify(response)
            
            except Exception as e:
                print(f"{PhoneError.INVALID_SIGNAL.value}: {e}")
                response = {
                'status': False,
                'message': PhoneError.INVALID_SIGNAL.value
                }
                return jsonify(response)
            

    def run(self, host='0.0.0.0', port=5000):
        self.app.run(host=host, port=port)

if __name__ == "__main__":
    backend = Backend()
    backend.run()