#include "IMU_Logger.h"

#include <utility>

ExFatFile IMULogger::_file;
bool IMULogger::_opened = false;
char IMULogger::_buffer[LOGGER_BUFFER_SIZE];
int IMULogger::_index = 0;
String IMULogger::_name = "Imu.csv";
Stream * _imu_debug{};

bool IMULogger::begin() {
    _index = 0;
    if(!sd_manager.begin()) return false;
    return true;
}

void IMULogger::debug(Stream &stream) {
    _imu_debug = &stream;
    _imu_debug->println("IMULogger debug set correctly!");
}

void IMULogger::end() {
    //sd_manager.end();
}

void IMULogger::set_name(String name) {
    _name = std::move(name);
    _opened = false;
}

void IMULogger::data_callback(int id, unsigned int timestamp, const String & data_string) {
    if (id == -1) {
        dump_to_sd();
        _file.close();
        _opened = false;
        return;
    };

    String text = String(timestamp);
    text += ", " + data_string;
    text += "\r\n";

    if (text.length() + _index > LOGGER_BUFFER_SIZE) {
        dump_to_sd();
    }

    text.toCharArray(&(_buffer[_index]), text.length());
    _index += text.length() - 1; // -1 to remove null terminator
}

void IMULogger::config_callback(SensorConfigurationPacket *config) {
    
    if (config->sampleRate == 0) {
        if (_opened){
            dump_to_sd();
            _file.close();
            _opened = false;
        }
        return;
    }

    if (_imu_debug) _imu_debug->println("Initialising imu file");
    if (!open_file()){ 
        if (_imu_debug) _imu_debug->println("Error opening the IMU file");
        return;
    }
    write_header();
    if (_file.isOpen())
        task_manager.begin(-1, config->sampleRate);
}

void IMULogger::dump_to_sd() {
    if (!open_file()) return;
    if (_index == 0) return;
    sd_manager.write_block(&_file, (uint8_t*)_buffer, _index);
    memset(_buffer, 0, LOGGER_BUFFER_SIZE);
    _index = 0;
}

void IMULogger::write_header() {
    _index = 0;
    String header = "timestamp,acc_x,acc_y,acc_z,gyro_x,gyro_y,gyro_z,magn_x,magn_y,magn_z\n\r";
    header.toCharArray(&(_buffer[_index]), header.length());
    _index += header.length() - 1; // -1 to remove null terminator
    dump_to_sd();
}

bool IMULogger::open_file() {
    if (_opened) return true;
    // find the next available file name for the recording
    const String logs_dir = "Imu";

    if (!sd_manager.exists(logs_dir)) sd_manager.mkdir(logs_dir);

    ExFile file;
    ExFile dir = sd_manager.sd->open(logs_dir);

    char fileName[64];
    char * split;

    int n = 1;

    // find highest Recording number
    while (file = dir.openNextFile()) {
        file.getName(fileName, sizeof(fileName));

        split = strtok(fileName, "_");
        if (strcmp(split,"Imu") == 0) {
            split = strtok(NULL, "_");
            n = max(n, atoi(split) + 1);
        }
    }

    // file name of the new recording
    _name = "/" + logs_dir + "/Imu_" + String(n) + "_" + String(millis()) + ".csv";

    if (_imu_debug) {
        _imu_debug->println("Log filename:");
        _imu_debug->println(_name);
    }

    _file = sd_manager.openFile(_name, true);
    _opened = _file.isOpen();
    return _opened;
}






