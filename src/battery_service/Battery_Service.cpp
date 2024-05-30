#include "Battery_Service.h"

void Battery_Service::begin() {
    battery = new Battery_Earable();
    battery->begin();

    _setup_ble_service();
}

void Battery_Service::update() {
    if (battery->check_battery()) {
        const int battery_level = battery->get_battery_level();
        const int charing_state = battery->get_charging_state();
        // if (_debug) {
        //     _debug->print("Battery Level % is now: ");
        //     _debug->println(battery_level);
        //     _debug->println(charing_state);
        // }
        batteryLevelC->setValue(battery_level);
        chargingStateC->setValue(charing_state);
    }
}

void Battery_Service::_setup_ble_service() {
    batteryService = new BLEService(batteryServiceUuid);
    batteryLevelC = new BLEUnsignedCharCharacteristic(batteryUuid, BLERead | BLENotify);
    chargingStateC = new BLEUnsignedCharCharacteristic(chargingUuid, BLERead | BLENotify);

    // Battery level
    BLE.setAdvertisedService(*batteryService);
    batteryService->addCharacteristic(*batteryLevelC);
    batteryService->addCharacteristic(*chargingStateC);
    BLE.addService(*batteryService);
    batteryLevelC->setValue(0);
    chargingStateC->setValue(0);
}
