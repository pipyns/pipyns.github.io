#include "TDC1000.h"
#include "TDC7200.h"

// ------------------------------------------------------------------//
//                     TDC1000 IC SETUP: US to ToF                   //
// ------------------------------------------------------------------//

#define PIN_TDC1000_RESET     (5)
#define PIN_TDC1000_SPI_CS    (6)

static TDC1000 usafe(PIN_TDC1000_SPI_CS, PIN_TDC1000_RESET);


// ------------------------------------------------------------------//
//                  TDC7200 IC SETUP: ToF to Digital                 //
// ------------------------------------------------------------------//
#define PIN_TDC7200_INT       (2)
#define PIN_TDC7200_ENABLE    (4)
#define PIN_TDC7200_SPI_CS    (7)
#define TDC7200_CLOCK_FREQ_HZ (8000000)

static TDC7200 tof(PIN_TDC7200_ENABLE, PIN_TDC7200_SPI_CS, TDC7200_CLOCK_FREQ_HZ);


void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:

}
