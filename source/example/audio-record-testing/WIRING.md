# ESP32 Audio System Wiring Guide

Hướng dẫn kết nối phần cứng cho hệ thống recording và playback audio.

## 📋 Components Required

- **ESP32 WROOM 32 DevKit** × 1
- **INMP441 I2S MEMS Microphone** × 1
- **MAX98357 I2S Class D Amplifier** × 1
- **SD Card Module** × 1
- **Micro SD Card** (formatted FAT32, 1GB - 32GB)
- **Speaker** 3-8Ω, 3W (for MAX98357)
- **Breadboard and jumper wires**
- **USB Cable** for power and programming

---

## 🔌 Pin Connections

### INMP441 Microphone → ESP32

| INMP441 Pin | ESP32 GPIO | Description |
|-------------|------------|-------------|
| VDD | 3.3V | Power supply |
| GND | GND | Ground |
| SCK | **GPIO 32** | Serial Clock (I2S_SCK) |
| WS | **GPIO 25** | Word Select / LRCLK |
| SD | **GPIO 33** | Serial Data (microphone output) |
| L/R | GND | Left channel (GND) or Right (3.3V) |

> [!NOTE]
> Connect L/R pin to **GND** for left channel. If you want right channel, connect to 3.3V.

---

### MAX98357 Amplifier → ESP32

| MAX98357 Pin | ESP32 Pin | Description |
|--------------|-----------|-------------|
| VIN | **5V** (hoặc 3.3V) | Power supply (5V cho volume tốt hơn) |
| GND | **GND** | Ground |
| BCLK | **GPIO 26** | Bit Clock |
| LRC | **GPIO 27** | Left/Right Clock (Word Select) |
| DIN | **GPIO 14** | Data Input |
| GAIN | Float/GND/5V | Volume control (xem bảng bên dưới) |
| SD | **5V** (hoặc 3.3V) | Shutdown control (HIGH = ON) |

#### MAX98357 Gain Settings:
| GAIN Pin | Gain Level |
|----------|------------|
| Float (not connected) | 9dB (default) |
| GND | 6dB |
| **5V** (hoặc 3.3V) | 12dB |
| 100kΩ to GND | 15dB |

> [!TIP]
> Start with **GAIN floating** (9dB). If too quiet, connect to 5V for 12dB. If too loud, connect to GND for 6dB.

---

### SD Card Module → ESP32

| SD Module Pin | ESP32 Pin | Description |
|---------------|-----------|-------------|
| VCC | **5V** (hoặc 3.3V) | Power supply |
| GND | **GND** | Ground |
| MISO | **GPIO 12** | Master In Slave Out |
| MOSI | **GPIO 13** | Master Out Slave In |
| SCK | **GPIO 21** | SPI Clock |
| CS | **GPIO 22** | Chip Select |

> [!WARNING]
> Some SD modules require **5V** power. If using 3.3V module, connect VCC to 3.3V instead.

---

## 📊 GPIO Summary

### Used GPIO Pins:

| GPIO | Device | Function |
|------|--------|----------|
| 12 | SD Card | MISO |
| 13 | SD Card | MOSI |
| 14 | Speaker | DIN |
| 21 | SD Card | SCK |
| 22 | SD Card | CS |
| 25 | Microphone | WS |
| 26 | Speaker | BCLK |
| 27 | Speaker | LRC |
| 32 | Microphone | SCK |
| 33 | Microphone | SD |
| 2 | LED | Status indicator (built-in) |

### Display Module Pins (No Conflict):

Display uses: GPIO 2, 4, 15 — **No overlap with audio system!** ✅

---

## 🔧 Wiring Diagram (Text-Based)

```
ESP32                    INMP441 Microphone
┌─────────────┐         ┌──────────────┐
│             │         │              │
│    3.3V ────┼─────────┤ VDD          │
│     GND ────┼─────────┤ GND          │
│  GPIO32 ────┼─────────┤ SCK          │
│  GPIO25 ────┼─────────┤ WS           │
│  GPIO33 ────┼─────────┤ SD           │
│             │    ┌────┤ L/R          │
│     GND ────┼────┘    │              │
└─────────────┘         └──────────────┘


ESP32                    MAX98357 Amplifier        Speaker
┌─────────────┐         ┌──────────────┐         ┌────────┐
│             │         │              │         │        │
│      5V ────┼─────────┤ VIN          │         │        │
│     GND ────┼─────────┤ GND          │         │        │
│  GPIO26 ────┼─────────┤ BCLK         │         │        │
│  GPIO27 ────┼─────────┤ LRC          │    ┌────┤ +      │
│  GPIO14 ────┼─────────┤ DIN          │    │┌───┤ -      │
│             │    ┌────┤ SD           │    ││   └────────┘
│      5V ────┼────┘    │              │    ││
│             │   Float─┤ GAIN   SPK+ ─┼────┘│
│             │         │        SPK- ─┼─────┘
└─────────────┘         └──────────────┘


ESP32                    SD Card Module
┌─────────────┐         ┌──────────────┐
│             │         │              │
│      5V ────┼─────────┤ VCC          │
│     GND ────┼─────────┤ GND          │
│  GPIO12 ────┼─────────┤ MISO         │
│  GPIO13 ────┼─────────┤ MOSI         │
│  GPIO21 ────┼─────────┤ SCK          │
│  GPIO22 ────┼─────────┤ CS           │
└─────────────┘         └──────────────┘
```

---

## ⚠️ Important Notes

### Power Supply

1. **ESP32**: Can be powered via USB (5V) or external 5V/3.3V
2. **INMP441**: Always use **3.3V** (NOT 5V!)
3. **MAX98357**: Can use **3.3V - 5.5V** (5V recommended for better audio quality)
4. **SD Module**: Check your module - some need 5V, some need 3.3V

> [!CAUTION]
> **Do NOT connect 5V to INMP441!** It will damage the microphone. Only use 3.3V.

### Ground Connections

All devices must share a **common ground** with ESP32. Make sure all GND pins are connected together.

### SD Card Format

- Use **FAT32** file system
- Card size: **1GB to 32GB** (cards > 32GB may use exFAT which is not supported)
- Format the card before first use

### Speaker Selection

MAX98357 works with:
- **3Ω to 8Ω** speakers
- **3W to 5W** power rating recommended
- Can also connect to **headphones** (use lower GAIN setting)

---

## 🧪 Testing Checklist

Before uploading the sketch:

- [ ] All power connections checked (3.3V for mic, 5V for amp)
- [ ] All ground pins connected together
- [ ] SD card inserted and formatted (FAT32)
- [ ] Speaker connected to MAX98357 SPK+ and SPK-
- [ ] No loose wires or short circuits
- [ ] USB cable connected for power and Serial Monitor

---

## 🔍 Troubleshooting

### No audio recorded / SD card error

1. ✓ Check SD card is formatted FAT32
2. ✓ Verify SD module wiring (especially CS pin)
3. ✓ Try different SD card
4. ✓ Check Serial Monitor for error messages

### Audio playback is very quiet

1. ✓ Increase MAX98357 GAIN (connect to **5V** for 12dB)
2. ✓ Use **5V** power instead of 3.3V for MAX98357
3. ✓ Check speaker is connected correctly
4. ✓ Verify speaker impedance (3-8Ω works best)

### Audio playback is distorted

1. ✓ Decrease MAX98357 GAIN (connect to GND for 6dB)
2. ✓ Check power supply is stable (don't power ESP32 + amplifier from weak USB port)
3. ✓ Add decoupling capacitor (100µF) across **VIN** (MAX98357) and GND
4. ✓ Reduce recording volume (speak further from mic)

### Microphone not recording

1. ✓ Verify INMP441 is using **3.3V** not 5V
2. ✓ Check SCK, WS, SD pin connections
3. ✓ Ensure L/R pin is connected to GND
4. ✓ Try clapping near microphone - check Serial Monitor for activity

### GPIO conflicts with display

The pin assignment was specifically chosen to avoid conflicts:
- **Display uses**: GPIO 2, 4, 5, 15, 18, 23
- **Audio uses**: GPIO 12, 13, 14, 21, 22, 25, 26, 27, 32, 33

No overlap! Both systems can run simultaneously. ✅

---

## 📚 Additional Resources

- [INMP441 Datasheet](https://www.invensense.com/products/digital/inmp441/)
- [MAX98357 Datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/MAX98357A-MAX98357B.pdf)
- [ESP32 I2S Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/i2s.html)
- [Arduino SD Library](https://www.arduino.cc/reference/en/libraries/sd/)

---

## ✅ Ready to Test!

Once wiring is complete:
1. Open `audio.ino` in Arduino IDE
2. Select **Board**: ESP32 Dev Module
3. Select correct **COM Port**
4. Upload sketch
5. Open **Serial Monitor** (115200 baud)
6. Watch for "Recording..." message
7. Speak into microphone for 10 seconds
8. Wait 5 seconds
9. Listen to playback through speaker!

Repeat cycle automatically. Enjoy! 🎉
