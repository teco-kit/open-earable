#include "JinglePlayer.h"

#ifndef NO_JINGLES

const uint16_t NOTIFICATION_WAV[] PROGMEM = {
    #include "jingles/NOTIFICATION_WAV.txt"
};

const uint16_t ERROR_WAV[] PROGMEM = {
    #include "jingles/ERROR_WAV.txt"
};

const uint16_t SUCCESS_WAV[] PROGMEM = {
    #include "jingles/SUCCESS_WAV.txt"
};

const uint16_t ALARM_WAV[] PROGMEM = {
    #include "jingles/ALARM_WAV.txt"
};

const uint16_t CLICK_WAV[] PROGMEM = {
    #include "jingles/CLICK_WAV.txt"
};

const uint16_t PING_WAV[] PROGMEM = {
    #include "jingles/PING_WAV.txt"
};

const uint16_t OPEN_WAV[] PROGMEM = {
    #include "jingles/OPEN_WAV.txt"
};

const uint16_t CLOSE_WAV[] PROGMEM = {
    #include "jingles/CLOSE_WAV.txt"
};

#else

const uint16_t NOTIFICATION_WAV[] PROGMEM = {};

const uint16_t ERROR_WAV[] PROGMEM = {};

const uint16_t SUCCESS_WAV[] PROGMEM = {};

const uint16_t ALARM_WAV[] PROGMEM = {};

const uint16_t CLICK_WAV[] PROGMEM = {};

const uint16_t PING_WAV[] PROGMEM = {};

const uint16_t OPEN_WAV[] PROGMEM = {};

const uint16_t CLOSE_WAV[] PROGMEM = {};

#endif


JinglePlayer * JinglePlayer::getJingle(Jingle jingle) {
    switch (jingle)
    {
    case SUCCESS:
        return new JinglePlayer(SUCCESS_WAV, sizeof(SUCCESS_WAV) / audio_b_size);
    case ERROR:
        return new JinglePlayer(ERROR_WAV, sizeof(ERROR_WAV) / audio_b_size);
    case ALARM:
        return new JinglePlayer(ALARM_WAV, sizeof(ALARM_WAV) / audio_b_size);
    case CLICK:
        return new JinglePlayer(CLICK_WAV, sizeof(CLICK_WAV) / audio_b_size);
    case NOTIFICATION:
        return new JinglePlayer(NOTIFICATION_WAV, sizeof(NOTIFICATION_WAV) / audio_b_size);
    case PING:
        return new JinglePlayer(PING_WAV, sizeof(PING_WAV) / audio_b_size);
    case OPEN:
        return new JinglePlayer(OPEN_WAV, sizeof(OPEN_WAV) / audio_b_size);
    case CLOSE:
        return new JinglePlayer(CLOSE_WAV, sizeof(CLOSE_WAV) / audio_b_size);
    default:
        break;
    }

    return NULL;
}

JinglePlayer::JinglePlayer(const uint16_t * jingle, int length) : jingle(jingle), length(length) {
    this->index = 0;
}

JinglePlayer::~JinglePlayer() {
    end();
}

bool JinglePlayer::begin() {
    if (!stream || !(*stream)) return false;
    
    if (_available) return true;

    // to enable preload
    _available = true;
    index = 0;

    //preload buffer
    preload_buffer();

    (*stream)->open();
    _available = (*stream)->available();
    return _available;
}

void JinglePlayer::end() {
    (*stream)->close();
    (*stream)->buffer.clear();
    _available = false;
}

bool JinglePlayer::available() {
    return _available;
}

void JinglePlayer::setStream(BufferedStream ** stream) {
    if (_available) end();
    this->stream = stream;
}

void JinglePlayer::preload_buffer() {
    int cont = provide(_preload_blocks);

    if (_preload_blocks - cont > 0) cont += provide(_preload_blocks - cont);

    Serial.print("preload blocks: ");
    Serial.println(cont);
}

int JinglePlayer::provide(int n) {
    if (!_available) return 0;

    int blocks = (*stream)->get_contiguous_blocks();
    //Serial.print("contiguous: ");
    //Serial.println(blocks);
    int cont = min(length-index, min(blocks, n));

    if (!cont) {
        (*stream)->close();
        return 0;
    }

    memcpy((*stream)->buffer.getWritePointer(), &(jingle[index * (*stream)->buffer.getBlockSize() / sizeof(int16_t)]), cont * (*stream)->buffer.getBlockSize());

    index += cont;

    (*stream)->provide(cont);

    if (cont < n) {
        (*stream)->close();
    }

    return cont;
}

float JinglePlayer::getSampleRate() {
    if (!_available) return -1;
    return 44100;
}

unsigned int JinglePlayer::get_size() {
    return length;
}

WAVConfigurationPacket JinglePlayer::get_config() {
    WAVConfigurationPacket wav_packet;

    wav_packet.mode = 3;

    /*wav_packet.state = 0;
    wav_packet.size = _name.length();

    memcpy(wav_packet.name, _name.c_str(), _name.length());*/

    return wav_packet;
}