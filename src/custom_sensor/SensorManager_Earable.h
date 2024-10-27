#ifndef EDGE_ML_EARABLE_SENSORMANAGER_EARABLE_H
#define EDGE_ML_EARABLE_SENSORMANAGER_EARABLE_H

#include "SensorID_Earable.h"
#include "IMU_Sensor.h"
#include "BARO_Sensor.h"
#include "PDM_Sensor.h"

class SensorManager_Earable : public SensorManagerInterface {
public:
    void setup() override {
        IMU_Sensor * imu = new IMU_Sensor();
        BARO_Sensor * baro = new BARO_Sensor();
        //DummySensor * dummy = new DummySensor();
        //PDM_Sensor * mic = new PDM_Sensor();

        SensorInterface ** modules = new SensorInterface * [MODULE_COUNT_PHYSICAL] {imu, baro, &pdm_sensor};

        SensorManagerInterface::set_modules(modules);
        SensorManagerInterface::set_sensor_counts(SENSOR_COUNT, MODULE_COUNT_PHYSICAL);
        //SensorManagerInterface::set_special_sensors(SpecialSensors, SPECIAL_SENSOR_COUNT);

        SensorManagerInterface::set_sensor_configs(CONFIG);
    }
};


#endif //EDGE_ML_EARABLE_SENSORMANAGER_EARABLE_H
