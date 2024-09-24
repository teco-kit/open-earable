#include "PDM_Sensor.h"
//#include "../audio_pdm/BLEStream.h"

PDM_Sensor::PDM_Sensor() {

}

void PDM_Sensor::start() {
    if (available) return;
    
    available = true;
}

void PDM_Sensor::end() {
    if (!available) return;
    
    available = false;
}

void PDM_Sensor::get_data(int sensorID, byte *data) {
    if (sensorID != PDM_MIC) {
        memset(data, 0, sizeof(int16_t) * AUDIO_STREAM_PACKAGE_SIZE); /////??????
        return;
    }

    memcpy(data, _buffer, sizeof(int16_t) * AUDIO_STREAM_PACKAGE_SIZE);
}

void PDM_Sensor::setBuffer(uint8_t * buffer) {
    _buffer = buffer;
}

int PDM_Sensor::get_sensor_count() {
    return sensor_count;
}

PDM_Sensor pdm_sensor;