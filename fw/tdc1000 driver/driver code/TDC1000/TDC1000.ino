#include "TDC1000.h"

#define PIN_TDC1000_START     (2)
#define PIN_TDC1000_STOP      (3)
#define PIN_TDC1000_TRIGGER   (4)
#define PIN_TDC1000_RESET     (5)
//#define PIN_TDC1000_CHSEL     (7)
//#define PIN_TDC1000_ERRB      (8)
#define PIN_TDC1000_SPI_CS (6)
//#ifndef USE_SI5351
//#define PIN_TDC1000_CLKIN (10)
//#endif

//#define PIN_DEBUG1            (5)
//#define PIN_DEBUG2            (A0)

#define TDC1000_CLKIN_FREQ_HZ (8000000)
#define TDC1000_CLKIN_FREQ_DIV (TDC1000::TxFreqDivider::Div8)

/**
#ifdef USE_SI5351
#include <si5351.h>
#else
#include <TimerOne.h>
#define PWM_CYCLE_US (1000000 / TDC1000_CLKIN_FREQ_HZ)
#endif

#ifdef USE_SI5351
static Si5351 si5351;
#endif**/

static TDC1000 usafe(PIN_TDC1000_SPI_CS, PIN_TDC1000_RESET);

static volatile uint16_t tstart_us;
static volatile uint16_t tstop_us;
static void irqHandlerStart() {
  //Serial.println("irqhandler");
  if (not(tstart_us)) {
    tstart_us = micros();
#ifdef PIN_DEBUG1
    digitalWrite(PIN_DEBUG1, HIGH);
#endif
  }
}
static void irqHandlerStop() {
  Serial.println("irqhandler Stop");
  if (not(tstop_us)) {
    tstop_us = micros();
#ifdef PIN_DEBUG2
    digitalWrite(PIN_DEBUG2, HIGH);
#endif
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("-- Starting TDC1000 test --"));
  while (not usafe.begin()) {
    Serial.println(F("Failed to init TDC1000"));
    delay(1000);
  }
  Serial.println(F("TDC1000 init OK"));

  bool ok = true;
  ok &= usafe.setTriggerEdge(true);
  ok &= usafe.setTx(TDC1000_CLKIN_FREQ_DIV, 30 /*pulses*/, 31 /*shift*/, false /*damping*/);
  ok &= usafe.setRx(true /*multiEcho*/);
  ok &= usafe.setRxSensitivity(TDC1000::RxDacEchoThreshold::m220mV, TDC1000::RxPgaGain::g9dB, TDC1000::RxLnaFbMode::capacitive);
  ok &= usafe.setRepeat(TDC1000::TxRxCycles::x1, 1 /*expected pulses*/);
  ok &= usafe.setTofMeasuementStandard(TDC1000::T0::ClkInDiv1, TDC1000::TxAutoZeroPeriod::T0x64,
                                     TDC1000::TxEchoTimeoutPeriod::T0x1024, 30, true); 
  ok &= usafe.setTofMeasuementShort(TDC1000::T0::ClkInDiv1, TDC1000::TxAutoZeroPeriod::T0x128,
                                    TDC1000::TxBlankPeriod::T0x128, TDC1000::TxEchoTimeoutPeriod::T0x1024);
  ok &= usafe.setMeasureTOF(TDC1000::TxRxChannel::Channel1, TDC1000::TofMode::Mode0);
  //ok &= usafe.setMeasureTOF(TDC1000::TxRxChannel::Swap, TDC1000::TofMode::Mode2);
  usafe.dumpSettings(TDC1000_CLKIN_FREQ_HZ);

  if (not ok) {
    Serial.println(F("Failed to configure TDC1000"));
    while (1) {};
  }

  pinMode(PIN_TDC1000_START, INPUT);
  pinMode(PIN_TDC1000_STOP, INPUT);

  digitalWrite(PIN_TDC1000_TRIGGER, LOW);
  pinMode(PIN_TDC1000_TRIGGER, OUTPUT);

  //    digitalWrite(PIN_TDC1000_CHSEL, LOW);
  //    pinMode(PIN_TDC1000_CHSEL, OUTPUT);

  //    pinMode(PIN_TDC1000_ERRB, INPUT_PULLUP);    // open drain, active low

  // Configure PWM to generate a pulse train of TDC1000_CLKIN_FREQ_HZ [Hz]
  // which is to be used as TDC1000 CLKIN.
  /**
  Timer1.initialize(PWM_CYCLE_US);
  Timer1.pwm(PIN_TDC1000_CLKIN, 1023);
  Timer1.setPwmDuty(PIN_TDC1000_CLKIN, 512);**/ 

#ifdef PIN_DEBUG1
  pinMode(PIN_DEBUG1, OUTPUT);
#endif
#ifdef PIN_DEBUG2
  pinMode(PIN_DEBUG2, OUTPUT);
#endif

  attachInterrupt(digitalPinToInterrupt(PIN_TDC1000_START), irqHandlerStart, RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_TDC1000_STOP), irqHandlerStop, RISING);
}

inline uint16_t elapsedMicros(const uint16_t start_us) {
  return uint16_t(micros()) - start_us;
}


void loop() {
  int go = Serial.read(); 
  while (go == -1){
    go = Serial.read(); 
  }
  tstart_us = 0u;
  tstop_us = 0u;

  usafe.clearErrorFlags();
  usafe.resetStatemachine();

  // Trigger new measurement
  digitalWrite(PIN_TDC1000_TRIGGER, HIGH);
  digitalWrite(PIN_TDC1000_TRIGGER, LOW);

  bool timeout = false;
  unsigned long tstart = micros();
  for (;;) {
    if (tstop_us) break;
    if (micros() - tstart > 5000u) {
      timeout = true;
      break;
    }
  }
#ifdef PIN_DEBUG1
  digitalWrite(PIN_DEBUG1, LOW);
#endif
#ifdef PIN_DEBUG2
  digitalWrite(PIN_DEBUG2, LOW);
#endif

  bool sigWeak, noSig, sigHigh;
  usafe.getErrorFlags(sigWeak, noSig, sigHigh);

  if (timeout) Serial.print(F("timeout "));
  if (sigWeak) Serial.print(F("sigweak "));
  if (noSig) Serial.print(F("nosig "));
  if (sigHigh) Serial.print(F("sighigh "));

  Serial.println(tstop_us - tstart_us);
}