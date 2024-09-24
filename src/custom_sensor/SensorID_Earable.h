#ifndef EDGE_ML_EARABLE_SENSORID_EARABLE_H
#define EDGE_ML_EARABLE_SENSORID_EARABLE_H

#include "EdgeML_Custom.h"

#define AUDIO_STREAM_PACKAGE_SIZE 16

const int SENSOR_COUNT = 3;
const int MODULE_COUNT_PHYSICAL = 3;
const int SPECIAL_SENSOR_COUNT = 1;

// In correct order ID ascending
enum SensorID {
    ACC_GYRO_MAG,
    BARO_TEMP,
    PDM_MIC
};

const int SpecialSensors[SPECIAL_SENSOR_COUNT] = {
        PDM_MIC
};

// In correct order ID ascending
enum ModuleID {
    MODULE_IMU,
    MODULE_BARO,
    MODULE_PDM,
    //MODULE_DUMMY
};

const SensorComponent ACC_COMPONENTS[] = {
        {"ACC", PARSE_TYPE_FLOAT, "X", "g"},
        {"ACC", PARSE_TYPE_FLOAT, "Y", "g"},
        {"ACC", PARSE_TYPE_FLOAT, "Z", "g"},
        {"GYRO", PARSE_TYPE_FLOAT, "X", "dps"},
        {"GYRO", PARSE_TYPE_FLOAT, "Y", "dps"},
        {"GYRO", PARSE_TYPE_FLOAT, "Z", "dps"},
        {"MAG", PARSE_TYPE_FLOAT, "X", "uT"},
        {"MAG", PARSE_TYPE_FLOAT, "Y", "uT"},
        {"MAG", PARSE_TYPE_FLOAT, "Z", "uT"}
};

const SensorComponent PRESSURE_TEMP_COMPONENTS[] = {
        {"TEMP", PARSE_TYPE_FLOAT, "Temperature", "°C"},
        {"BARO", PARSE_TYPE_FLOAT, "Pressure", "kPa"}
};

const SensorComponent PDM_MIC_COMPONENTS[] = {
        {"MIC", PARSE_TYPE_INT16, "0", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "1", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "2", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "3", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "4", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "5", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "6", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "7", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "8", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "9", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "10", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "11", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "12", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "13", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "14", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "15", "AMP"},
        /*{"MIC", PARSE_TYPE_INT16, "16", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "17", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "18", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "19", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "20", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "21", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "22", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "23", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "24", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "25", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "26", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "27", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "28", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "29", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "30", "AMP"},
        {"MIC", PARSE_TYPE_INT16, "31", "AMP"},*/
};

const SensorConfig CONFIG[SENSOR_COUNT] = {
        {
                "ACC_GYRO_MAG",
                ACC_GYRO_MAG,
                MODULE_IMU,
                9,
                ACC_COMPONENTS
        },
        {
                "PRESSURE_TEMP",
                BARO_TEMP,
                MODULE_BARO,
                2,
                PRESSURE_TEMP_COMPONENTS
        },
        {
                "PDM MIC",
                PDM_MIC,
                MODULE_PDM, //MODULE_DUMMY,
                AUDIO_STREAM_PACKAGE_SIZE,
                PDM_MIC_COMPONENTS
        },
};

#endif //EDGE_ML_EARABLE_SENSORID_EARABLE_H