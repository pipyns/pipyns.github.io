#pragma once

#include "TDC1000.h"
#include "TDC7200.h"
#include "Timer.h"

class UltrasonicInterface {
  public:
    UltrasonicInterface(const int interruptPin);
    bool attachTDC1000(TDC1000 *usafe);
    bool attachTDC7200(TDC7200 *tof, const uint8_t tofMode);
    bool getSingleChannelMeasurement(const TDC1000::TxRxChannel channelIn, const TDC1000::TofMode modeIn, uint64_t &tofReading);
    bool bidirectionalSample(const TDC1000::TofMode modeIn, uint64_t &tof1, uint64_t &tof2);
  private:
        TDC1000 *m_usafe = nullptr;
        TDC7200 *m_tof = nullptr;
        Timer timer;
        const int m_interruptPin;
};
