/*
  Blink

  Blinks the LP-MSPM0C1104 onboard LED using LED_BUILTIN.
  The LaunchPad LED on PA22 is active-low, so LOW turns the LED on
  and HIGH turns it off.
*/

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(250);
  digitalWrite(LED_BUILTIN, LOW);
  delay(250);
}
