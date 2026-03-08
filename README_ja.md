# MSPM0 シリーズ向け Arduino Core

Texas Instruments MSPM0 シリーズ MCU 向けの Arduino core です。

## 対応ハードウェア

現在、このリポジトリが対応しているのは `LP-MSPM0C1104` のみです。
CMSIS-DAP 等の SWD プローブを使用すれば、単体の `MSPM0C1104` IC への
書き込みも可能です。

## 制約事項

- `Wire` は controller mode のみ対応です
- `SPI` は controller mode のみ対応です
- SPI の chip select はスケッチ側で制御してください
- `tone()` はブロッキング動作で、`duration` に 0 は指定できません
- `noTone()` は対象ピンを LOW に戻すのみです

## インストール

Arduino IDE の Boards Manager から導入できます。

1. `設定` を開きます。
2. `追加のボードマネージャの URL` に次を追加します。

```text
https://raw.githubusercontent.com/tako0910/arduino-mspm0/master/package_ti_mspm0_index.json
```

3. `ツール > ボード > ボードマネージャ` を開きます。
4. `Texas Instruments MSPM0 Arduino Core` を検索します。
5. パッケージをインストールします。

## 書き込み

書き込みは OpenOCD を使用し、SWD 経由で行います。LP-MSPM0C1104 LaunchPad
にはオンボードの XDS110 デバッグプローブが搭載されており、そのまま使用
できます。

別の SWD プローブを使用する場合は `ツール > 書き込み装置` でプログラマを
選択し、`スケッチ > 書き込み装置を使って書き込み` から書き込んでください。

利用可能なプログラマ:

- `TI XDS110` — LaunchPad に搭載
- `CMSIS-DAP` — CMSIS-DAP / DAPLink 互換の汎用プローブ

## ボードオプション

現在の board definition には次のメニューがあります。

- `Serial Buffer Size`
  - `Small (TX:16 / RX:16)`
  - `Tiny (TX:0 / RX:16)`
  - `Medium (TX:32 / RX:32)`

## ピンマッピング

| Arduino | MCU pin | 用途 |
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

デフォルト割り当て:

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

## AI の利用

この Arduino 移植は主に Codex (GPT-5.4) が開発し、Claude Code
(Opus 4.6) が副次的な貢献を行いました。コードレビューは両モデルに加え
Gemini 3 Flash が担当しました。
