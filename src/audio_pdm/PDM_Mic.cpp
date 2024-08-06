#include "PDM_mic.h"
#include "utils/BufferedOutputStream.h"

#if defined(ARDUINO_ARCH_NRF52840)

//#include <hal/nrf_pdm.h>
#include "nrf_pdm.h"

#define DEFAULT_PDM_GAIN     20
#define PDM_IRQ_PRIORITY     7

#define NRF_PDM_FREQ_1280K  (nrf_pdm_freq_t)(0x0A000000UL)               ///< PDM_CLK= 1.280 MHz (32 MHz / 25) => Fs= 20000 Hz [Ratio80 => Fs= 16000 Hz]
#define NRF_PDM_FREQ_2000K  (nrf_pdm_freq_t)(0x10000000UL)               ///< PDM_CLK= 2.000 MHz (32 MHz / 16) => Fs= 31250 Hz [Ratio80 => Fs= 25000 Hz]
#define NRF_PDM_FREQ_2667K  (nrf_pdm_freq_t)(0x15000000UL)               ///< PDM_CLK= 2.667 MHz (32 MHz / 12) => Fs= 41667 Hz [Ratio80 => Fs= 33333 Hz]
#define NRF_PDM_FREQ_3200K  (nrf_pdm_freq_t)(0x19000000UL)               ///< PDM_CLK= 3.200 MHz (32 MHz / 10) => Fs= 50000 Hz [Ratio80 => Fs= 40000 Hz]
#define NRF_PDM_FREQ_4000K  (nrf_pdm_freq_t)(0x20000000UL)               ///< PDM_CLK= 4.000 MHz (32 MHz /  8) => Fs= 62500 Hz [Ratio80 => Fs= 50000 Hz]

PDM_Mic::PDM_Mic() {
    _onReceive = NULL;
    stream = new BufferedOutputStream();
}

PDM_Mic::~PDM_Mic() {

}

/**
 * TODO: check at and of FILE
*/
bool PDM_Mic::consume() {
    stream->consume(true);
    return stream->available();
}

bool PDM_Mic::begin() {
    if (_available) return true;
    // Enable high frequency oscillator if not already enabled
    if (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {
        NRF_CLOCK->TASKS_HFCLKSTART = 1;
        while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) { }
    }

    // configure the sample rate and channels
    switch (_sampleRate) {
        case 16000:
            NRF_PDM->RATIO = ((PDM_RATIO_RATIO_Ratio80 << PDM_RATIO_RATIO_Pos) & PDM_RATIO_RATIO_Msk);
            nrf_pdm_clock_set(NRF_PDM_FREQ_1280K);
            break;
        case 20000:
            // Untested
            nrf_pdm_clock_set(NRF_PDM_FREQ_1280K);
            break;
        case 25000:
            // Untested
            NRF_PDM->RATIO = ((PDM_RATIO_RATIO_Ratio80 << PDM_RATIO_RATIO_Pos) & PDM_RATIO_RATIO_Msk);
            nrf_pdm_clock_set(NRF_PDM_FREQ_2000K);
            break;
        case 31250:
            // Untested
            nrf_pdm_clock_set(NRF_PDM_FREQ_2000K);
            break;
        case 33333:
            // Untested
            NRF_PDM->RATIO = ((PDM_RATIO_RATIO_Ratio80 << PDM_RATIO_RATIO_Pos) & PDM_RATIO_RATIO_Msk);
            nrf_pdm_clock_set(NRF_PDM_FREQ_2667K);
            break;
        case 40000:
            // Untested
            NRF_PDM->RATIO = ((PDM_RATIO_RATIO_Ratio80 << PDM_RATIO_RATIO_Pos) & PDM_RATIO_RATIO_Msk);
            nrf_pdm_clock_set(NRF_PDM_FREQ_3200K);
            break;
        case 41667:
            nrf_pdm_clock_set(NRF_PDM_FREQ_2667K);
            break;
        case 50000:
            // Untested
            /*if (high) {
                nrf_pdm_clock_set(NRF_PDM_FREQ_3200K);
            } else {
                NRF_PDM->RATIO = ((PDM_RATIO_RATIO_Ratio80 << PDM_RATIO_RATIO_Pos) & PDM_RATIO_RATIO_Msk);
                nrf_pdm_clock_set(NRF_PDM_FREQ_4000K);
            }*/
            NRF_PDM->RATIO = ((PDM_RATIO_RATIO_Ratio80 << PDM_RATIO_RATIO_Pos) & PDM_RATIO_RATIO_Msk);
            nrf_pdm_clock_set(NRF_PDM_FREQ_4000K);
            break;
        case 62500:
            // Untested
            nrf_pdm_clock_set(NRF_PDM_FREQ_4000K);
            break;
        default:
            Serial.print("Error: Unsupported PDM sample rate: ");
            Serial.println(_sampleRate);
            return false; // unsupported
    }

    switch (_channels) {
        case 2:
            nrf_pdm_mode_set(NRF_PDM_MODE_STEREO, NRF_PDM_EDGE_LEFTFALLING);
            break;

        case 1:
            nrf_pdm_mode_set(NRF_PDM_MODE_MONO, NRF_PDM_EDGE_LEFTFALLING);
            break;

        default:
            Serial.print("Error: Unsupported number of PDM channels: ");
            Serial.println(_channels);
            return false; // unsupported
    }

    if(_gain == -1) {
        _gain = DEFAULT_PDM_GAIN;
    }
    nrf_pdm_gain_set(_gain, _gain);

    // configure the I/O and mux
    pinMode(_clkPin, OUTPUT);
    digitalWrite(_clkPin, LOW);

    pinMode(_dinPin, INPUT);

    nrf_pdm_psel_connect(digitalPinToPinName(_clkPin), digitalPinToPinName(_dinPin));

    // clear events and enable PDM interrupts
    nrf_pdm_event_clear(NRF_PDM_EVENT_STARTED);
    nrf_pdm_event_clear(NRF_PDM_EVENT_END);
    nrf_pdm_event_clear(NRF_PDM_EVENT_STOPPED);
    nrf_pdm_int_enable(NRF_PDM_INT_STARTED | NRF_PDM_INT_STOPPED);

    // clear the buffer
    stream->buffer.reset();

    nrf_pdm_buffer_set((uint32_t*)stream->buffer.getWritePointer(), stream->buffer.getBlockSize() / (sizeof(int16_t) * 1));

    // set the PDM IRQ priority and enable
    NVIC_SetPriority(PDM_IRQn, PDM_IRQ_PRIORITY);
    NVIC_ClearPendingIRQ(PDM_IRQn);
    NVIC_EnableIRQ(PDM_IRQn);

    _available = true;
    return _available;
}

bool PDM_Mic::begin(int channels, int sampleRate/*, bool high*/) {
    _channels = channels;
    _sampleRate = sampleRate;

    return begin();
}

bool PDM_Mic::available() {
    return _available;
}

void PDM_Mic::start() {
    if (!_available || _running) return;

    _first = true;
    _running = true;
    
    // enable and trigger start task
    nrf_pdm_enable();
    nrf_pdm_event_clear(NRF_PDM_EVENT_STARTED);
    nrf_pdm_task_trigger(NRF_PDM_TASK_START);
}

void PDM_Mic::stop() {
    if (!_available || !_running) return;
    nrf_pdm_task_trigger(NRF_PDM_TASK_STOP);
    _running = false;
}

void PDM_Mic::end() {
    // disable PDM and IRQ
    nrf_pdm_disable();

    NVIC_DisableIRQ(PDM_IRQn);

    // Don't disable high frequency oscillator since it could be in use by RADIO

    // unconfigure the I/O and un-mux
    nrf_pdm_psel_disconnect();

    pinMode(_clkPin, INPUT);

    _available = false;
    _running = false;
}

void PDM_Mic::onReceive(void(*function)(void)) {
    _onReceive = function;
}

void PDM_Mic::setPins(int dinPin, int clkPin) {
    _dinPin = dinPin;
    _clkPin = clkPin;
}

int PDM_Mic::setChannels(int channels) {
    _channels = constrain(channels,1,2);
    return _channels;
}

int PDM_Mic::setSampleRate(int sampleRate) {
    _sampleRate = pdm_mic.checkSampleRateValid(sampleRate) ? sampleRate : sampleRate_default;
    return _sampleRate;
}

void PDM_Mic::setGain(int gain) {
    _gain = gain;
    if (_available) nrf_pdm_gain_set(_gain, _gain);
}

int PDM_Mic::getSampleRate() {
    return _sampleRate;
}

int PDM_Mic::getChannels() {
    return _channels;
}

void PDM_Mic::IrqHandler(bool halftranfer) {
    if (nrf_pdm_event_check(NRF_PDM_EVENT_STARTED)) {
        nrf_pdm_event_clear(NRF_PDM_EVENT_STARTED);
        if (stream->remaining() > 0) {

            if (!_first) {
                consume();
            } else {
                _first = false;
            }

            // switch to the next buffer
            nrf_pdm_buffer_set((uint32_t*)stream->buffer.getWritePointer(1), stream->buffer.getBlockSize() / (sizeof(int16_t) * 1));
            //stream->buffer.incrementWritePointer();

            // call receive callback if provided
            if (_onReceive) _onReceive();
        } else {
            // TODO:log collision
        }
    } else if (nrf_pdm_event_check(NRF_PDM_EVENT_STOPPED)) {
        nrf_pdm_event_clear(NRF_PDM_EVENT_STOPPED);
    } else if (nrf_pdm_event_check(NRF_PDM_EVENT_END)) {
        nrf_pdm_event_clear(NRF_PDM_EVENT_END);
    }
}

bool PDM_Mic::checkSampleRateValid(int sampleRate) {
    for (int rate : valid_sample_rates) {
        if (rate == sampleRate) {
            return true;
        }
    }
    return false;
}

unsigned long PDM_Mic::get_buffer_hits() {
    return stream->buffer.get_num_underflow();
}

CircularBlockBuffer *PDM_Mic::get_buffer() {
    return &stream->buffer;
}

extern "C" {
__attribute__((__used__)) void PDM_IRQHandler_v(void)
{
    pdm_mic.IrqHandler(true);
}
}

PDM_Mic pdm_mic;

#endif
