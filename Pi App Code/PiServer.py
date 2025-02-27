from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route('/getData/<status>/<message>', methods=['GET'])
def getData(status, message):
    response = {
        'status': status,
        'message': message
    }
    return jsonify(response)

@app.route('/sendData', methods=['POST'])
def sendData():
    data = request.data.decode('utf-8')
    print(f"Received data: {data}")

    response = {
        'message': 'Data received successfully!',
        'status': 'success'
    }
    return jsonify(response)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
