#ifndef EDGE_ML_EARABLE_PDM_SENSOR_H
#define EDGE_ML_EARABLE_PDM_SENSOR_H

#include "EdgeML_Custom.h"
#include "SensorID_Earable.h"

class PDM_Sensor : public SensorInterface {
public:
    PDM_Sensor();

    void start() override;
    void end() override;

    void setBuffer(uint8_t * buffer);
    void get_data(int ID, byte * data) override;

    int get_sensor_count() override;

    static const int sensor_count = 1;
private:
    bool available = false;

    uint8_t * _buffer;

    //PDM_MIC

    //int16_t data[16];
};

extern PDM_Sensor pdm_sensor;


#endif //EDGE_ML_EARABLE_IMU_SENSOR_H
