#include "display.h"
#include "TFT_22_ILI9225.h"

// Pin definitions
#define TFT_RST    4
#define TFT_RS     2  // DC pin
#define TFT_CS     5
#define TFT_LED    15
// CLK=18, MOSI=23 (VSPI hardware pins)

// TFT object (using hardware SPI)
TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED);

// Current display state
static int currentY = 0;
static const int LINE_HEIGHT = 16;
static const int MARGIN = 4;
static int animationCounter = 0;  // For loading animation
static bool displayBusy = false;  // Prevent updates during I2S/network operations

// ============================================
// INITIALIZATION
// ============================================

void displayInit() {
  tft.begin();
  tft.setOrientation(0);  // Portrait mode
  tft.setBacklight(true);
  tft.setBackgroundColor(COLOR_BG);
  displayClear();
  
  // Show title
  tft.setFont(Terminal12x16);
  tft.drawText(MARGIN, MARGIN, "AI Voice Chatbot", COLOR_TITLE);
  currentY = 24;
}

// ============================================
// BASIC FUNCTIONS
// ============================================

void displayClear() {
  tft.clear();
  currentY = 0;
}

void displayStatus(const char* status, uint16_t color) {
  // Skip if system busy (I2S/Network using resources)
  if (displayBusy) return;
  
  // Animated dots: . -> .. -> ... -> . (cycle every call)
  int dots = (animationCounter % 3) + 1;  // 1, 2, 3, 1, 2, 3...
  animationCounter++;
  
  // Build status string with dots using Arduino String
  String statusWithDots = String(status);
  for (int i = 0; i < dots; i++) {
    statusWithDots += ".";
  }
  
  // CRITICAL: Give SPI bus time (SD card may be using it)
  delay(10);
  
  // Force clear and redraw to ensure visibility
  tft.fillRectangle(0, 0, SCREEN_WIDTH, 20, COLOR_BG);
  delay(5);
  tft.setFont(Terminal6x8);
  tft.drawText(MARGIN, 2, statusWithDots.c_str(), color);
  
  // Small delay to let display controller process
  delay(5);
}

// ============================================
// MESSAGE DISPLAY
// ============================================

void displayMessage(const char* label, const char* text, uint16_t color) {
  // Check if need to scroll/clear
  if (currentY > SCREEN_HEIGHT - 40) {
    displayClear();
    displayInit();  // Re-show title
  }
  
  // Draw label
  tft.setFont(Terminal6x8);
  tft.drawText(MARGIN, currentY, label, color);
  currentY += 12;
  
  // Draw text (word wrap)
  String msg = String(text);
  int maxChars = 28;  // Approx chars per line with 6x8 font
  int startPos = 0;
  
  while (startPos < msg.length()) {
    int endPos = startPos + maxChars;
    if (endPos > msg.length()) endPos = msg.length();
    
    // Find space for word wrap
    if (endPos < msg.length()) {
      int spacePos = msg.lastIndexOf(' ', endPos);
      if (spacePos > startPos) endPos = spacePos;
    }
    
    String line = msg.substring(startPos, endPos);
    tft.drawText(MARGIN, currentY, line.c_str(), COLOR_TEXT);
    currentY += 10;
    
    startPos = endPos;
    if (msg.charAt(startPos) == ' ') startPos++;  // Skip leading space
    
    // Check bounds
    if (currentY > SCREEN_HEIGHT - 10) break;
  }
  
  currentY += 4;  // Spacing between messages
}

// ============================================
// STATE-SPECIFIC DISPLAYS
// ============================================

void displayListening() {
 // Serial.println("[DISPLAY] Updating: LISTENING");
  tft.setFont(Terminal12x16);
  displayStatus("LISTENING", COLOR_STATUS);
}

void displayRecording() {
  //Serial.println("[DISPLAY] Updating: RECORDING");
  tft.setFont(Terminal12x16);
  displayStatus("RECORDING", COLOR_ERROR);
}

void displayProcessing(const char* step) {
  //Serial.printf("[DISPLAY] Updating: PROCESSING (%s)\n", step);
  tft.setFont(Terminal12x16);
  if (strstr(step, "STT")) {
    displayStatus("PROCESSING", COLOR_STATUS);
  } else if (strstr(step, "Gemini")) {
    displayStatus("ASKING AI", COLOR_STATUS);
  } else {
    displayStatus(step, COLOR_STATUS);
  }
}

void displayQuestion(const char* text) {
  displayMessage("YOU:", text, COLOR_USER);
}

void displayAnswer(const char* text) {
  displayMessage("AI:", text, COLOR_BOT);
}

void displayError(const char* error) {
  if (error == NULL || strlen(error) == 0) {
    error = "Unknown error";
  }
  displayMessage("ERROR:", error, COLOR_ERROR);
  displayStatus("ERROR", COLOR_ERROR);
}

// ============================================
// DISPLAY BUSY CONTROL
// ============================================

void setDisplayBusy(bool busy) {
  displayBusy = busy;
}
