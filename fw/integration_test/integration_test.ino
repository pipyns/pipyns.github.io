#include "TDC1000.h"
#include "TDC7200.h"
#include <FastLED.h>

// ----------------------------------------------------------------- //
//                    External Oscillator Rate                       //
// ----------------------------------------------------------------- //
#define OSC_FREQ_HZ           (8000000)

// ----------------------------------------------------------------- //
//                   TDC1000 IC SETUP: US to ToF                     //
// ----------------------------------------------------------------- //
#define PIN_TDC1000_RESET     (5)
#define PIN_TDC1000_SPI_CS    (6)

static TDC1000 usafe(PIN_TDC1000_SPI_CS, PIN_TDC1000_RESET, OSC_FREQ_HZ);

// ----------------------------------------------------------------- //
//                  TDC7200 IC SETUP: ToF to Digital                 //
// ----------------------------------------------------------------- //
#define NUM_STOPS             (1)
#define PIN_TDC7200_INT       (2)
#define PIN_TDC7200_ENABLE    (4)
#define PIN_TDC7200_SPI_CS    (7)

static TDC7200 tof(PIN_TDC7200_ENABLE, PIN_TDC7200_SPI_CS, OSC_FREQ_HZ);

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

  // Setup Time-of-Flight Interface
  int setupCountdown = 50;
  Serial.println("Setting Up ToF IC");
  while (!tof.begin() && setupCountdown >= 0) {
    delay(50);
    --setupCountdown;
  }
  if (setupCountdown < 0) {
    flashOnBoard(CRGB::Green, 3);
    Serial.println("UNABLE TO SETUP TOF IC");
    while(1) {}
  }

  // Setup Measurement Parameters for ToF Interface
  Serial.println("Configuring TOF Measurement");
  if (not tof.setupMeasurement(10, 2, NUM_STOPS, 1)) {
    flashOnBoard(CRGB::Blue, 3);
    Serial.println("UNABLE TO SETUP MEASUREMENTS FOR TOF IC");
    while(1) {}
  }

  // Setup Ultrasonic Interface
  setupCountdown = 50;
  Serial.println("Setting Up Ultrasonic IC");
  while (!usafe.begin() && setupCountdown >= 0) {
    delay(50);
    --setupCountdown;
  }
  
  if (setupCountdown < 0) {
    flashOnBoard(CRGB::White, 3);
    Serial.println("UNABLE TO SETUP ULTRASONIC IC");
    while(1) {}
  }
  
  Serial.println("Setup Successful");
}

void loop() {
  tof.startMeasurement();
  Serial.println("Starting Measurement");
  uint8_t measurementTimeout = 10;
  while (digitalRead(PIN_TDC7200_INT) == HIGH && measurementTimeout > 0) {
    Serial.println("Waiting...");
    delay(100);
    --measurementTimeout;
  }
  for (uint8_t stop_num = 1; stop_num <= NUM_STOPS; ++stop_num) {
    uint64_t timer;
    if (tof.readMeasurement(stop_num, timer)) {
      char buff[40];
      ui64toa(timer, buff, 10);
      Serial.print(F("\tTime-of-Flight [ps]: ")); Serial.print(buff); Serial.print(F("\n"));
    }
  }
}
