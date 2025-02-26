from pydbus import SystemBus
from gi.repository import GLib
from dbus.mainloop.glib import DBusGMainLoop

SERVICE_UUID = "12345678-1234-5678-1234-56789abcdef0"
CHARACTERISTIC_UUID = "12345678-1234-5678-1234-56789abcdef1"

class BLECharacteristic:
    def __init__(self, uuid, value=b"Hello from Python BLE"):
        self.uuid = uuid
        self.value = value

    def ReadValue(self, options):
        print("Client read request")
        return list(self.value)

    def WriteValue(self, value, options):
        self.value = bytes(value)
        print(f"Received from client: {self.value.decode('utf-8')}")

class BLEService:
    def __init__(self):
        self.characteristic = BLECharacteristic(CHARACTERISTIC_UUID)

    def register(self):
        bus = SystemBus()
        adapter = bus.get("org.bluez", "/org/bluez/hci0")
        adapter.RegisterApplication("/", {})

        print("BLE GATT Server started...")

if __name__ == "__main__":
    DBusGMainLoop(set_as_default=True)
    server = BLEService()
    server.register()
    loop = GLib.MainLoop()
    loop.run()
