from flask import Flask, request, jsonify
from GameBoard import *

class PiServer:
    def __init__(self, __name__, gb):
        self.app = Flask(__name__)
        self.gb = gb
        self._registerRoutes()

    def _registerRoutes(self):
        @self.app.route('/sendData', methods=['POST'])
        def sendData():
            data = request.data.decode('utf-8')
            print(f"Received data: {data}")
            
            result = self.gb.validateMove(Move([data[0], data[1]], [data[2], data[3]]))

            response = {
                'status': result[0],
                'message': result[1]
            }
            return jsonify(response)

    def run(self, host='0.0.0.0', port=5000):
        self.app.run(host=host, port=port)
