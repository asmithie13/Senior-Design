from flask import Flask, request, jsonify

app = Flask(__name__)

# Example route to handle GET request
@app.route('/get_data', methods=['GET'])
def get_data():
    response = {
        'message': 'Hello from Raspberry Pi!',
        'status': 'success'
    }
    return jsonify(response)

# Example route to handle POST request
@app.route('/send_data', methods=['POST'])
def send_data():
    data = request.json  # Get JSON data from iOS app
    print(f"Received data: {data}")

    # Process the data or store it as needed
    response = {
        'message': 'Data received successfully!',
        'status': 'success'
    }
    return jsonify(response)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
