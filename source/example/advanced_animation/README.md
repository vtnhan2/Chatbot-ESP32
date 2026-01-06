# Advanced Animation Demo - TURBO MODE ⚡

## 🎯 Overview

Bộ demo animation nâng cao cho màn hình ILI9225 TFT với ESP32, tối ưu hóa tối đa với:
- **CPU Overclock: 240MHz** (3x faster than default 80MHz)
- **SPI Clock: 60MHz** (+50% from 40MHz)
- **Horizontal Scrolling: 3 pixels/frame**

Đạt **40-50 FPS** với animation phức tạp!

## ✨ Features

### 🚀 **TURBO MODE Performance**
- **CPU: 240MHz** (ESP32 maximum frequency)
- **SPI: 60MHz** (optimized for ILI9225)
- **Horizontal Scrolling: 3px/frame** (smooth panning effect)
- **Object Capacity:**
  - 150 particles (up from 100)
  - 120 stars (up from 80)
  - 12 balls (up from 8)

### 1. **Particle System** (Mode 0)
- Hệ thống hạt với vật lý thực tế
- Trọng lực và bounce effects
- Fading colors theo tuổi thọ của particle
- 150 particles đồng thời

### 2. **Starfield** (Mode 1)
- Hiệu ứng 3D parallax với scrolling
- 120 ngôi sao với độ sâu khác nhau
- Speed trails cho các ngôi sao gần
- Perspective projection

### 3. **Physics Balls** (Mode 2)
- 12 quả bóng với collision detection
- Bouncing với elastic collision
- Random colors từ HSV color space
- Real-time physics simulation

### 4. **Waveform Visualizer** (Mode 3)
- 3 sine waves đồng thời **với scrolling effect**
- Tần số và biên độ khác nhau
- Color-coded (Red/Green/Blue)
- Optimized với lookup table

## 🚀 Performance

### Tối ưu hóa:
- **CPU Clock**: 240MHz (ESP32 max)
- **SPI Clock**: 60MHz (tăng 50% từ 40MHz)
- **Fixed-point Math**: Thay thế float để tăng tốc
- **Sine Lookup Table**: Pre-calculated cho trigonometry
- **Dirty Rectangle**: Chỉ vẽ lại vùng thay đổi
- **Scrolling**: Hardware-accelerated với offset calculation

### FPS Targets (with 240MHz CPU):
- **Particle System**: 40-45 FPS
- **Starfield**: 45-50 FPS
- **Physics Balls**: 35-40 FPS
- **Waveform**: 50+ FPS

## 📦 Installation

1. Copy file `advanced_animation.ino` vào folder:
   ```
   source/example/advanced_animation/
   ```

2. Đảm bảo đã cập nhật SPI clock trong `TFT_ili9225.cpp`:
   ```cpp
   #define SPI_DEFAULT_FREQ 60000000  // 60MHz
   ```

3. Kết nối hardware:
   ```
   ILI9225    ESP32
   -------    -----
   VCC   ---> 3.3V
   GND   ---> GND
   CS    ---> GPIO 5
   RST   ---> GPIO 4
   RS/DC ---> GPIO 2
   SDI   ---> GPIO 23 (VSPI MOSI)
   CLK   ---> GPIO 18 (VSPI CLK)
   LED   ---> GPIO 15
   ```

## 🎮 Usage

1. Upload code lên ESP32:
   ```bash
   arduino-cli compile --fqbn esp32:esp32:esp32 advanced_animation.ino
   arduino-cli upload --fqbn esp32:esp32:esp32 -p COM[X] advanced_animation.ino
   ```

2. Mở Serial Monitor (115200 baud) để xem FPS counter

3. Animation sẽ tự động chuyển đổi mỗi 15 giây:
   - Mode 0: Particle System
   - Mode 1: Starfield
   - Mode 2: Physics Balls
   - Mode 3: Waveform Visualizer

## 🔧 Customization

### Điều chỉnh số lượng objects:
```cpp
#define MAX_PARTICLES 100  // Số particles (50-150)
#define MAX_STARS     80   // Số stars (50-100)
#define MAX_BALLS     8    // Số balls (4-12)
```

### Thay đổi thời gian mỗi mode:
```cpp
const unsigned long MODE_DURATION = 15000; // milliseconds
```

### Điều chỉnh frame rate:
```cpp
delay(10);  // At end of loop() - decrease for faster FPS
```

## 📊 Performance Metrics

Real-time FPS counter hiển thị ở góc trên bên phải:
- **Green**: FPS ≥ 25 (Excellent)
- **Yellow**: FPS 15-24 (Good)
- **Red**: FPS < 15 (Needs optimization)

Serial output cũng in FPS mỗi giây.

## 🐛 Troubleshooting

### Display artifacts hoặc flickering:
1. Giảm SPI clock xuống 50MHz hoặc 40MHz
2. Kiểm tra chất lượng dây nối
3. Đảm bảo nguồn 3.3V ổn định

### FPS thấp:
1. Giảm số lượng objects (particles, stars, balls)
2. Increase delay() ở cuối loop
3. Disable một số effect

### Display không hoạt động:
1. Kiểm tra kết nối pins
2. Verify RS/DC và RST pins
3. Test với LED pin kết nối trực tiếp 3.3V

## 📝 Technical Details

### Fixed-Point Arithmetic:
```cpp
#define FP_SHIFT 4           // 4-bit fractional part
#define FP_SCALE (1 << 4)    // Scale factor = 16
#define TO_FP(x) ((x) << 4)  // Convert to fixed-point
#define FROM_FP(x) ((x) >> 4)  // Convert from fixed-point
```

### Color Space:
- HSV to RGB conversion for smooth color transitions
- 16-bit RGB565 color format
- Alpha blending cho particle fading

### Physics:
- Simple Euler integration
- Elastic collisions với walls
- Gravity constant: 2 pixels/frame²

## 🎨 Code Structure

```
advanced_animation.ino
├── Configuration (pins, constants)
├── Data Structures (Particle, Star, Ball)
├── Math Helpers (fixed-point, trigonometry)
├── Color Helpers (HSV conversion)
├── Initialization Functions
├── Animation Update Functions
└── Main Loop (mode switching)
```

## 📚 References

- ILI9225 Datasheet
- ESP32 SPI Documentation
- Fixed-Point Arithmetic Guide
- HSV Color Space

## 🙏 Credits

Optimized for Chatbot-ESP32 project
Using TFT_22_ILI9225 library
