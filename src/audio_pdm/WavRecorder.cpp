#include "WavRecorder.h"

WavRecorder::WavRecorder(String name) : _name(name) {
    _wavWriter = new WAVWriter();
}

WavRecorder::~WavRecorder() {
    delete _wavWriter;
}

bool WavRecorder::begin() {
    (*stream)->open();
    _wavWriter->setName(_name);
    _wavWriter->setSampleRate(_sampleRate);
    _wavWriter->setChannels(_channels);
    const bool writer_begin = _wavWriter->begin();
    if (writer_begin) start();
    return writer_begin;  
}

void WavRecorder::setSampleRate(int sampleRate) {
    _sampleRate = sampleRate;
}

void WavRecorder::setChannels(int channels) {
    _channels = channels;
}

bool WavRecorder::available() {
    return _stream;
}

void WavRecorder::end() {
    if (_stream) stop();
    _wavWriter->end();
    (*stream)->close();
}

void WavRecorder::setStream(BufferedStream ** stream) {
    if (_stream) end();
    this->stream = stream;
}

int WavRecorder::provide(int max_cont) {
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
    int size = (int) (*stream)->buffer.getBlockSize() * cont;

    bool written = _wavWriter->writeChunk(read_pointer, size);

    if (!written) {
        (*stream)->close();
        return 0;
    }

    (*stream)->provide(cont);

    return cont;
}

String WavRecorder::get_name() {
    return _name;
}

void WavRecorder::start() {
    if (_stream) return;

    _stream = true;
    _wavWriter->cleanFile();
    _wavWriter->writeHeader();
    _wavWriter->startRecording();
}

void WavRecorder::stop() {
    if (!_stream) return;

    _stream = false;
    _wavWriter->endRecording();
}