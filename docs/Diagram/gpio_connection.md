# GPIO Connection - ESP32 WROOM 32 DevKit

## 📺 ILI9225 TFT Display (2.2" 176x220)

### Pinout Connection

| ILI9225 Pin | ESP32 GPIO | Mô tả |
|-------------|------------|-------|
| **VCC** | 3.3V | Nguồn điện (3.3V) |
| **GND** | GND | Ground |
| **CS** | GPIO 5 | Chip Select |
| **RST** | GPIO 4 | Reset |
| **RS/DC** | GPIO 2 | Data/Command |
| **SDI/MOSI** | GPIO 23 | SPI Master Out Slave In |
| **CLK/SCK** | GPIO 18 | SPI Clock |
| **LED** | GPIO 15 | Backlight control |

### Sơ đồ kết nối

```
ESP32 WROOM 32 DevKit          ILI9225 Display
┌─────────────────┐            ┌─────────────────┐
│                 │            │                 │
│        3.3V ────┼────────────┼──── VCC         │
│         GND ────┼────────────┼──── GND         │
│      GPIO 5 ────┼────────────┼──── CS          │
│      GPIO 4 ────┼────────────┼──── RST         │
│      GPIO 2 ────┼────────────┼──── RS/DC       │
│     GPIO 23 ────┼────────────┼──── SDI/MOSI    │
│     GPIO 18 ────┼────────────┼──── CLK/SCK     │
│     GPIO 15 ────┼────────────┼──── LED         │
│                 │            │                 │
└─────────────────┘            └─────────────────┘
```

### Ghi chú quan trọng

> ⚠️ **Lưu ý về nguồn điện**:
> - ILI9225 hoạt động ở mức logic 3.3V
> - **KHÔNG** kết nối trực tiếp với 5V

> 📌 **SPI Pins ESP32**:
> - GPIO 23 (VSPI MOSI) - Dùng cho truyền dữ liệu
> - GPIO 18 (VSPI CLK) - Clock signal
> - GPIO 5 (VSPI CS0) - Chip select mặc định của VSPI

### Định nghĩa Pin trong code

```cpp
// ILI9225 Display Pins
#define TFT_RST     4   // Reset
#define TFT_RS      2   // Data/Command (DC)
#define TFT_CS      5   // Chip Select
#define TFT_LED    15   // Backlight

// SPI Pins (Hardware SPI - VSPI)
#define TFT_CLK    18   // SPI Clock
#define TFT_SDI    23   // SPI MOSI
```

---

## 🔌 Tổng hợp GPIO đã sử dụng

| GPIO | Chức năng | Module |
|------|-----------|--------|
| GPIO 2 | RS/DC | ILI9225 Display |
| GPIO 4 | RST | ILI9225 Display |
| GPIO 5 | CS | ILI9225 Display |
| GPIO 15 | LED Backlight | ILI9225 Display |
| GPIO 18 | SPI CLK | ILI9225 Display |
| GPIO 23 | SPI MOSI | ILI9225 Display |

---

## 📋 GPIO còn trống (ESP32 WROOM 32)

### GPIO có thể sử dụng tự do:
- **Input/Output**: GPIO 12, 13, 14, 16, 17, 19, 21, 22, 25, 26, 27, 32, 33
- **Input only**: GPIO 34, 35, 36, 39 (không có pull-up/down nội)

### GPIO nên tránh:
- **GPIO 0**: Boot button (kéo LOW khi flash)
- **GPIO 1**: TX0 (UART)
- **GPIO 3**: RX0 (UART)
- **GPIO 6-11**: Kết nối với flash chip (KHÔNG SỬ DỤNG)