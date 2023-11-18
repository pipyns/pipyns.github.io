#define NOP __asm__ __volatile__ ("nop\n\t")

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
}

void loop() {
  // put your main code here, to run repeatedly:

  PORTD |= 1<<PD3; //set PD3 high and delay 7 machine cycles
  NOP; // delay 62.5ns on a 16MHz AtMega
  NOP; // delay 62.5ns on a 16MHz AtMega
  NOP; // delay 62.5ns on a 16MHz AtMega
  NOP; // delay 62.5ns on a 16MHz AtMega
  NOP; // delay 62.5ns on a 16MHz AtMega
  NOP; // delay 62.5ns on a 16MHz AtMega
  NOP; // delay 62.5ns on a 16MHz AtMega 
  PORTD &= ~(1<<PD3); //set PD3 low
  NOP; // delay 62.5ns on a 16MHz AtMega
  NOP; // delay 62.5ns on a 16MHz AtMega
  NOP; // delay 62.5ns on a 16MHz AtMega
  NOP; // delay 62.5ns on a 16MHz AtMega
  NOP; // delay 62.5ns on a 16MHz AtMega
  NOP; // delay 62.5ns on a 16MHz AtMega
  NOP; // delay 62.5ns on a 16MHz AtMega 

}
