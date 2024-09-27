#include "BLEStream.h"
#include "EdgeML.h"
#include "../custom_sensor/PDM_Sensor.h"
#include "Recorder.h" // for debugging only

#define FILT_ORDER 2

const float a[FILT_ORDER][3] = {{1, -1.36511724,  0.47759225},
                                {1, -1.6117271 ,  0.74452084}};
const float b[FILT_ORDER][3] = {{9.33498613e-04, 1.86699723e-03, 9.33498613e-04},
                                {1, 2, 1}};


const int down_sample = 8;
int16_t ble_buffer[AUDIO_STREAM_PACKAGE_SIZE];
int16_t sos_buffer[down_sample];
int index_ble = 0;

BLEStream::BLEStream() {
    filter = new SOSFilter(FILT_ORDER, b, a);
}

BLEStream::~BLEStream() {
    delete filter;
}

bool BLEStream::begin() {
    (*stream)->open();

    start();
    return true;
}

void BLEStream::setSampleRate(int sampleRate) {
    _sampleRate = sampleRate;
}

void BLEStream::setChannels(int channels) {
    _channels = channels;
}

bool BLEStream::available() {
    return _stream;
}

void BLEStream::end() {
    if (_stream) stop();
    
    (*stream)->close();
}

void BLEStream::setStream(BufferedStream ** stream) {
    if (_stream) end();
    this->stream = stream;
}

int BLEStream::provide(int max_cont) {
    if (!_stream) return 0;

    int cont = min((*stream)->get_contiguous_blocks(), max_cont);

    if (!cont) return 0;

    if (initial_drop > 0) {
        const int n = min(initial_drop, cont);
        (*stream)->provide(n);
        initial_drop -= n;
        cont -= n;

        if (!cont) return n;
    }

    uint8_t *read_pointer = (*stream)->buffer.getReadPointer();
    const int block_size = (*stream)->buffer.getBlockSize();

    for (int i = 0; i<cont; i++) {
        for (int k = 0; k < block_size; k+=down_sample * sizeof(int16_t)) {
            memcpy(sos_buffer, read_pointer + i * block_size + k, down_sample * sizeof(int16_t));
            filter->update((int16_t *) sos_buffer, down_sample);
            ble_buffer[index_ble++] = sos_buffer[0];
            if (index_ble == AUDIO_STREAM_PACKAGE_SIZE) {
                index_ble = 0;

                sensorProvider.update_manager();
                pdm_sensor.setBuffer((uint8_t*) ble_buffer);
                sensorProvider.update_sensor(PDM_MIC, true);
                bleHandler_G.update();

                delay(8);
            }
        }
    }

    // Serial.print("PDM: ");
    // Serial.println(recorder.available() ? (*recorder.target->stream)->remaining() : -1);

    (*stream)->provide(cont);

    return cont;
}

void BLEStream::start() {
    if (_stream) return;

    filter->reset();
    index_ble = 0;

    _stream = true;
}

void BLEStream::stop() {
    if (!_stream) return;

    _stream = false;
}