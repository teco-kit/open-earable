#ifndef INPUT_DEVICE_H
#define INPUT_DEVICE_H

#include "utils/Consumer.h"

class InputDevice : public Consumer {
public:
    virtual bool begin() = 0;
    virtual void end() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool available() = 0;

    virtual int setSampleRate(int _sampleRate) = 0;
    virtual int getSampleRate() = 0;
    virtual int setChannels(int _channels) = 0;
    virtual int getChannels() = 0;
    
    void setBuffer(uint8_t * buffer, int blockSize, int blockCount) {
        stream->buffer.set_buffer(buffer, blockSize, blockCount);
    }
};

#endif