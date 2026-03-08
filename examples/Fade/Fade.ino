/*
  Fade

  Fades the LP-MSPM0C1104 onboard LED using PWM on LED_BUILTIN.
  The LaunchPad LED on PA22 is active-low, so the PWM duty is inverted here.
  This sketch keeps the duty between 1 and 254 to avoid the visible
  mode-switch artifact at the 0%/100% endpoints.
*/

static uint8_t brightnessToDuty(uint8_t brightness)
{
  const uint16_t inverted = 255U - brightness;
  return (uint8_t) (1U + ((inverted * 253U) / 255U));
}

static void writeLedBrightness(uint8_t brightness)
{
  analogWrite(LED_BUILTIN, brightnessToDuty(brightness));
}

void setup() {
  analogWriteResolution(8);
}

void loop() {
  for (int brightness = 0; brightness <= 255; ++brightness) {
    writeLedBrightness((uint8_t) brightness);
    delay(4);
  }

  for (int brightness = 255; brightness >= 0; --brightness) {
    writeLedBrightness((uint8_t) brightness);
    delay(4);
  }
}
