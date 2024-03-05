#include "Interfaces.h"
#include <Arduino.h>

UltrasonicInterface::UltrasonicInterface(const int interruptPin) : m_interruptPin(interruptPin) {}

bool UltrasonicInterface::attachTDC1000(TDC1000 *usafe) {
  m_usafe = usafe;
  if (!m_usafe->begin()) {
    return false;
  }
  return true;
}

bool UltrasonicInterface::attachTDC7200(TDC7200 *tof) {
  m_tof = tof;
  if (!m_tof->begin()) {
    return false;
  }
  if (!m_tof->setupMeasurement(10, 2, 1, 1)) {
      return false;
  }
  return true;
}

bool UltrasonicInterface::getSingleChannelMeasurement(const TDC1000::TxRxChannel channelIn, const TDC1000::TofMode modeIn, uint64_t &tofReading) {
    // Set sampling timeout
    this->timer.start();

    // Clear return var
    tofReading = 0ull;

    // Configure Channel and Mode for Measurement
    m_usafe->setMeasureTOF(channelIn, modeIn);
    
    // While no reading is found
    while (tofReading == 0ull) {
        Serial.println(timer.read());
        if (timer.read() > 2500) break;
        m_tof->startMeasurement();
        while (digitalRead(m_interruptPin) == HIGH) {
            // TDC7200 has built-in timeout for operation
            // Serial.println("Waiting...");
            delay(10);
        }
        if (m_tof->readMeasurement(1, tofReading)) {
            this->timer.stop();
            return true;
        }
    }
    this->timer.stop();
    return false;
}

bool UltrasonicInterface::bidirectionalSample(const TDC1000::TofMode modeIn, uint64_t &tof1, uint64_t &tof2) {
    tof1 = 0ull;
    tof2 = 0ull;
    bool sampleStatus = true;
    sampleStatus &= this->getSingleChannelMeasurement(TDC1000::TxRxChannel::Channel1, modeIn, tof1);
    sampleStatus &= this->getSingleChannelMeasurement(TDC1000::TxRxChannel::Channel2, modeIn, tof2);
    if (!sampleStatus) {
        return false;
    }
    return true;
}
