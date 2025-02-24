import bluetooth

def runBluetoothServer():
    # Create a Bluetooth server socket using RFCOMM protocol
    serverSocket = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
    serverSocket.bind(("", bluetooth.PORT_ANY))
    serverSocket.listen(1)

    # Get the port the server socket is listening on
    port = serverSocket.getsockname()[1]
    print(f"Waiting for connection on RFCOMM channel {port}...")

    try:
        bluetooth.advertise_service(
            serverSocket,
            "PythonBluetooth",
            service_classes=[bluetooth.SERIAL_PORT_CLASS],
            profiles=[bluetooth.SERIAL_PORT_PROFILE]
        )
    except Exception as e:
        print(f"An unexpected error occurred: {e}")
        return

    clientSocket, clientInfo = serverSocket.accept()    # CURRENTLY GETS STUCK HERE
    print(f"Accepted connection from {clientInfo}")

    try:
        while True:
            data = clientSocket.recv(1024)
            if not data:
                break

            message = data.decode('utf-8')
            print(f"Received from Swift: {message}")
            response = f"Echo: {message}"
            clientSocket.send(response.encode('utf-8'))
            print(f"Sent to Swift: {response}")

    except OSError as e:
        print(f"Socket error: {e}")
    finally:
        print("Closing connection...")
        clientSocket.close()
        serverSocket.close()

if __name__ == '__main__':
    runBluetoothServer()
