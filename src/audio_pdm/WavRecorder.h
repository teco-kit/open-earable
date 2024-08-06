#ifndef WAV_RECORDER_SENSOR_H
#define WAV_RECORDER_SENSOR_H

#include "EdgeML_Custom.h"
#include "AudioTarget.h"
#include <custom_sensor/SensorID_Earable.h>

#include "utils/WAVWriter.h"

class WavRecorder : public AudioTarget {
public:
    WavRecorder(String name);
    ~WavRecorder();

    int provide(int n) override;
    bool available() override;
    bool begin() override;
    void end() override;
    void setStream(BufferedStream ** stream) override;

    void start();
    void stop();

    void setSampleRate(int sampleRate) override;
    void setGain(int gain);
    void setChannels(int channels);

    void enable_serial_data();
    void disable_serial_data();

    String get_name();
private:
    bool _stream = false;

    int initial_drop = 1;

    const String _name;
    int _sampleRate = 0;
    int _channels = 1;

    WAVWriter * _wavWriter;
};

//extern PDM_MIC_Sensor pdm_mic_sensor;

#endif //OPEN_EARABLE_PDM_MIC_SENSOR_H
