#ifndef RECORDER_H
#define RECORDER_H

#include <custom_sensor/SensorID_Earable.h>
#include "EdgeML_Custom.h"

#include "utils/SDManager.h"
#include <generic/Debug.h>

#include "AudioTarget.h"
#include "utils/BufferedInputStream.h"
#include "InputDevice.h"

const int pdm_b_size = 6144; //4096;
const int pdm_b_count = 8;
extern uint8_t PDM_BUFFER[pdm_b_size * pdm_b_count] __attribute__((aligned (16)));

/*
State Controller
*/
class Recorder {
public:
    Recorder();
    ~Recorder();

    bool begin();
    void end();

    void record();
    void stop();
    //void pause();

    void setTarget(AudioTarget * target);
    void setDevice(InputDevice * device);
    void setSampleRate(int sample_rate);

    void print_info();

    static void config_callback(SensorConfigurationPacket * config);
    static void debug(Stream &stream);
    
    AudioTarget * target;
    InputDevice * device;

    bool available();
private:
    int _sampleRate;
    bool _running = false;
    bool _available = false;
};

extern Recorder recorder;

#endif //OPEN_EARABLE_AUDIO_PLAYER_H