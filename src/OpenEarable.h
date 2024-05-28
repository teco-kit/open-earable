//
// Earable EdgeML code created by Dylan Ray Roodt [https://github.com/Xraydylan] for TECO KIT
// More infos at: https://github.com/OpenEarable/open-earable
//
#ifndef EDGE_ML_EARABLE_EDGEML_EARABLE_H
#define EDGE_ML_EARABLE_EDGEML_EARABLE_H

#include "EdgeML.h"
#include <battery_service/Battery_Service.h>
#include "button_service/Button.h"
#include "led_service/LED_Service.h"

#include <audio_pdm/PDM_Mic.h>
#include <audio_pdm/WavRecorder.h>
#include <audio_pdm/Recorder.h>

#include <audio_play/WavPlayer.h>
#include <audio_play/JinglePlayer.h>
#include <audio_play/ToneGenerator.h>

#include <audio_play/Audio_Player.h>

#include <custom_sensor/SensorManager_Earable.h>
//#include <configuration_handler/Configuration_Handler.h>

#include <task_manager/TaskManager.h>

#include <sd_logger/SD_Logger.h>

#include <utility>

const String device_name = "OpenEarable";
const String firmware_version = "1.3.1b";
const String hardware_version = "1.3.0";

bool _data_logger_flag = false;

void data_callback(int id, unsigned int timestamp, uint8_t * data, int size);
void config_callback(SensorConfigurationPacket *config);

class OpenEarable {
public:
    OpenEarable() = default;

    void begin() {
        Serial.begin(0);

        _interface = new SensorManager_Earable();
        _battery = new Battery_Service();

        sd_manager.begin();

        edge_ml_generic.set_config_callback(config_callback);
        //edge_ml_generic.set_data_callback(data_callback);

        if (_debug) {
            _battery->debug(*_debug);
        }

        if (_data_logger_flag) {
            SD_Logger::begin();
        }

        // Can both be initialized without extra cost
        //bool success = pdm_mic_sensor.init();
        recorder.setDevice(&pdm_mic);
        bool success = recorder.begin();
        if (_debug) success ? _debug->println("PDM Ready!") : _debug->println("PDM FAIL!");

        audio_player.setDevice(&i2s_player);
        success = audio_player.begin();
        if (_debug) success ? _debug->println("Player Ready!") : _debug->println("Player FAIL!");

        edge_ml_generic.ble_manual_advertise();
        edge_ml_generic.set_ble_config(device_name, firmware_version, hardware_version);
        edge_ml_generic.set_custom(_interface);
        edge_ml_generic.begin();

        earable_btn.begin();
        _battery->begin();
        play_service.begin();
        button_service.begin();
        led_service.begin();

        task_manager.begin();

        BLE.advertise();
    };

    void update() {
        _battery->update();

        task_manager.update();

        //interrupt based
        //earable_btn.update();
    };

    void debug(Stream &stream) {
        _debug = &stream;
        edge_ml_generic.debug(stream);
    };

    void configure_sensor(SensorConfigurationPacket& config) {
        if (config.sensorId != PDM_MIC) {
            config.sampleRate = config.sampleRate; // * _rate_factor;
        }

        edge_ml_generic.configure_sensor(config);
    };

    void stop_all_sensors() {
        SensorConfigurationPacket config;
        config.sampleRate = 0;
        config.latency = 0;

        for (int i=0; i<SENSOR_COUNT; i++) {
            config.sensorId = i;
            edge_ml_generic.configure_sensor(config);
        }
    };

private:
    SensorManager_Earable * _interface{};   // Created new
    Battery_Service * _battery{};           // Created new

    Stream * _debug{};

    static void data_callback(int id, unsigned int timestamp, uint8_t * data, int size) {
        if (_data_logger_flag) {
            String data_string = edge_ml_generic.parse_to_string(id, data);
            SD_Logger::data_callback(id, timestamp, data_string);
        }
    }

    static void config_callback(SensorConfigurationPacket *config);
};

OpenEarable open_earable;

void OpenEarable::config_callback(SensorConfigurationPacket *config) {
    if (config->sensorId == PDM_MIC) Recorder::config_callback(config);
    else if (config->sensorId == BARO_TEMP) task_manager.begin(config->sampleRate, -1);
    else if (config->sensorId == ACC_GYRO_MAG) task_manager.begin(-1, config->sampleRate);
    else {
        if (i2s_player.is_running()) {
            i2s_player.stop();
            delay(1);
            i2s_player.start();
        }
    }
    //else task_manager.begin();
}

#endif //EDGE_ML_EARABLE_EDGEML_EARABLE_H
