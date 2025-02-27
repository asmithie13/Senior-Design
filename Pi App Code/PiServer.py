from flask import Flask, request, jsonify

class PiServer:
    def __init__(self, name):
        self.app = Flask(name)
        self._registerRoutes()

    def _registerRoutes(self):
        @self.app.route('/getData/<status>/<message>', methods=['GET'])
        def getData(status, message):
            response = {
                'status': status,
                'message': message
            }
            return jsonify(response)

        @self.app.route('/sendData', methods=['POST'])
        def sendData():
            data = request.data.decode('utf-8')
            print(f"Received data: {data}")
            
            response = {
                'message': 'Data received successfully!',
                'status': 'success'
            }
            return jsonify(response)

    def run(self, host='0.0.0.0', port=5000):
        self.app.run(host=host, port=port)

#app = PiServer(__name__)
#app.run()