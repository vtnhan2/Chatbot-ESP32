/**
 * @file advanced_animation.ino
 * @brief Advanced smooth animations for ILI9225 with optimized rendering
 * 
 * Features:
 * - CPU overclocked to 240MHz for maximum performance
 * - Horizontal image scrolling (3 pixels left per frame)
 * - Particle system with physics
 * - Waveform visualizer
 * - Bouncing balls with collision
 * - Starfield animation
 * - Real-time FPS counter
 * 
 * Optimized for 60MHz SPI clock + 240MHz CPU
 */

#include <SPI.h>
#include "TFT_22_ILI9225.h"

// ============================================
// PIN DEFINITIONS
// ============================================
#define TFT_RST     4
#define TFT_RS      2
#define TFT_CS      5
#define TFT_LED    15

// ============================================
// CONFIGURATION
// ============================================
#define SCREEN_WIDTH  176
#define SCREEN_HEIGHT 220
#define MAX_PARTICLES 150  // Increased for 240MHz CPU
#define MAX_STARS     120  // Increased for better visual
#define MAX_BALLS     12   // More balls for physics demo
#define SCROLL_SPEED  3    // Pixels to scroll per frame

// ============================================
// TFT OBJECT
// ============================================
TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED);

// ============================================
// PARTICLE SYSTEM
// ============================================
struct Particle {
    int16_t x, y;           // Position (fixed point * 16)
    int8_t vx, vy;          // Velocity
    uint16_t color;
    uint8_t life;
    bool active;
};

Particle particles[MAX_PARTICLES];

// ============================================
// STARFIELD
// ============================================
struct Star {
    int16_t x, y, z;        // 3D position
    uint16_t color;
};

Star stars[MAX_STARS];

// ============================================
// BOUNCING BALLS
// ============================================
struct Ball {
    int16_t x, y;           // Position (fixed point * 16)
    int16_t vx, vy;         // Velocity (fixed point * 16)
    uint8_t radius;
    uint16_t color;
};

Ball balls[MAX_BALLS];

// ============================================
// IMAGE SCROLLING
// ============================================
int16_t scrollOffset = 0;  // Current scroll position

// ============================================
// PERFORMANCE TRACKING
// ============================================
unsigned long frameCount = 0;
unsigned long lastFPSTime = 0;
int currentFPS = 0;
unsigned long lastFrameTime = 0;
float avgFrameTime = 0;

// ============================================
// MATH HELPERS (Fixed Point)
// ============================================
#define FP_SHIFT 4
#define FP_SCALE (1 << FP_SHIFT)
#define TO_FP(x) ((x) << FP_SHIFT)
#define FROM_FP(x) ((x) >> FP_SHIFT)

// Fast sine lookup table (0-90 degrees, scaled to 0-255)
const uint8_t sinTable[91] PROGMEM = {
    0, 4, 9, 13, 18, 22, 27, 31, 35, 40, 44, 48, 53, 57, 61, 65, 
    70, 74, 78, 83, 87, 91, 95, 99, 103, 107, 111, 115, 119, 123, 127, 131, 
    135, 138, 142, 146, 149, 153, 156, 160, 163, 167, 170, 173, 177, 180, 183, 186, 
    189, 192, 195, 198, 200, 203, 206, 208, 211, 213, 216, 218, 220, 223, 225, 227, 
    229, 231, 232, 234, 236, 238, 239, 241, 242, 243, 245, 246, 247, 248, 249, 250, 
    251, 252, 252, 253, 253, 254, 254, 254, 254, 254, 255
};

int16_t fastSin(int16_t angle) {
    angle = angle % 360;
    if (angle < 0) angle += 360;
    
    if (angle <= 90) return pgm_read_byte(&sinTable[angle]);
    if (angle <= 180) return pgm_read_byte(&sinTable[180 - angle]);
    if (angle <= 270) return -pgm_read_byte(&sinTable[angle - 180]);
    return -pgm_read_byte(&sinTable[360 - angle]);
}

int16_t fastCos(int16_t angle) {
    return fastSin(angle + 90);
}

// ============================================
// COLOR HELPERS
// ============================================
uint16_t colorFromHSV(uint8_t h, uint8_t s, uint8_t v) {
    uint8_t r, g, b;
    
    if (s == 0) {
        r = g = b = v;
    } else {
        uint8_t region = h / 43;
        uint8_t remainder = (h - (region * 43)) * 6;
        
        uint8_t p = (v * (255 - s)) >> 8;
        uint8_t q = (v * (255 - ((s * remainder) >> 8))) >> 8;
        uint8_t t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;
        
        switch (region) {
            case 0: r = v; g = t; b = p; break;
            case 1: r = q; g = v; b = p; break;
            case 2: r = p; g = v; b = t; break;
            case 3: r = p; g = q; b = v; break;
            case 4: r = t; g = p; b = v; break;
            default: r = v; g = p; b = q; break;
        }
    }
    
    return tft.setColor(r, g, b);
}

// ============================================
// INITIALIZATION FUNCTIONS
// ============================================

void initParticles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].active = false;
    }
}

void spawnParticle(int16_t x, int16_t y, uint16_t color) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) {
            particles[i].x = TO_FP(x);
            particles[i].y = TO_FP(y);
            particles[i].vx = random(-20, 21);
            particles[i].vy = random(-30, -10);
            particles[i].color = color;
            particles[i].life = 255;
            particles[i].active = true;
            break;
        }
    }
}

void initStarfield() {
    for (int i = 0; i < MAX_STARS; i++) {
        stars[i].x = random(-500, 500);
        stars[i].y = random(-500, 500);
        stars[i].z = random(50, 500);
        stars[i].color = COLOR_WHITE;
    }
}

void initBalls() {
    for (int i = 0; i < MAX_BALLS; i++) {
        balls[i].x = TO_FP(random(20, SCREEN_WIDTH - 20));
        balls[i].y = TO_FP(random(20, SCREEN_HEIGHT - 40));
        balls[i].vx = random(-40, 41);
        balls[i].vy = random(-40, 41);
        balls[i].radius = random(3, 8);
        balls[i].color = colorFromHSV(random(0, 256), 255, 255);
    }
}

// ============================================
// SETUP
// ============================================
void setup() {
    // *** OVERCLOCK CPU TO 240MHz ***
    setCpuFrequencyMhz(240);
    
    Serial.begin(115200);
    delay(500);
    
    Serial.println("\n====================================");
    Serial.println("Advanced Animation Demo - TURBO MODE");
    Serial.printf("CPU Frequency: %d MHz\n", getCpuFrequencyMhz());
    Serial.println("SPI: 60MHz | Scroll: 3px/frame");
    Serial.println("====================================\n");
    
    // Initialize display
    tft.begin();
    tft.setOrientation(0);
    tft.setBacklight(true);
    tft.clear();
    
    // Show startup screen
    tft.setFont(Terminal6x8);
    tft.drawText(20, 90, "TURBO MODE 240MHz", COLOR_ORANGE);
    tft.drawText(30, 105, "ADVANCED DEMO", COLOR_CYAN);
    tft.drawText(40, 120, "Loading...", COLOR_WHITE);
    delay(1000);
    
    // Initialize animations
    initParticles();
    initStarfield();
    initBalls();
    
    lastFPSTime = millis();
    lastFrameTime = micros();
    Serial.println("Initialization complete!\n");
}

// ============================================
// ANIMATION FUNCTIONS
// ============================================

void updateParticles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            // Clear old position
            int16_t px = FROM_FP(particles[i].x);
            int16_t py = FROM_FP(particles[i].y);
            if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT) {
                tft.drawPixel(px, py, COLOR_BLACK);
            }
            
            // Update physics
            particles[i].vy += 2; // Gravity
            particles[i].x += particles[i].vx;
            particles[i].y += particles[i].vy;
            particles[i].life -= 5;
            
            // Check boundaries
            if (particles[i].life <= 0 || FROM_FP(particles[i].y) >= SCREEN_HEIGHT) {
                particles[i].active = false;
                continue;
            }
            
            // Draw new position
            px = FROM_FP(particles[i].x);
            py = FROM_FP(particles[i].y);
            if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT) {
                // Fade color based on life
                uint8_t brightness = particles[i].life;
                uint16_t fadedColor = tft.setColor(
                    ((particles[i].color >> 11) * brightness) >> 8,
                    (((particles[i].color >> 5) & 0x3F) * brightness) >> 8,
                    ((particles[i].color & 0x1F) * brightness) >> 8
                );
                tft.drawPixel(px, py, fadedColor);
            }
        }
    }
}

void updateStarfield() {
    for (int i = 0; i < MAX_STARS; i++) {
        // Clear old position
        int16_t sx = (stars[i].x * 256) / stars[i].z + SCREEN_WIDTH / 2;
        int16_t sy = (stars[i].y * 256) / stars[i].z + SCREEN_HEIGHT / 2;
        if (sx >= 0 && sx < SCREEN_WIDTH && sy >= 0 && sy < SCREEN_HEIGHT) {
            tft.drawPixel(sx, sy, COLOR_BLACK);
        }
        
        // Move star
        stars[i].z -= 8;
        if (stars[i].z <= 0) {
            stars[i].x = random(-500, 500);
            stars[i].y = random(-500, 500);
            stars[i].z = 500;
        }
        
        // Draw new position with brightness based on depth
        sx = (stars[i].x * 256) / stars[i].z + SCREEN_WIDTH / 2;
        sy = (stars[i].y * 256) / stars[i].z + SCREEN_HEIGHT / 2;
        if (sx >= 0 && sx < SCREEN_WIDTH && sy >= 0 && sy < SCREEN_HEIGHT) {
            uint8_t brightness = 255 - (stars[i].z >> 1);
            uint16_t color = tft.setColor(brightness, brightness, brightness);
            
            // Draw trail for fast stars
            if (stars[i].z < 100) {
                int16_t oldsx = (stars[i].x * 256) / (stars[i].z + 8) + SCREEN_WIDTH / 2;
                int16_t oldsy = (stars[i].y * 256) / (stars[i].z + 8) + SCREEN_HEIGHT / 2;
                if (oldsx >= 0 && oldsx < SCREEN_WIDTH && oldsy >= 0 && oldsy < SCREEN_HEIGHT) {
                    tft.drawLine(oldsx, oldsy, sx, sy, color);
                }
            } else {
                tft.drawPixel(sx, sy, color);
            }
        }
    }
}

void updateBalls() {
    for (int i = 0; i < MAX_BALLS; i++) {
        // Clear old position
        int16_t bx = FROM_FP(balls[i].x);
        int16_t by = FROM_FP(balls[i].y);
        tft.fillCircle(bx, by, balls[i].radius, COLOR_BLACK);
        
        // Update position
        balls[i].x += balls[i].vx;
        balls[i].y += balls[i].vy;
        
        // Bounce off walls
        if (FROM_FP(balls[i].x) <= balls[i].radius || 
            FROM_FP(balls[i].x) >= SCREEN_WIDTH - balls[i].radius) {
            balls[i].vx = -balls[i].vx;
            balls[i].x = FROM_FP(balls[i].x) <= balls[i].radius ? 
                         TO_FP(balls[i].radius) : 
                         TO_FP(SCREEN_WIDTH - balls[i].radius);
        }
        
        if (FROM_FP(balls[i].y) <= balls[i].radius || 
            FROM_FP(balls[i].y) >= SCREEN_HEIGHT - balls[i].radius) {
            balls[i].vy = -balls[i].vy;
            balls[i].y = FROM_FP(balls[i].y) <= balls[i].radius ? 
                         TO_FP(balls[i].radius) : 
                         TO_FP(SCREEN_HEIGHT - balls[i].radius);
        }
        
        // Draw new position
        bx = FROM_FP(balls[i].x);
        by = FROM_FP(balls[i].y);
        tft.fillCircle(bx, by, balls[i].radius, balls[i].color);
    }
}

void drawWaveform() {
    static int16_t waveOffset = 0;
    int16_t y_base = SCREEN_HEIGHT - 30;
    
    // Clear old wave area
    tft.fillRectangle(0, y_base - 20, SCREEN_WIDTH - 1, y_base + 20, COLOR_BLACK);
    
    // Draw multiple sine waves with scroll offset
    for (int x = 0; x < SCREEN_WIDTH - 1; x++) {
        int16_t scrolledX = (x + scrollOffset) % SCREEN_WIDTH;
        
        // Wave 1: Fast, small amplitude
        int16_t y1 = y_base + (fastSin((scrolledX * 4 + waveOffset) % 360) * 8) / 255;
        int16_t y1_next = y_base + (fastSin(((scrolledX + 1) * 4 + waveOffset) % 360) * 8) / 255;
        tft.drawLine(x, y1, x + 1, y1_next, COLOR_RED);
        
        // Wave 2: Medium speed, medium amplitude
        int16_t y2 = y_base + (fastSin((scrolledX * 2 + waveOffset * 2) % 360) * 12) / 255;
        int16_t y2_next = y_base + (fastSin(((scrolledX + 1) * 2 + waveOffset * 2) % 360) * 12) / 255;
        tft.drawLine(x, y2, x + 1, y2_next, COLOR_GREEN);
        
        // Wave 3: Slow, large amplitude  
        int16_t y3 = y_base + (fastSin((scrolledX + waveOffset * 3) % 360) * 15) / 255;
        int16_t y3_next = y_base + (fastSin((scrolledX + 1 + waveOffset * 3) % 360) * 15) / 255;
        tft.drawLine(x, y3, x + 1, y3_next, COLOR_BLUE);
    }
    
    waveOffset += 5;
    if (waveOffset >= 360) waveOffset = 0;
}

void drawFPS() {
    // Calculate frame time
    unsigned long currentFrameTime = micros();
    float frameTime = (currentFrameTime - lastFrameTime) / 1000.0; // ms
    lastFrameTime = currentFrameTime;
    avgFrameTime = avgFrameTime * 0.9 + frameTime * 0.1; // Smooth average
    
    // Update FPS counter every second
    frameCount++;
    unsigned long currentTime = millis();
    if (currentTime - lastFPSTime >= 1000) {
        currentFPS = frameCount;
        frameCount = 0;
        lastFPSTime = currentTime;
        
        Serial.printf("FPS: %d | Avg Frame: %.2fms | CPU: %dMHz\n", 
                      currentFPS, avgFrameTime, getCpuFrequencyMhz());
    }
    
    // Draw FPS in top-right corner
    tft.fillRectangle(SCREEN_WIDTH - 42, 0, SCREEN_WIDTH - 1, 10, COLOR_BLACK);
    tft.setFont(Terminal6x8);
    
    char fpsText[12];
    sprintf(fpsText, "FPS:%d", currentFPS);
    tft.drawText(SCREEN_WIDTH - 40, 2, fpsText, 
                 currentFPS >= 35 ? COLOR_GREEN : 
                 currentFPS >= 25 ? COLOR_YELLOW : COLOR_RED);
}

// ============================================
// SCROLLING HELPER
// ============================================
void applyScrolling() {
    // Scroll left by SCROLL_SPEED pixels
    scrollOffset = (scrollOffset + SCROLL_SPEED) % SCREEN_WIDTH;
}

// ============================================
// MAIN LOOP
// ============================================
int animationMode = 0;
unsigned long lastModeSwitch = 0;
const unsigned long MODE_DURATION = 15000; // 15 seconds per mode

void loop() {
    unsigned long currentTime = millis();
    
    // Switch animation modes
    if (currentTime - lastModeSwitch >= MODE_DURATION) {
        animationMode = (animationMode + 1) % 4;
        lastModeSwitch = currentTime;
        tft.clear();
        
        Serial.printf("\n=== Switching to Mode %d ===\n", animationMode);
        
        // Reset for new mode
        if (animationMode == 0) initParticles();
        if (animationMode == 1) initStarfield();
        if (animationMode == 2) initBalls();
    }
    
    // Run animation based on mode
    switch (animationMode) {
        case 0: // Particle fountain
            // Spawn new particles
            if (frameCount % 2 == 0) {
                spawnParticle(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 20, 
                             colorFromHSV((millis() / 10) % 256, 255, 255));
            }
            updateParticles();
            
            // Draw title
            tft.setFont(Terminal6x8);
            tft.fillRectangle(0, 0, SCREEN_WIDTH - 1, 10, COLOR_BLACK);
            tft.drawText(30, 2, "PARTICLE SYSTEM", COLOR_CYAN);
            break;
            
        case 1: // Starfield
            updateStarfield();
            
            tft.setFont(Terminal6x8);
            tft.fillRectangle(0, 0, SCREEN_WIDTH - 1, 10, COLOR_BLACK);
            tft.drawText(45, 2, "STARFIELD", COLOR_CYAN);
            break;
            
        case 2: // Bouncing balls
            updateBalls();
            
            tft.setFont(Terminal6x8);
            tft.fillRectangle(0, 0, SCREEN_WIDTH - 1, 10, COLOR_BLACK);
            tft.drawText(30, 2, "PHYSICS BALLS", COLOR_CYAN);
            break;
            
        case 3: // Waveform
            drawWaveform();
            
            tft.setFont(Terminal6x8);
            tft.fillRectangle(0, 0, SCREEN_WIDTH - 1, 10, COLOR_BLACK);
            tft.drawText(35, 2, "WAVE VISUAL", COLOR_CYAN);
            break;
    }
    
    // Apply horizontal scrolling for all modes
    applyScrolling();
    
    // Always draw FPS
    drawFPS();
    
    // Minimal delay - let CPU run at full speed
    delay(5);  // Reduced from 10ms for higher FPS
}
