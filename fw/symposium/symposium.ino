#include "Interfaces.h"
#include <WiFi.h>
#include <FastLED.h>
#include <Deneyap_Servo.h>

// ----------------------------------------------------------------- //
//                    EXTERNAL OSCILLATOR RATE                       //
// ----------------------------------------------------------------- //
#define OSC_FREQ_HZ           (8000000)

Timer sensorTimer;

// ----------------------------------------------------------------- //
//                           SERVO SETUP                             //
// ----------------------------------------------------------------- //
#define PIN_SERVO             (7)

Servo valve;

// ----------------------------------------------------------------- //
//                        WIFI + W.S. SETUP                          //
// ----------------------------------------------------------------- //
const char* ssid = "pip_capping";
const char* pass = "pip_capping";
byte server[] = {129, 97, 230, 83};

//const char* ssid = "Engineering Capstone";
//const char* pass = "capstone2024";
//byte server[] = {10, 33, 136, 231};

//const char* ssid = "SM-G950W7614";
//const char* pass = "wgdm4795";
//byte server[] = {192, 168, 179, 206};

//const char* ssid = "basekay484";
//const char* pass = "$basekay484$";
//byte server[] = {192, 168, 0, 30};
char httpBuff[100];

WiFiClient client;

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
  // By default: HIGH OUTPUT
  pinMode(0,OUTPUT);
  digitalWrite(0,LOW);

  // Config peripheral pins
  pinMode(PIN_TDC7200_INT,INPUT);
  pinMode(PIN_TDC1000_RESET,OUTPUT);
  pinMode(PIN_SERVO,OUTPUT);

  // Attach Servo
  valve.attach(PIN_SERVO);
  
  // Setup Power to OnBoard LED + Turn Off (Power-Saving)
  FastLED.addLeds<NEOPIXEL, NEO_DATA>(leds, NEO_SIZE);
  digitalWrite(NEO_POW, LOW);
  pinMode(NEO_POW, OUTPUT);
  FastLED.setBrightness(100);
  flashOnBoard(CRGB::Purple, 1);
  delay(1000);

  // Instantiate Serial + Wait for Availability
  Serial.begin(115200);
  while(!Serial.available()) { };

  // Setup WiFi Connection
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Waiting for WiFi Connection. . .");
  }
  Serial.print("IP Address: "); Serial.println(WiFi.localIP());

  while(!Serial.available()) { };

  bool setupStatus = true;
  setupStatus &= ultrasonicInterface.attachTDC1000(&usafe);
  setupStatus &= ultrasonicInterface.attachTDC7200(&tof, 1);

  if (!setupStatus) {
    Serial.println("Failed Initialization");
    flashOnBoard(CRGB::Red, 1);
    while(1) {};
  }
  
  // Indicate Successful Initialization
  Serial.println("Successful Initialization");
  flashOnBoard(CRGB::Green, 1);  
}

void loop() {

  // Bidirectional Sampling
  bool bidirectionalSuccess = false;
  uint64_t tofUp = 0ll;
  uint64_t tofDown = 0ll;
  bidirectionalSuccess = ultrasonicInterface.bidirectionalSample(TDC1000::TofMode::Mode1, tofUp, tofDown); 
  if (bidirectionalSuccess) {
    char buff1[40];
    char buff2[40];
    ui64toa(tofUp, buff1, 10);
    ui64toa(tofDown, buff2, 10);
    Serial.print(F("\tTime-of-Flight UP   [ps]: ")); Serial.print(buff1); Serial.print(F("\n"));
    Serial.print(F("\tTime-of-Flight DOWN [ps]: ")); Serial.print(buff2); Serial.print(F("\n"));
  }
  else {
    Serial.println("Error Obtaining Sensor Reading");
    tofUp = 0;
    tofDown = 0;
  }

  // Forward to SW-BE
  uint64_t dtof = tofUp > tofDown ? tofUp - tofDown : tofDown - tofUp;
  if (client.connect(server, 5000)) {
    snprintf(httpBuff, sizeof(httpBuff), "{\"device_id\":\"12345\",\"time\":%d,\"flow\":%f,\"tof\":%d}",millis()/1000,0.1,dtof);
    String a = httpBuff;
    Serial.println(a);
    client.println("POST /predict HTTP/1.1");
    client.println("Host: 10.33.136.231:5000");
    client.println("Content-Type: application/json; charset=UTF-8");
    client.print("Content-Length:"); client.print(a.length()); client.print("\r\n");
    client.println("Connection: close");
    client.println();
    client.println(a);
  }

  for(;;) {
    String proc = "";
    char instr = ' ';
    while (client.available()) {
      char c = client.read();
      proc += c;
      if(proc.endsWith("ESP_INSTR: ")) {
        while (!client.available()) { }
        instr = client.read();
        client.flush();
        break;
      }
    }
    
    if(instr == '0') {
      Serial.println("HERE - NOTHING");
    }
    else if(instr == '1') {
      Serial.println("HERE - CLOSE");
      valve.write(175);
    }
    else if(instr == '2') {
      Serial.println("HERE - OPEN");
      valve.write(112);
    }
    if (!client.connected()) {
      client.stop();
      break;
    }
  }

  // Restart Timer
  sensorTimer.stop();
  sensorTimer.start();
  
  // Sample once every 30s
  while(sensorTimer.read() < 10000) { delay(1000); }
  sensorTimer.stop();
}
