/*
  SSD1306

  Minimal I2C example for a 128x32 SSD1306 OLED using the built-in Wire
  library only. The sketch initializes a common SSD1306 module at address
  0x3C and writes two short text lines.

  Wiring on LP-MSPM0C1104:
  - OLED SDA -> D7 / PA0
  - OLED SCL -> D6 / PA11

  Wiring on LP-MSPM0G3507:
  - OLED SDA -> D10 / PB3
  - OLED SCL -> D9 / PB2
  - OLED VCC -> 3V3
  - OLED GND -> GND

  If your module uses address 0x3D, change kDisplayAddress below.
*/

#include <Wire.h>

namespace {

constexpr uint8_t kDisplayAddress = 0x3C;
constexpr uint8_t kDisplayWidth = 128;
constexpr uint8_t kDisplayPages = 4;
constexpr uint8_t kWirePayloadLimit = 31;

constexpr uint8_t kGlyphSpace[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
constexpr uint8_t kGlyphZero[5]  = {0x3E, 0x45, 0x49, 0x51, 0x3E};
constexpr uint8_t kGlyphE[5]     = {0x7F, 0x49, 0x49, 0x49, 0x41};
constexpr uint8_t kGlyphH[5]     = {0x7F, 0x08, 0x08, 0x08, 0x7F};
constexpr uint8_t kGlyphL[5]     = {0x7F, 0x01, 0x01, 0x01, 0x01};
constexpr uint8_t kGlyphM[5]     = {0x7F, 0x20, 0x10, 0x20, 0x7F};
constexpr uint8_t kGlyphO[5]     = {0x3E, 0x41, 0x41, 0x41, 0x3E};
constexpr uint8_t kGlyphP[5]     = {0x7F, 0x48, 0x48, 0x48, 0x30};
constexpr uint8_t kGlyphS[5]     = {0x31, 0x49, 0x49, 0x49, 0x46};

constexpr uint8_t kInitSequence[] = {
  0xAE,       // Display off
  0xD5, 0x80, // Clock divide
  0xA8, 0x1F, // Multiplex ratio for 32 rows
  0xD3, 0x00, // Display offset
  0x40,       // Start line
  0x8D, 0x14, // Charge pump on
  0x20, 0x02, // Page addressing mode
  0xA1,       // Segment remap
  0xC0,       // COM scan direction
  0xDA, 0x02, // COM pins for 128x32 panels
  0x81, 0x7F, // Contrast
  0xD9, 0xF1, // Pre-charge
  0xDB, 0x40, // VCOM detect
  0xA4,       // Resume RAM display
  0xA6,       // Normal display
  0x2E,       // Deactivate scroll
  0xAF        // Display on
};

const uint8_t *glyphFor(char c)
{
  switch (c) {
    case '0': return kGlyphZero;
    case 'E': return kGlyphE;
    case 'H': return kGlyphH;
    case 'L': return kGlyphL;
    case 'M': return kGlyphM;
    case 'O': return kGlyphO;
    case 'P': return kGlyphP;
    case 'S': return kGlyphS;
    case ' ': return kGlyphSpace;
    default:  return kGlyphSpace;
  }
}

bool writeBlock(uint8_t control, const uint8_t *data, size_t len)
{
  while (len > 0) {
    size_t chunk = len;
    if (chunk > kWirePayloadLimit) {
      chunk = kWirePayloadLimit;
    }

    Wire.beginTransmission(kDisplayAddress);
    Wire.write(control);
    for (size_t i = 0; i < chunk; ++i) {
      Wire.write(data[i]);
    }
    if (Wire.endTransmission() != 0) {
      return false;
    }

    data += chunk;
    len -= chunk;
  }

  return true;
}

bool writeCommandList(const uint8_t *commands, size_t len)
{
  return writeBlock(0x00, commands, len);
}

bool writeData(const uint8_t *data, size_t len)
{
  return writeBlock(0x40, data, len);
}

bool setCursor(uint8_t page, uint8_t column)
{
  const uint8_t commands[3] = {
    static_cast<uint8_t>(0xB0 | (page & 0x07)),
    static_cast<uint8_t>(0x00 | (column & 0x0F)),
    static_cast<uint8_t>(0x10 | ((column >> 4) & 0x0F))
  };
  return writeCommandList(commands, sizeof(commands));
}

bool clearDisplay()
{
  uint8_t zeros[kWirePayloadLimit] = {0};

  for (uint8_t page = 0; page < kDisplayPages; ++page) {
    if (!setCursor(page, 0)) {
      return false;
    }

    uint8_t remaining = kDisplayWidth;
    while (remaining > 0) {
      size_t chunk = remaining;
      if (chunk > sizeof(zeros)) {
        chunk = sizeof(zeros);
      }
      if (!writeData(zeros, chunk)) {
        return false;
      }
      remaining -= static_cast<uint8_t>(chunk);
    }
  }

  return true;
}

bool drawChar(uint8_t page, uint8_t column, char c)
{
  uint8_t bytes[6];
  const uint8_t *glyph = glyphFor(c);

  for (uint8_t i = 0; i < 5; ++i) {
    bytes[i] = glyph[i];
  }
  bytes[5] = 0x00;

  if (!setCursor(page, column)) {
    return false;
  }
  return writeData(bytes, sizeof(bytes));
}

bool drawText(uint8_t page, uint8_t column, const char *text)
{
  while (*text != '\0') {
    if ((column + 6U) > kDisplayWidth) {
      return true;
    }
    if (!drawChar(page, column, *text++)) {
      return false;
    }
    column = static_cast<uint8_t>(column + 6U);
  }

  return true;
}

void signalError()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  while (true) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(150);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(150);
  }
}

} // namespace

void setup()
{
  Wire.begin();
  Wire.setClock(400000UL);
  delay(100);

  if (!writeCommandList(kInitSequence, sizeof(kInitSequence))) {
    signalError();
  }
  if (!clearDisplay()) {
    signalError();
  }
  if (!drawText(0, 49, "HELLO")) {
    signalError();
  }
  if (!drawText(2, 49, "MSPM0")) {
    signalError();
  }
}

void loop()
{
}
