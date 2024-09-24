#ifndef BLE_STREAM_SENSOR_H
#define BLE_STREAM_SENSOR_H

#include "EdgeML_Custom.h"
#include "AudioTarget.h"
#include <custom_sensor/SensorID_Earable.h>
#include "../audio_play/SOSFilter.h"

class BLEStream : public AudioTarget {
public:
    BLEStream();
    ~BLEStream();

    int provide(int n) override;
    bool available() override;
    bool begin() override;
    void end() override;
    void setStream(BufferedStream ** stream) override;

    void start();
    void stop();

    void setSampleRate(int sampleRate) override;
    void setChannels(int channels) override;
private:
    bool _stream = false;

    int initial_drop = 1;

    int _sampleRate = 0;
    int _channels = 1;

    SOSFilter * filter;
};

#endif //OPEN_EARABLE_PDM_MIC_SENSOR_H
