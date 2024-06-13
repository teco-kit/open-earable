#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include "EdgeML.h"

#include <audio_pdm/Recorder.h>
#include "audio_play/Audio_Player.h"

class TaskManager {
public:
    void begin(float baro_samplerate = -1, float imu_samplerate = -1);
    void update();
private:
    int _current_conf_num;

    const int _default_loop_delay = 50;
    int _baro_delay = _default_loop_delay;
    int _imu_delay = _default_loop_delay;

    //float _rate_factor = 1.25;

    int _edge_ml_delay = 100;
    unsigned int _edge_ml_last;
    unsigned int _baro_last;
    unsigned int _imu_last;

    int _overlap = 2; // Overlap time in ms
    int _buffer_interval_time;

    bool _buffer_flag = false;

    /*int _pdm_min_blocks = 5;
    int _play_min_blocks = 5; // MUST be 1 bigger than play_update

    int _pdm_update_blocks = 4;
    int _play_update_blocks = 4;*/

    float const alpha = 0.01;

    float _mean_pdm = 11.6;
    const float _std_pdm = 4.41;
    float _var_pdm = _std_pdm * _std_pdm; // * 4.41;

    float _mean_i2s = 5.7;
    const float _std_i2s = 1.7;
    float _var_i2s = _std_i2s * _std_i2s; // * 1.7;

    void update_edge_ml();
    int update_audio(Provider * provider, int n);
    bool check_overlap(Provider * provider);
};

extern TaskManager task_manager;

#endif //TASK_MANAGER_H