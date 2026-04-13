# Arduino Core for MSPM0 Series

Arduino core for Texas Instruments MSPM0 series MCUs.

## Supported Hardware

This repository currently supports `LP-MSPM0C1104` and `LP-MSPM0G3507`.
You can also program standalone `MSPM0C1104` or `MSPM0G3507` ICs via SWD using a
CMSIS-DAP or other debug probe.

## Limitations

- `Wire` is controller-only
- `SPI` is controller-only
- SPI chip select is sketch-controlled
- `tone()` is blocking and requires a non-zero duration
- `noTone()` only drives the pin low

## Installation

Install via Arduino IDE Boards Manager:

1. Open `Preferences`.
2. Add this URL to `Additional Boards Manager URLs`:

```text
https://raw.githubusercontent.com/tako0910/arduino-mspm0/master/package_ti_mspm0_index.json
```

3. Open `Tools > Board > Boards Manager`.
4. Search for `Texas Instruments MSPM0 Arduino Core`.
5. Install the package.

## Upload

Upload uses OpenOCD over SWD. The LaunchPad boards have an onboard
XDS110 debug probe that works out of the box:

To use a different SWD probe, select a programmer under
`Tools > Programmer` and upload via `Sketch > Upload Using Programmer`.

Available programmers:

- `TI XDS110` — onboard the LaunchPad
- `CMSIS-DAP` — any CMSIS-DAP / DAPLink compatible probe

## Board Options

The board definition currently provides these menus:

- `Serial Buffer Size`
  - `Small (TX:16 / RX:16)`
  - `Tiny (TX:0 / RX:16)`
  - `Medium (TX:32 / RX:32)`

## Pin Mapping

### LP-MSPM0C1104

| Arduino | MCU pin | Notes |
| --- | --- | --- |
| `D0` | `PA26` | `Serial RX`, `A1`, PWM |
| `D1` | `PA27` | `Serial TX`, `A0`, PWM |
| `D2` | `PA24` | `A3`, PWM |
| `D3` | `PA28` | `A5`, PWM |
| `D4` | `PA6` | `SPI SCK`, PWM |
| `D5` | `PA22` | `LED_BUILTIN`, `A4`, PWM |
| `D6` | `PA11` | `Wire SCL` |
| `D7` | `PA0` | `Wire SDA`, PWM |
| `D8` | `PA23` | GPIO, PWM |
| `D9` | `PA2` | software `SPI SS`, PWM |
| `D10` | `PA4` | `SPI MISO` |
| `D11` | `PA18` | `SPI MOSI`, `A7`, PWM |
| `D12` | `PA17` | `A9`, PWM |
| `D13` | `PA16` | `A8`, PWM |
| `D14` | `PA25` | `A2`, PWM |

Defaults:

- `Serial`
  - RX: `D0` (`PA26`)
  - TX: `D1` (`PA27`)
- `Wire`
  - SDA: `D7` (`PA0`)
  - SCL: `D6` (`PA11`)
- `SPI`
  - MOSI: `D11` (`PA18`)
  - MISO: `D10` (`PA4`)
  - SCK: `D4` (`PA6`)
- `LED_BUILTIN`: `D5` (`PA22`)

### LP-MSPM0G3507

| Arduino | MCU pin | Notes |
| --- | --- | --- |
| `D0` | `PA11` | `Serial RX` |
| `D1` | `PA10` | `Serial TX` |
| `D2` | `PA18` | user button |
| `D3` | `PA0` | `LED_BUILTIN`, PWM |
| `D4` | `PA1` | PWM |
| `D5` | `PB21` | user button |
| `D6` | `PB22` | blue LED, PWM |
| `D7` | `PB26` | red LED, PWM |
| `D8` | `PB27` | green LED |
| `D9` | `PB2` | `Wire SCL` |
| `D10` | `PB3` | `Wire SDA` |
| `D11` | `PB6` | `SPI SS`, PWM |
| `D12` | `PB7` | `SPI MISO`, PWM |
| `D13` | `PB8` | `SPI MOSI`, PWM |
| `D14` | `PB9` | `SPI SCK`, PWM |
| `D15` | `PA25` | `A2`, PWM |
| `D16` | `PB25` | `A4` |
| `D17` | `PA26` | `A1` |
| `D18` | `PA27` | `A0` |
| `D19` | `PA9` | BoosterPack header |
| `D20` | `PB23` | BoosterPack header |
| `D21` | `PA8` | BoosterPack header |
| `D22` | `PB15` | BoosterPack header |
| `D23` | `PB17` | BoosterPack header |
| `D24` | `PB12` | BoosterPack header |
| `D25` | `PB24` | BoosterPack header, `A5` |
| `D26` | `PB0` | BoosterPack header |
| `D27` | `PB1` | BoosterPack header |
| `D28` | `PB4` | BoosterPack header |
| `D29` | `PB16` | BoosterPack header |
| `D30` | `PB19` | BoosterPack header |
| `D31` | `PA28` | BoosterPack header |
| `D32` | `PA22` | BoosterPack header, `A7` |
| `D33` | `PA31` | BoosterPack header |
| `D34` | `PB18` | BoosterPack header |
| `D35` | `PB20` | BoosterPack header, `A6` |
| `D36` | `PB13` | BoosterPack header |
| `D37` | `PA24` | BoosterPack header, `A3` |
| `D38` | `PA17` | BoosterPack header |
| `D39` | `PA16` | BoosterPack header |
| `D40` | `PA12` | BoosterPack header |
| `D41` | `PA13` | BoosterPack header |
| `D42` | `PA15` | BoosterPack header |

Defaults:

- `Serial`
  - RX: `D0` (`PA11`)
  - TX: `D1` (`PA10`)
- `Wire`
  - SDA: `D10` (`PB3`)
  - SCL: `D9` (`PB2`)
- `SPI`
  - MOSI: `D13` (`PB8`)
  - MISO: `D12` (`PB7`)
  - SCK: `D14` (`PB9`)
  - SS: `D11` (`PB6`)
- `LED_BUILTIN`: `D3` (`PA0`)

## AI Usage

This Arduino port was primarily developed by Codex (GPT-5.4), with
Claude Code (Opus 4.6) as secondary contributor. Code review was
performed by both models along with Gemini 3 Flash.
