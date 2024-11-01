#include "SOSFilter.h"

#include <utility>

SOSFilter::SOSFilter(int order, const float (*b)[3], const float (*a)[3]) {
    this->order = order;
    this->b = b;
    this->a = a;

    //buffer.resize(order, std::vector<float>(2, 0.0f));
    buffer = new float*[order];
    for (int i = 0; i < order; ++i) {
        buffer[i] = new float[2]{0.0f, 0.0f};  // Speicher für 2 Verzögerungen pro Sektion
    }
}

SOSFilter::~SOSFilter() {
    //reset();
}

void SOSFilter::reset() {
    for(int i = 0; i < order; i++) {
        //std::memset(buffer[i].data(), 0, 2 * sizeof(float));
        std::memset(buffer[i], 0, 2 * sizeof(float));
    }
}

void SOSFilter::update(int16_t * data, int length) {
    float y[order+1] = {0};
    for (int n = 0; n < length; n++) {
        y[0] = data[n];

        for (int k = 0; k < order; k++) {
            y[k+1] = b[k][0] * y[k] + buffer[k][0];
            buffer[k][0] = b[k][1] * y[k] - a[k][1] * y[k+1] + buffer[k][1];
            buffer[k][1] = b[k][2] * y[k] - a[k][2] * y[k+1];
        }

        data[n] = constrain(y[order],-1*(1<<15),(1<<15)-1);
    }
}