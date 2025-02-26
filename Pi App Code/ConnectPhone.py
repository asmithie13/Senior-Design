import asyncio
from bleak import BleakServer, BleakGATTService, BleakGATTCharacteristic

SERVICE_UUID = "12345678-1234-5678-1234-56789abcdef0"
CHARACTERISTIC_UUID = "12345678-1234-5678-1234-56789abcdef1"

class MyGattServer:
    def __init__(self):
        self.server = None
        self.value = bytearray(b"Hello from Python BLE")

    async def on_read(self, characteristic):
        """Handle read requests from the client."""
        print("Client read request")
        return self.value

    async def on_write(self, characteristic, value):
        """Handle write requests from the client."""
        self.value = value
        print(f"Received from client: {value.decode('utf-8')}")

    async def run(self):
        """Start the BLE GATT server."""
        self.server = BleakServer()

        # Create a GATT service with a characteristic
        service = BleakGATTService(SERVICE_UUID)
        characteristic = BleakGATTCharacteristic(
            CHARACTERISTIC_UUID,
            properties=["read", "write"],
            read=self.on_read,
            write=self.on_write,
        )

        service.add_characteristic(characteristic)
        self.server.add_service(service)

        print("Starting BLE GATT Server...")
        await self.server.start()
        print("BLE Server running...")

        while True:
            await asyncio.sleep(1)  # Keep server alive

if __name__ == "__main__":
    asyncio.run(MyGattServer().run())
