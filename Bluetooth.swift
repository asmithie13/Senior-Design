//
//  Bluetooth.swift
//  SeniorDesign
//
//  Created by Dillon Hepler on 2/24/25.
//


import Foundation
import CoreBluetooth
import Combine

class BluetoothManager: NSObject, ObservableObject, CBCentralManagerDelegate, CBPeripheralDelegate {
    // Published property to notify the UI of validation results
    @Published var validationResult: String?

    // Core Bluetooth properties
    private var centralManager: CBCentralManager!
    private var raspberryPiPeripheral: CBPeripheral?
    private var moveCharacteristic: CBCharacteristic?
    private var validationCharacteristic: CBCharacteristic?

    // UUIDs for the service and characteristics CHANGE TO MATCH RASPBERRY PI
    private let serviceUUID = CBUUID(string: "12345678-1234-1234-1234-123456789012")
    private let moveCharacteristicUUID = CBUUID(string: "12345678-1234-1234-1234-123456789013")
    private let validationCharacteristicUUID = CBUUID(string: "12345678-1234-1234-1234-123456789014")

    // Initialize the central manager
    override init() {
        super.init()
        centralManager = CBCentralManager(delegate: self, queue: nil)
    }

    // MARK: - CBCentralManagerDelegate Methods

    // Called when the Bluetooth state updates
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        if central.state == .poweredOn {
            // Start scanning for peripherals advertising our service
            centralManager.scanForPeripherals(withServices: [serviceUUID], options: nil)
        }
    }

    // Called when a peripheral is discovered
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String: Any], rssi RSSI: NSNumber) {
        raspberryPiPeripheral = peripheral
        centralManager.stopScan()
        centralManager.connect(peripheral, options: nil)
    }

    // Called when the peripheral is connected
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        peripheral.delegate = self
        peripheral.discoverServices([serviceUUID])
    }

    // MARK: - CBPeripheralDelegate Methods

    // Called when services are discovered
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        guard let services = peripheral.services else { return }
        for service in services {
            if service.uuid == serviceUUID {
                peripheral.discoverCharacteristics([moveCharacteristicUUID, validationCharacteristicUUID], for: service)
            }
        }
    }

    // Called when characteristics are discovered
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        guard let characteristics = service.characteristics else { return }
        for characteristic in characteristics {
            if characteristic.uuid == moveCharacteristicUUID {
                moveCharacteristic = characteristic
            } else if characteristic.uuid == validationCharacteristicUUID {
                validationCharacteristic = characteristic
                // Subscribe to notifications for validation updates
                peripheral.setNotifyValue(true, for: characteristic)
            }
        }
    }

    // Called when a characteristic value is updated (e.g., validation result)
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        if characteristic.uuid == validationCharacteristicUUID,
           let data = characteristic.value,
           let response = String(data: data, encoding: .utf8) {
            validationResult = response
        }
    }

    // MARK: - Public Methods

    // Send a move to the Raspberry Pi
    func sendMove(_ move: String) {
        guard let peripheral = raspberryPiPeripheral,
              let characteristic = moveCharacteristic,
              let data = move.data(using: .utf8) else { return }
        peripheral.writeValue(data, for: characteristic, type: .withResponse)
    }
}
