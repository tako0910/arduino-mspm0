/*
  Blink

  Blinks the onboard LED using LED_BUILTIN.
  On both LP-MSPM0C1104 and LP-MSPM0G3507 the default LaunchPad LED is
  wired active-low, so LOW turns the LED on and HIGH turns it off.
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
