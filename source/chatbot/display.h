#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include "TFT_22_ILI9225.h"

// Display dimensions
#define SCREEN_WIDTH  176
#define SCREEN_HEIGHT 220

// Colors
#define COLOR_BG       0x0000  // Black
#define COLOR_TEXT     0xFFFF  // White
#define COLOR_TITLE    0x07FF  // Cyan
#define COLOR_USER     0x07E0  // Green
#define COLOR_BOT      0xF81F  // Magenta
#define COLOR_STATUS   0xFFE0  // Yellow
#define COLOR_ERROR    0xF800  // Red

// Display functions
void displayInit();
void displayClear();
void displayStatus(const char* status, uint16_t color = COLOR_STATUS);
void displayMessage(const char* label, const char* text, uint16_t color);
void displayListening();
void displayRecording();
void displayProcessing(const char* step);
void displayQuestion(const char* text);
void displayAnswer(const char* text);
void displayError(const char* error);

// Display busy control (pause during I2S/network operations)
void setDisplayBusy(bool busy);

#endif // DISPLAY_H
