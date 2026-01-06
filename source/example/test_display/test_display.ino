/**
 * @file test_display.ino
 * @brief Example code để test màn hình ILI9225 với ESP32 WROOM 32 DevKit
 * 
 * Kết nối:
 *   ILI9225    ESP32
 *   -------    -----
 *   VCC   -->  3.3V
 *   GND   -->  GND
 *   CS    -->  GPIO 5
 *   RST   -->  GPIO 4
 *   RS/DC -->  GPIO 2
 *   SDI   -->  GPIO 23 (VSPI MOSI)
 *   CLK   -->  GPIO 18 (VSPI CLK)
 *   LED   -->  GPIO 15
 */

#include <SPI.h>
#include "TFT_22_ILI9225.h"

// ============================================
// PIN DEFINITIONS
// ============================================
#define TFT_RST     4   // Reset
#define TFT_RS      2   // Data/Command (DC)
#define TFT_CS      5   // Chip Select
#define TFT_LED    15   // Backlight

// Hardware SPI pins (ESP32 VSPI)
#define TFT_CLK    18   // SPI Clock
#define TFT_SDI    23   // SPI MOSI

// ============================================
// TFT OBJECT
// ============================================
// Sử dụng Hardware SPI (nhanh hơn)
TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED);

// Nếu muốn dùng Software SPI (chậm hơn nhưng linh hoạt hơn):
// TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_SDI, TFT_CLK, TFT_LED);

// ============================================
// SETUP
// ============================================
void setup() {
    Serial.begin(115200);
    delay(1000);  // Chờ Serial ổn định
    
    Serial.println("\n\n=============================");
    Serial.println("ILI9225 Display Test");
    Serial.println("=============================");
    
    Serial.println("Pin Configuration:");
    Serial.printf("  RST:  GPIO %d\n", TFT_RST);
    Serial.printf("  RS:   GPIO %d\n", TFT_RS);
    Serial.printf("  CS:   GPIO %d\n", TFT_CS);
    Serial.printf("  LED:  GPIO %d\n", TFT_LED);
    Serial.printf("  CLK:  GPIO 18 (VSPI)\n");
    Serial.printf("  MOSI: GPIO 23 (VSPI)\n");
    
    Serial.println("\nInitializing display...");
    
    // Khởi tạo display
    tft.begin();
    tft.setOrientation(0);  // 0, 1, 2, 3 (xoay màn hình)
    tft.setBacklight(true);
    
    Serial.println("Display initialized!");
    Serial.printf("Screen size: %d x %d\n", tft.maxX(), tft.maxY());
    
    // Test ngay lập tức với màu đơn sắc
    Serial.println("\n*** QUICK TEST: Filling screen RED ***");
    tft.fillRectangle(0, 0, 175, 219, COLOR_RED);
    delay(2000);
    
    Serial.println("*** QUICK TEST: Filling screen GREEN ***");
    tft.fillRectangle(0, 0, 175, 219, COLOR_GREEN);
    delay(2000);
    
    Serial.println("*** QUICK TEST: Filling screen BLUE ***");
    tft.fillRectangle(0, 0, 175, 219, COLOR_BLUE);
    delay(2000);
    
    Serial.println("\nIf screen is still BLANK, check:");
    Serial.println("  1. RS/DC pin connection (try swap with RST)");
    Serial.println("  2. LED pin -> try connect directly to 3.3V");
    Serial.println("  3. Make sure using 3.3V not 5V");
    Serial.println("=============================\n");
}

// ============================================
// MAIN LOOP
// ============================================
void loop() {
    // Test 1: Clear screen với các màu khác nhau
    testClearScreen();
    
    // Test 2: Vẽ các hình cơ bản
    testDrawShapes();
    
    // Test 3: Hiển thị text
    testDrawText();
    
    // Test 4: Gradient và màu sắc
    testColorGradient();
    
    // Test 5: Animation đơn giản
    testAnimation();
    
    Serial.println("All tests completed! Restarting...\n");
    delay(2000);
}

// ============================================
// TEST FUNCTIONS
// ============================================

/**
 * Test 1: Clear màn hình với các màu khác nhau
 */
void testClearScreen() {
    Serial.println("Test 1: Clear Screen Colors");
    
    uint16_t colors[] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_YELLOW, COLOR_CYAN, COLOR_MAGENTA};
    const char* colorNames[] = {"RED", "GREEN", "BLUE", "YELLOW", "CYAN", "MAGENTA"};
    
    for (int i = 0; i < 6; i++) {
        tft.clear();
        tft.fillRectangle(0, 0, tft.maxX(), tft.maxY(), colors[i]);
        Serial.printf("  Color: %s\n", colorNames[i]);
        delay(500);
    }
    
    tft.clear();
    delay(300);
}

/**
 * Test 2: Vẽ các hình cơ bản
 */
void testDrawShapes() {
    Serial.println("Test 2: Draw Shapes");
    tft.clear();
    
    // Vẽ hình chữ nhật
    Serial.println("  Drawing rectangles...");
    tft.drawRectangle(10, 10, 80, 60, COLOR_RED);
    tft.fillRectangle(90, 10, 160, 60, COLOR_GREEN);
    delay(500);
    
    // Vẽ hình tròn
    Serial.println("  Drawing circles...");
    tft.drawCircle(45, 120, 30, COLOR_BLUE);
    tft.fillCircle(125, 120, 30, COLOR_YELLOW);
    delay(500);
    
    // Vẽ đường thẳng
    Serial.println("  Drawing lines...");
    for (int i = 0; i < 176; i += 10) {
        tft.drawLine(0, 180, i, 220, COLOR_CYAN);
    }
    delay(500);
    
    // Vẽ tam giác
    Serial.println("  Drawing triangle...");
    tft.drawTriangle(88, 160, 60, 200, 116, 200, COLOR_MAGENTA);
    
    delay(2000);
}

/**
 * Test 3: Hiển thị text
 */
void testDrawText() {
    Serial.println("Test 3: Draw Text");
    tft.clear();
    
    // Đặt font mặc định
    tft.setFont(Terminal6x8);
    
    // Hiển thị tiêu đề
    tft.drawText(20, 10, "ILI9225 TEST", COLOR_WHITE);
    tft.drawText(25, 25, "ESP32 WROOM", COLOR_CYAN);
    
    // Vẽ đường kẻ phân cách
    tft.drawLine(0, 40, 176, 40, COLOR_GRAY);
    
    // Hiển thị thông tin
    tft.drawText(5, 50, "Resolution:", COLOR_YELLOW);
    tft.drawText(75, 50, "176x220", COLOR_WHITE);
    
    tft.drawText(5, 65, "Color:", COLOR_YELLOW);
    tft.drawText(75, 65, "16-bit RGB", COLOR_WHITE);
    
    tft.drawText(5, 80, "Interface:", COLOR_YELLOW);
    tft.drawText(75, 80, "SPI", COLOR_WHITE);
    
    // Vẽ bảng màu
    tft.drawText(5, 100, "Color Palette:", COLOR_GREEN);
    
    uint16_t palette[] = {COLOR_RED, COLOR_ORANGE, COLOR_YELLOW, COLOR_GREEN, 
                          COLOR_CYAN, COLOR_BLUE, COLOR_VIOLET, COLOR_WHITE};
    int x = 5;
    for (int i = 0; i < 8; i++) {
        tft.fillRectangle(x, 115, x + 18, 135, palette[i]);
        x += 21;
    }
    
    // Footer
    tft.drawLine(0, 200, 176, 200, COLOR_GRAY);
    tft.drawText(30, 207, "Chatbot ESP32", COLOR_ORANGE);
    
    delay(3000);
}

/**
 * Test 4: Gradient và màu sắc
 */
void testColorGradient() {
    Serial.println("Test 4: Color Gradient");
    tft.clear();
    
    tft.setFont(Terminal6x8);
    tft.drawText(30, 5, "COLOR GRADIENT", COLOR_WHITE);
    
    // Red gradient
    for (int i = 0; i < 176; i++) {
        uint16_t color = tft.setColor((i * 255 / 176), 0, 0);
        tft.drawLine(i, 25, i, 65, color);
    }
    tft.drawText(5, 70, "Red", COLOR_RED);
    
    // Green gradient
    for (int i = 0; i < 176; i++) {
        uint16_t color = tft.setColor(0, (i * 255 / 176), 0);
        tft.drawLine(i, 85, i, 125, color);
    }
    tft.drawText(5, 130, "Green", COLOR_GREEN);
    
    // Blue gradient
    for (int i = 0; i < 176; i++) {
        uint16_t color = tft.setColor(0, 0, (i * 255 / 176));
        tft.drawLine(i, 145, i, 185, color);
    }
    tft.drawText(5, 190, "Blue", COLOR_BLUE);
    
    delay(3000);
}

/**
 * Test 5: Animation đơn giản
 */
void testAnimation() {
    Serial.println("Test 5: Simple Animation");
    tft.clear();
    
    tft.setFont(Terminal6x8);
    tft.drawText(40, 5, "ANIMATION", COLOR_WHITE);
    
    // Bouncing ball animation
    int x = 88, y = 110;
    int dx = 3, dy = 2;
    int radius = 10;
    
    for (int frame = 0; frame < 100; frame++) {
        // Xóa vị trí cũ
        tft.fillCircle(x, y, radius + 1, COLOR_BLACK);
        
        // Cập nhật vị trí
        x += dx;
        y += dy;
        
        // Kiểm tra va chạm với biên
        if (x <= radius + 5 || x >= 170 - radius) dx = -dx;
        if (y <= radius + 20 || y >= 200 - radius) dy = -dy;
        
        // Vẽ bóng mới
        tft.fillCircle(x, y, radius, COLOR_ORANGE);
        tft.drawCircle(x, y, radius, COLOR_YELLOW);
        
        // Vẽ khung
        tft.drawRectangle(5, 20, 170, 200, COLOR_GRAY);
        
        delay(30);
    }
    
    tft.drawText(45, 210, "COMPLETED!", COLOR_GREEN);
    delay(1500);
}
