#include "Interfaces.h"
#include <FastLED.h>

// ----------------------------------------------------------------- //
//                    External Oscillator Rate                       //
// ----------------------------------------------------------------- //
#define OSC_FREQ_HZ           (8000000)

// ----------------------------------------------------------------- //
//                   TDC1000 IC SETUP: US to ToF                     //
// ----------------------------------------------------------------- //
#define PIN_TDC1000_RESET     (1)
#define PIN_TDC1000_SPI_CS    (2)
#define PIN_TDC1000_ENABLE    (6)

static TDC1000 usafe(PIN_TDC1000_ENABLE, PIN_TDC1000_SPI_CS, PIN_TDC1000_RESET, OSC_FREQ_HZ);

// ----------------------------------------------------------------- //
//                  TDC7200 IC SETUP: ToF to Digital                 //
// ----------------------------------------------------------------- //
#define NUM_STOPS             (1)
#define PIN_TDC7200_INT       (4)
#define PIN_TDC7200_ENABLE    (5)
#define PIN_TDC7200_SPI_CS    (3)

static TDC7200 tof(PIN_TDC7200_ENABLE, PIN_TDC7200_SPI_CS, OSC_FREQ_HZ);

// ----------------------------------------------------------------- //
//                         UI INTERFACE SETUP                        //
// ----------------------------------------------------------------- //
 
static UltrasonicInterface ultrasonicInterface(PIN_TDC7200_INT);

// ----------------------------------------------------------------- //
//                 ON-BOARD NEOPIXEL SETUP: RGB LED                  //
// ----------------------------------------------------------------- //
#define NEO_DATA              (18)
#define NEO_POW               (17)
#define NEO_SIZE              (1)

static CRGB leds[NEO_SIZE];

// ----------------------------------------------------------------- //
//                        HELPER FUNCTIONS                           //
// ----------------------------------------------------------------- //
static void flashOnBoard(CRGB colorCode, const uint8_t numFlash) {
  digitalWrite(NEO_POW, HIGH); // Power cycle implemented for power saving
  delay(100); // Needed for LED to reboot and not flash random color lol
  for (uint8_t i = numFlash + 1; i > 0; --i) {
    leds[0] = colorCode;
    FastLED.show();
    delay(500);
    if (i >= 0) {
      leds[0] = CRGB::Black;
      FastLED.show();
      delay(500);
    }
  }
  digitalWrite(NEO_POW, LOW);
}

static void ui64toa(uint64_t v, char * buf, uint8_t base) {
  int idx = 0;
  uint64_t w = 0;
  while (v > 0) {
    w = v / base;
    buf[idx++] = (v - w * base) + '0';
    v = w;
  }
  buf[idx] = 0;
  // Reverse char array
  for (int i = 0, j = idx - 1; i < idx / 2; i++, j--) {
    char c = buf[i];
    buf[i] = buf[j];
    buf[j] = c;
  }
}

// ----------------------------------------------------------------- //
//                         EXECUTABLE CODE                           //
// ----------------------------------------------------------------- //
void setup() {
  pinMode(0,OUTPUT);
  digitalWrite(0,LOW);
  pinMode(PIN_TDC7200_INT,INPUT);
  pinMode(PIN_TDC1000_RESET,OUTPUT);
  
  // Setup Power to OnBoard LED + Turn Off (Power-Saving)
  FastLED.addLeds<NEOPIXEL, NEO_DATA>(leds, NEO_SIZE);
  digitalWrite(NEO_POW, LOW);
  pinMode(NEO_POW, OUTPUT);
  FastLED.setBrightness(100);
  flashOnBoard(CRGB::Purple, 1);
  delay(1000);
  // Instantiate Serial + Wait for Availability
  Serial.begin(115200);
  delay(1000);
  while (!Serial.available()) { }

  bool setupStatus = true;
  setupStatus &= ultrasonicInterface.attachTDC1000(&usafe);
  setupStatus &= ultrasonicInterface.attachTDC7200(&tof, 1);

  if (!setupStatus) {
    Serial.println("Failed Initialization");
    while(1) {};
  }
}

void loop() {
  // Test single transducer
  bool measurementSuccess = false;
  uint64_t measurementValue = 0u;
  measurementSuccess = ultrasonicInterface.getSingleChannelMeasurement(TDC1000::TxRxChannel::Channel1, TDC1000::TofMode::Mode1, measurementValue); 

  if (measurementSuccess) {
    char buff[40];
    ui64toa(measurementValue, buff, 10);
    Serial.print(F("\tTime-of-Flight [ps]: ")); Serial.print(buff); Serial.print(F("\n"));
    delay(1000);
  }
  else {
    Serial.println("lol no");
  }

//  // Test bidirectional transducers
//  bool bidirectionalSuccess = false;
//  uint64_t tofUp = 0ll;
//  uint64_t tofDown = 0ll;
//  bidirectionalSuccess = ultrasonicInterface.bidirectionalSample(TDC1000::TofMode::Mode1, tofUp, tofDown); 
//  if (bidirectionalSuccess) {
//    char buff1[40];
//    char buff2[40];
//    ui64toa(tofUp, buff1, 10);
//    ui64toa(tofDown, buff2, 10);
//    Serial.print(F("\tTime-of-Flight UP   [ps]: ")); Serial.print(buff1); Serial.print(F("\n"));
//    Serial.print(F("\tTime-of-Flight DOWN [ps]: ")); Serial.print(buff2); Serial.print(F("\n"));
//    delay(500);
//  }
//  else {
//    Serial.println("lol no");
//  }
}
