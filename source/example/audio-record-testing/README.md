# ESP32 Audio Recording & Playback Demo

Demo cho hệ thống recording và playback audio với ESP32.

## 🎯 Tính Năng

- ✅ Record audio từ **INMP441** I2S microphone
- ✅ Lưu file **WAV** vào SD card
- ✅ Phát lại qua **MAX98357** I2S amplifier
- ✅ Auto test loop: Record 10s → Wait 5s → Playback 10s

## 📁 Files

- `audio.ino` - Main sketch
- `WIRING.md` - Hướng dẫn nối dây chi tiết

## 🚀 Quick Start

1. **Nối dây theo WIRING.md**
2. **Cắm SD card** (FAT32 format)
3. **Upload sketch**
4. **Mở Serial Monitor** (115200 baud)
5. **Nói vào mic** khi thấy "Recording..."
6. **Nghe playback** qua speaker!

## 🔧 GPIO Pins

| Device | Pins Used |
|--------|-----------|
| INMP441 | 32, 25, 33 |
| MAX98357 | 26, 27, 14 |
| SD Card | 12, 13, 21, 22 |

**Không conflict** với display module! ✅

## 📚 Documentation

Chi tiết xem [WIRING.md](WIRING.md)
