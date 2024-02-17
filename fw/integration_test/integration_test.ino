#include "TDC1000.h"
#include "TDC7200.h"
#include <Adafruit_NeoPixel.h>

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
#define TDC7200_CLOCK_FREQ_HZ (8000000)

static TDC7200 tof(PIN_TDC7200_ENABLE, PIN_TDC7200_SPI_CS, OSC_FREQ_HZ);

// ----------------------------------------------------------------- //
//                 ON-BOARD NEOPIXEL SETUP: RGB LED                  //
// ----------------------------------------------------------------- //
#define NEO_DATA              (18)
#define NEO_POW               (17)
#define NEO_SIZE              (1)

static Adafruit_NeoPixel onBoardLED(NEO_SIZE, NEO_DATA, NEO_RGB + NEO_KHZ800);

// ----------------------------------------------------------------- //
//                        HELPER FUNCTIONS                           //
// ----------------------------------------------------------------- //
static void flashOnBoard(const uint8_t R, const uint8_t G, const uint8_t B) {
  onBoardLED.setPixelColor(0, onBoardLED.Color(R, G, B, 100));
  digitalWrite(NEO_POW, HIGH);
  onBoardLED.show();
  for (int reps = 5; reps <= 0; --reps) {
    delay(250);
    onBoardLED.setPixelColor(0, onBoardLED.Color(R, G, B, 0));
    onBoardLED.show();
    delay(250);
    onBoardLED.setPixelColor(0, onBoardLED.Color(R, G, B, 100));
    onBoardLED.show();
  }
  onBoardLED.setPixelColor(0, onBoardLED.Color(0, 0, 0, 0));
  onBoardLED.show();
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
  pinMode(NEO_POW, OUTPUT);
  digitalWrite(NEO_POW, LOW);

  // Instantiate Serial + Wait for Availability
  Serial.begin(115200);
  while (!Serial) {};

  // Setup Ultrasonic Interface
  int setupCountdown = 10;
  setupCountdown = 10;
  while (!usafe.begin() && setupCountdown >= 0) {
    delay(50);
    --setupCountdown;
  }
  if (setupCountdown < 0) {
    flashOnBoard(255u, 0u, 255u);
    Serial.println("UNABLE TO SETUP ULTRASONIC IC");
    while(1) {}; 
  }

  // Setup Time-of-Flight Interface
  setupCountdown = 10;
  while (!tof.begin() && setupCountdown >= 0) {
    delay(50);
    --setupCountdown;
  }
  if (setupCountdown < 0) {
    flashOnBoard(255u, 255u, 0u);
    Serial.println("UNABLE TO SETUP TOF IC");
    while(1) {}; 
  }

  // Setup Measurement Parameters for ToF Interface
  if (!tof.setupMeasurement(10, 1, NUM_STOPS, 2)) {
    flashOnBoard(50u, 200u, 50u);
    Serial.println("UNABLE TO SETUP MEASUREMENTS FOR TOF IC");
    while(1) {};
  }
}

void loop() {
  tof.startMeasurement();
  while (digitalRead(PIN_TDC7200_INT) == HIGH) {};
  for (uint8_t stop = 1; stop <= NUM_STOPS; ++stop) {
    uint64_t time;
    if (tof.readMeasurement(stop, time)) {
      char buff[40];
      ui64toa(time, buff, 10);
      Serial.print(F("\tTime-of-Flight [ps]: ")); Serial.print(buff); Serial.print(F("\n"));
    }
  }
}
