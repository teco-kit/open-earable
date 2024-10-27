#ifndef TEST_WAVWRITER_H
#define TEST_WAVWRITER_H

#include "Arduino.h"

#include "SDManager.h"

#include "WaveInfo.h"


class WAVWriter {
public:
    WAVWriter();
    ~WAVWriter();
    
    bool begin();
    void end();

    void cleanFile();

    void setName(String name);

    void setSampleRate(int sampleRate);
    void setChannels(int channels);

    bool writeHeader();

    bool startRecording();
    bool writeChunk(uint8_t *block, int size);
    bool endRecording();

private:
    ExFatFile file;

    String _name = "Recording.wav";

    WaveInfo info;

    bool writeHeaderSizes();
};


#endif //TEST_WAVWRITER_H
