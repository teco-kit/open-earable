#include "BARO_Logger.h"

#include <utility>

ExFatFile BARO_Logger::_file;
bool BARO_Logger::_opened = false;
char BARO_Logger::_buffer[LOGGER_BUFFER_SIZE];
int BARO_Logger::_index = 0;
String BARO_Logger::_name = "Baro.csv";
Stream * _debug{};

bool BARO_Logger::begin() {
    _index = 0;
    if(!sd_manager.begin()) return false;
    if (_debug) _debug->println("Initialising baro file");
    if (!open_file()) return false;
    write_header();
    return _file.isOpen();
}

void BARO_Logger::debug(Stream &stream) {
    _debug = &stream;
    _debug->println("BAROLogger debug set correctly!");
}

void BARO_Logger::end() {
    //sd_manager.end();
}

void BARO_Logger::set_name(String name) {
    _name = std::move(name);
    _opened = false;
}

void BARO_Logger::data_callback(int id, unsigned int timestamp, const String & data_string) {
    if (id == -1) {
        dump_to_sd();
        _file.close();
        _opened = false;
        return;
    };

    String text = String(id);
    text += ", " + String(timestamp);
    text += ", " + data_string;
    text += "\r\n";

    if (text.length() + _index > LOGGER_BUFFER_SIZE) {
        dump_to_sd();
    }

    text.toCharArray(&(_buffer[_index]), text.length());
    _index += text.length() - 1; // -1 to remove null terminator
}

void BARO_Logger::dump_to_sd() {
    if (!open_file()) return;
    if (_index == 0) return;
    sd_manager.write_block(&_file, (uint8_t*)_buffer, _index);
    memset(_buffer, 0, LOGGER_BUFFER_SIZE);
    _index = 0;
}

void BARO_Logger::write_header() {
    _index = 0;
    String header = "ID, TIMESTAMP, Data1, Data2";
    header.toCharArray(&(_buffer[_index]), header.length());
    _index += header.length() - 1; // -1 to remove null terminator
    dump_to_sd();
}

bool BARO_Logger::open_file() {
    if (_opened) return true;
    // find the next available file name for the recording
    const String logs_dir = "Baro";

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
        if (strcmp(split,"Baro") == 0) {
            split = strtok(NULL, "_");
            n = max(n, atoi(split) + 1);
        }
    }

    // file name of the new recording
    _name = "/" + logs_dir + "/Baro_" + String(n) + "_" + String(millis()) + ".csv";

    if (_debug) {
        _debug->println("Log filename:");
        _debug->println(_name);
    }

    _file = sd_manager.openFile(_name, true);
    _opened = _file.isOpen();
    return _opened;
}






