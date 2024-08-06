#ifndef _PDM2_H_INCLUDED
#define _PDM2_H_INCLUDED

#include <Arduino.h>
#include "Earable_Pins.h"
#include "InputDevice.h"

#ifndef analogPinToPinName
#include <pinDefinitions.h>
#endif

const int valid_sample_rates[] = {
        16000,
        20000,
        25000,
        31250,
        33333,
        40000,
        41667,
        50000,
        62500
};

const int sampleRate_default = 41667;  // 16000 Hz / 41667 Hz / 62500 Hz  // Default

class PDM_Mic : public InputDevice {
public:
    PDM_Mic();
    virtual ~PDM_Mic();

    bool begin() override;
    bool begin(int channels, int sampleRate);

    void end() override;

    void start() override;
    void stop() override;

    bool available() override;

    int setSampleRate(int sampleRate) override;

    bool consume() override;

    void onReceive(void(*)(void));

    //PORTENTA_H7 min -12 max 51
    //NANO 33 BLE SENSe min 0 max 80
    //NICLA_VISION min 0 max 8
    void setGain(int gain);

    void setPins(int dinPin, int clkPin);
    int setChannels(int channels) override;

    int getSampleRate() override;
    int getChannels() override;

    void setBlockBufferSizes(int blockSize, int blockCount);

    unsigned long get_buffer_hits();

    CircularBlockBuffer * get_buffer();

// private:
    void IrqHandler(bool halftranfer);

private:
    int _dinPin = EPIN_PDMDIN; //29
    int _clkPin = EPIN_PDMCLK; //28

    int _channels = 1;
    int _sampleRate = 16000;

    int _gain = 60;

    bool _first = true;

    unsigned long _buffer_hits = 0;

    void (*_onReceive)(void);

    bool _available = false;
    bool _running = false;

    bool checkSampleRateValid(int sampleRate);
};

extern PDM_Mic pdm_mic;

#endif
