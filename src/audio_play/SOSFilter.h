#ifndef OPEN_EARABLE_SOS_H
#define OPEN_EARABLE_SOS_H

#include "Arduino.h"
#include <vector>

#define SOS_ORDER 3

/**
 * Directorform II Transposed
 */
class SOSFilter {
public:
    SOSFilter(int order, const float (*b)[3], const float (*a)[3]);
    ~SOSFilter();

    void reset();
    void update(int16_t * data, int length);
private:
    int order;
    const float (*b)[3];
    const float (*a)[3];
    //std::vector<std::vector<float>> buffer;
    float **buffer;
};

#endif //OPEN_EARABLE_AUDIO_PLAYER_H
