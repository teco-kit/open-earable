#include "TaskManager.h"

void TaskManager::update() {
    update_edge_ml();

    if (_buffer_flag) return;
    _buffer_flag = true;

    const int _min_blocks = 2;
    const int _max_blocks = 4;
    //const float STD = 0.5;

    /*Serial.print("Play: ");
    Serial.print(audio_player.available() ? (*audio_player.source->stream)->remaining() : -1);
    Serial.print(" PDM: ");
    Serial.println(recorder.available() ? (*recorder.target->stream)->remaining() : -1);*/

    Provider * provider;

    int _blocks_handled = 0;
    
    while (!check_overlap(NULL) && max(recorder.available() ? (*recorder.target->stream)->ready() : 0, audio_player.available() ? (*audio_player.source->stream)->ready() : 0) >= _min_blocks) {
        // check priority
        float diff = (recorder.available() ? (*recorder.target->stream)->remaining() : pdm_b_count) - (audio_player.available() ? (*audio_player.source->stream)->remaining() : audio_b_count);
        //float mean = (pdm_mic_sensor.remaining_blocks() + audio_player.remaining_blocks()) / 2.0;
        int blocks = min(abs(diff) + _min_blocks / 2, _max_blocks - _blocks_handled); //max(mean * STD, min_update / 2); //min_update / 2;
        //int blocks = 1;

        if (audio_player.available() && diff > 0) {
            provider = audio_player.source;
        } else if (recorder.available()) {
            provider = recorder.target;
        } else {
            break; // needed?
        }

        long start = millis();
        //_blocks_handled += update(provider, min(blocks,(*provider->stream)->ready()));
        int cont = update_audio(provider, min(blocks,(*provider->stream)->ready()));
        float _time = millis() - start;
        
        _blocks_handled += cont;

        /*if (cont > 0) {
            if (provider == audio_player.source) {
                _mean_i2s = (1-alpha) * _mean_i2s + alpha * _time / cont;
                _var_i2s = (1-alpha) * _var_i2s + alpha * powf(_time / cont - _mean_i2s, 2);
            } else if (provider == recorder.target){
                _mean_pdm = (1-alpha) * _mean_pdm + alpha * _time / cont;
                _var_pdm = (1-alpha) * _var_pdm + alpha * powf(_time / cont - _mean_pdm , 2);
            }
        }*/

        if (_blocks_handled >= _max_blocks) break;
    }

    /*long now = millis();
    Serial.print(now - (long)_edge_ml_last - (long)_edge_ml_delay);
    Serial.print(", ");
    Serial.println(_blocks_handled);*/

    /*Serial.print("I2S: ");
    Serial.print(_mean_i2s);
    Serial.print(", ");
    Serial.print(sqrt(_var_i2s));

    Serial.print(" PDM: ");
    Serial.print(_mean_pdm);
    Serial.print(", ");
    Serial.println(sqrt(_var_pdm));*/
}

void TaskManager::update_edge_ml() {
    unsigned int now = millis();
    if (now - _baro_last >= _baro_delay) {
        sensorProvider.update_sensor(BARO_TEMP, true);
        _baro_last = now;
    }
    if (now - _imu_last >= _imu_delay) {
        sensorProvider.update_sensor(ACC_GYRO_MAG, true);
        _imu_last = now;
    }
    if (now - _edge_ml_last >= _edge_ml_delay) {
        bleHandler_G.update();
        //Serial.print(" edge_ml time: ");
        //Serial.println(millis() - now);

        _edge_ml_last = now;
        _buffer_flag = false;
        //now = millis();
        // BLE.poll();
    }
}

int TaskManager::update_audio(Provider * provider, int max_buffers) {
    if (max_buffers <= 0 || (*provider->stream)->ready() == 0) return 0;

    int cont = provider->provide(max_buffers);

    // Make sure that time limit is not reached
    // Compute rest: rest 0 => good; rest == total blocks => bad and return
    if (!cont || max_buffers == cont || check_overlap(provider)) return cont;
    int cont2 = provider->provide(max_buffers - cont);
    return cont + cont2;
}

void TaskManager::begin(float baro_samplerate, float imu_samplerate) {
    if (baro_samplerate >= 0) _baro_delay = (int) baro_samplerate > 0 ? (1000.0/baro_samplerate) : _default_loop_delay;
    if (imu_samplerate >= 0) _imu_delay = (int) imu_samplerate > 0 ? (1000.0/imu_samplerate) : _default_loop_delay;

    /*float edge_rate = max(baro_samplerate, imu_samplerate);

    if (edge_rate <= 0) {
        edge_rate = _default_loop_rate;
    }

    _edge_ml_delay = (int)(1000.0/edge_rate);*/
    _edge_ml_last = millis();
    _baro_last = _edge_ml_last;
    _imu_last = _edge_ml_last;

    _buffer_interval_time = _edge_ml_delay - _overlap;

    _buffer_flag = false;
}

bool TaskManager::check_overlap(Provider * provider) {
    float est = 0;
    if (provider == audio_player.source) {
        est = _mean_i2s + 2 * _std_i2s;
    } else if (provider == recorder.target) {
        est = _mean_pdm + 2 * _std_pdm;
    }
    return (millis() - _edge_ml_last + est > _buffer_interval_time);
}

TaskManager task_manager;