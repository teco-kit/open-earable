#include "Recorder.h"
#include "WavRecorder.h"

uint8_t PDM_BUFFER[pdm_b_size * pdm_b_count] __attribute__((aligned (16)));

Stream * _rec_debug{};

Recorder::Recorder() {

}

Recorder::~Recorder() {
    
}

void Recorder::setSampleRate(int sample_rate) {
    _sampleRate = sample_rate;
    if (device) _sampleRate = device->setSampleRate(sample_rate);
    if (target) target->setSampleRate(_sampleRate);
}

bool Recorder::begin() {
    if (_available) return true;
    if (!device || !target) return false;
    device->setBuffer(PDM_BUFFER, pdm_b_size, pdm_b_count);
    _sampleRate = device->setSampleRate(_sampleRate);
    target->setSampleRate(_sampleRate);
    target->begin();
    if (!target->available()) return false;
    _available = device->begin();
    if(!_available) target->end();
    return _available;
}

bool Recorder::available() {
    return _available;
}

void Recorder::end() {
    if (_available) return;
    device->end();
    target->end();
    _available = false;
}

void Recorder::debug(Stream &stream) {
    _rec_debug = &stream;
    _rec_debug->println("Recorder debug set correctly!");
}

void Recorder::print_info() {
    Serial.println("RECORDER INFO:");
    Serial.print("sample rate: ");
    Serial.println(_sampleRate);
    Serial.print("target: ");
    if (target) {
        Serial.println("SET");
        if (target->available()) {
            Serial.println("target: available");
            //Serial.print("taget samplerate: ");
            //Serial.println(target->);
        } else Serial.println("target: not available");
    } else Serial.println("NOT SET");

    Serial.print("device: ");
    if (device) {
        Serial.println("SET");
        if (device->available()) {
            Serial.println("device: available");
            Serial.print("device samplerate: ");
            Serial.println(device->getSampleRate());
        } else  Serial.println("device: not available");
    } else Serial.println("NOT SET");
}

void Recorder::record() {
    if (!_available) begin();
    if (_available) device->start();
    else {
        Serial.println("Failed to setup recorder!");
        print_info();
    }
}

/**
 * TODO: Buffer fertig abbarbeiten
*/
void Recorder::stop() {
    if (!_available) return;
    device->end();
    target->end();
    _available = false;
}

void Recorder::setDevice(InputDevice * device) {
    this->device = device;
    _sampleRate = device->setSampleRate(_sampleRate);

    if (target) {
        target->setStream(&(device->stream));
        target->setSampleRate(_sampleRate);
    }
}

void Recorder::setTarget(AudioTarget * target) {
    this->target = target;
    if (!target) return;
    target->setSampleRate(_sampleRate);
    if (device) this->target->setStream(&(device->stream));
}

void Recorder::config_callback(SensorConfigurationPacket *config) {
    // Check for PDM MIC ID
    if (config->sensorId != PDM_MIC) return;

    // Get sample rate
    int sample_rate = int(config->sampleRate);

    // End sensor if sample rate is 0
    if (sample_rate == 0) {
        recorder.stop();
        return;
    }

    // Check for valid sample rate
    recorder.setSampleRate(sample_rate);

    // Make sure that pdm mic is not running already!
    recorder.stop();

    // find the next available file name for the recording
    const String recording_dir = "Recordings";

    if (!sd_manager.exists(recording_dir)) sd_manager.mkdir(recording_dir);

    ExFile file;
    ExFile dir = sd_manager.sd->open(recording_dir);

    char fileName[64];
    char * split;

    int n = 1;

    // find highest Recording number
    while (file = dir.openNextFile()) {
        file.getName(fileName, sizeof(fileName));

        split = strtok(fileName, "_");
        if (strcmp(split,"Recording") == 0) {
            split = strtok(NULL, "_");
            n = max(n, atoi(split) + 1);
        }
    }

    // file name of the new recording
    String file_name = "/" + recording_dir + "/Recording_" + String(n) + "_" + String(millis()) + ".wav";

    if (_rec_debug) {
        _rec_debug->println("Recording:");
        _rec_debug->println(file_name);
    }

    // set WaveRecorder
    recorder.setTarget(new WavRecorder(file_name));

    // Start pdm mic
    recorder.record();
}

Recorder recorder;