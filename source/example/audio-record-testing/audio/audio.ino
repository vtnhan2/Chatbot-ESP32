/**
 * @file audio.ino
 * @brief ESP32 Audio Recording and Playback Demo
 * 
 * Features:
 * - Record audio from INMP441 I2S microphone
 * - Save to SD card as WAV file
 * - Playback through MAX98357 I2S amplifier
 * - Auto test loop: Record 10s → Wait 5s → Playback 10s
 * 
 * Hardware:
 * - ESP32 WROOM 32 DevKit
 * - INMP441 I2S MEMS Microphone
 * - MAX98357 I2S Class D Amplifier
 * - SD Card Module (SPI)
 * 
 * See WIRING.md for pin connections
 */

#include <driver/i2s.h>
#include <SD.h>
#include <SPI.h>

// ============================================
// PIN DEFINITIONS
// ============================================

// INMP441 Microphone (I2S Input - I2S_NUM_0)
#define I2S_MIC_SCK     32   // Serial Clock
#define I2S_MIC_WS      25   // Word Select (LRCLK)
#define I2S_MIC_SD      33   // Serial Data

// MAX98357 Amplifier (I2S Output - I2S_NUM_1)
#define I2S_SPK_BCLK    26   // Bit Clock
#define I2S_SPK_LRC     27   // Left/Right Clock
#define I2S_SPK_DIN     14   // Data In

// SD Card Module (HSPI - avoid conflict with LCD and I2S)
#define SD_MISO         12
#define SD_MOSI         13
#define SD_SCK          21   // Changed from 18 to avoid conflict with LCD CLK
#define SD_CS           22

// Status LED (built-in)
#define LED_PIN         2

// ============================================
// AUDIO CONFIGURATION
// ============================================
#define SAMPLE_RATE     16000  // 16 kHz (good for voice)
#define BITS_PER_SAMPLE 16     // 16-bit
#define CHANNELS        1      // Mono
#define RECORD_TIME     10     // seconds
#define PLAYBACK_TIME   10     // seconds
#define PAUSE_TIME      5      // seconds

#define I2S_READ_LEN    (1024 * 2)  // Buffer size for reading
#define FLASH_RECORD_SIZE (SAMPLE_RATE * RECORD_TIME * BITS_PER_SAMPLE / 8)

// WAV file name
#define WAV_FILE_NAME   "/recording.wav"

// ============================================
// WAV FILE HEADER STRUCTURE
// ============================================
struct WAVHeader {
  char riff[4] = {'R', 'I', 'F', 'F'};
  uint32_t fileSize;
  char wave[4] = {'W', 'A', 'V', 'E'};
  char fmt[4] = {'f', 'm', 't', ' '};
  uint32_t fmtSize = 16;
  uint16_t audioFormat = 1;  // PCM
  uint16_t numChannels = CHANNELS;
  uint32_t sampleRate = SAMPLE_RATE;
  uint32_t byteRate = SAMPLE_RATE * CHANNELS * BITS_PER_SAMPLE / 8;
  uint16_t blockAlign = CHANNELS * BITS_PER_SAMPLE / 8;
  uint16_t bitsPerSample = BITS_PER_SAMPLE;
  char data[4] = {'d', 'a', 't', 'a'};
  uint32_t dataSize;
};

// ============================================
// GLOBAL VARIABLES
// ============================================
File wavFile;
WAVHeader wavHeader;
int16_t i2s_read_buffer[I2S_READ_LEN];
uint8_t playback_buffer[I2S_READ_LEN * 2];

// ============================================
// SETUP
// ============================================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("\n\n========================================");
  Serial.println("  ESP32 Audio Recording & Playback");
  Serial.println("========================================\n");
  
  // Initialize SD Card
  if (!initSDCard()) {
    Serial.println("✗ SD Card initialization failed!");
    Serial.println("Please check:");
    Serial.println("  - SD card is inserted");
    Serial.println("  - SD card is formatted (FAT32)");
    Serial.println("  - Wiring connections");
    while (1) {
      blinkError();
    }
  }
  
  Serial.println("✓ System ready!\n");
  Serial.println("Auto Test Loop:");
  Serial.println("  1. Record 10 seconds");
  Serial.println("  2. Wait 5 seconds");
  Serial.println("  3. Playback 10 seconds");
  Serial.println("  4. Repeat\n");
  
  delay(2000);
}

// ============================================
// MAIN LOOP
// ============================================
void loop() {
  // Step 1: Record audio
  Serial.println("\n" + String('=', 50));
  Serial.println("🎙️  RECORDING AUDIO...");
  Serial.println(String('=', 50));
  recordAudio();
  
  // Step 2: Pause
  Serial.println("\n" + String('=', 50));
  Serial.println("⏸️  PAUSING...");
  Serial.println(String('=', 50));
  pauseWithBlink(PAUSE_TIME);
  
  // Step 3: Playback audio
  Serial.println("\n" + String('=', 50));
  Serial.println("🔊 PLAYING BACK...");
  Serial.println(String('=', 50));
  playbackAudio();
  
  Serial.println("\n✓ Cycle complete! Restarting...\n");
  delay(1000);
}

// ============================================
// SD CARD FUNCTIONS
// ============================================
bool initSDCard() {
  Serial.println("Initializing SD card...");
  
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  
  if (!SD.begin(SD_CS)) {
    return false;
  }
  
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    return false;
  }
  
  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
  
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %llu MB\n", cardSize);
  
  return true;
}

// ============================================
// I2S MICROPHONE SETUP
// ============================================
void setupI2SMicrophone() {
  Serial.println("Setting up I2S microphone...");
  
  // Uninstall if already installed
  i2s_driver_uninstall(I2S_NUM_0);
  
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };
  
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_MIC_SCK,
    .ws_io_num = I2S_MIC_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_MIC_SD
  };
  
  esp_err_t err = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  if (err != ESP_OK) {
    Serial.printf("✗ I2S mic install error: %d\n", err);
    return;
  }
  i2s_set_pin(I2S_NUM_0, &pin_config);
  i2s_zero_dma_buffer(I2S_NUM_0);
  
  Serial.println("✓ I2S microphone ready");
}

// ============================================
// I2S SPEAKER SETUP
// ============================================
void setupI2SSpeaker() {
  Serial.println("Setting up I2S speaker...");
  
  // Uninstall if already installed
  i2s_driver_uninstall(I2S_NUM_1);
  
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };
  
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SPK_BCLK,
    .ws_io_num = I2S_SPK_LRC,
    .data_out_num = I2S_SPK_DIN,
    .data_in_num = I2S_PIN_NO_CHANGE
  };
  
  esp_err_t err = i2s_driver_install(I2S_NUM_1, &i2s_config, 0, NULL);
  if (err != ESP_OK) {
    Serial.printf("✗ I2S speaker install error: %d\n", err);
    return;
  }
  i2s_set_pin(I2S_NUM_1, &pin_config);
  i2s_zero_dma_buffer(I2S_NUM_1);
  
  Serial.println("✓ I2S speaker ready");
}

// ============================================
// AUDIO RECORDING
// ============================================
void recordAudio() {
  setupI2SMicrophone();
  
  // Create WAV file
  wavFile = SD.open(WAV_FILE_NAME, FILE_WRITE);
  if (!wavFile) {
    Serial.println("✗ Failed to create WAV file");
    return;
  }
  
  // Write placeholder header (will update later)
  wavHeader.fileSize = 0;
  wavHeader.dataSize = 0;
  wavFile.write((uint8_t*)&wavHeader, sizeof(WAVHeader));
  
  Serial.printf("Recording to: %s\n", WAV_FILE_NAME);
  Serial.printf("Duration: %d seconds\n", RECORD_TIME);
  Serial.printf("Sample Rate: %d Hz\n", SAMPLE_RATE);
  Serial.println("Speak now!\n");
  
  uint32_t totalBytesWritten = 0;
  uint32_t startTime = millis();
  uint32_t recordDuration = RECORD_TIME * 1000;
  
  while (millis() - startTime < recordDuration) {
    size_t bytesRead = 0;
    
    // Read from I2S microphone
    i2s_read(I2S_NUM_0, &i2s_read_buffer, I2S_READ_LEN * sizeof(int16_t), &bytesRead, portMAX_DELAY);
    
    if (bytesRead > 0) {
      // Write to SD card
      size_t bytesWritten = wavFile.write((uint8_t*)i2s_read_buffer, bytesRead);
      totalBytesWritten += bytesWritten;
      
      // Blink LED during recording
      digitalWrite(LED_PIN, (millis() / 250) % 2);
      
      // Progress indicator
      if ((millis() - startTime) % 1000 < 50) {
        Serial.print(".");
      }
    }
  }
  
  digitalWrite(LED_PIN, LOW);
  Serial.println("\n");
  
  // Update WAV header with actual sizes
  wavHeader.fileSize = totalBytesWritten + sizeof(WAVHeader) - 8;
  wavHeader.dataSize = totalBytesWritten;
  
  wavFile.seek(0);
  wavFile.write((uint8_t*)&wavHeader, sizeof(WAVHeader));
  wavFile.close();
  
  // Uninstall I2S driver
  i2s_driver_uninstall(I2S_NUM_0);
  
  Serial.printf("✓ Recording complete!\n");
  Serial.printf("  File size: %d bytes\n", totalBytesWritten);
  Serial.printf("  Duration: %.1f seconds\n", totalBytesWritten / (float)(SAMPLE_RATE * CHANNELS * BITS_PER_SAMPLE / 8));
}

// ============================================
// AUDIO PLAYBACK
// ============================================
void playbackAudio() {
  setupI2SSpeaker();
  
  // Open WAV file
  wavFile = SD.open(WAV_FILE_NAME, FILE_READ);
  if (!wavFile) {
    Serial.println("✗ Failed to open WAV file");
    i2s_driver_uninstall(I2S_NUM_1);
    return;
  }
  
  // Skip WAV header
  wavFile.seek(sizeof(WAVHeader));
  
  Serial.printf("Playing: %s\n", WAV_FILE_NAME);
  Serial.printf("File size: %d bytes\n", wavFile.size());
  Serial.println("Listen now!\n");
  
  uint32_t totalBytesRead = 0;
  size_t bytesWritten = 0;
  
  while (wavFile.available()) {
    // Read from SD card
    size_t bytesRead = wavFile.read(playback_buffer, sizeof(playback_buffer));
    
    if (bytesRead > 0) {
      // Write to I2S speaker
      i2s_write(I2S_NUM_1, playback_buffer, bytesRead, &bytesWritten, portMAX_DELAY);
      totalBytesRead += bytesRead;
      
      // Blink LED during playback
      digitalWrite(LED_PIN, (millis() / 100) % 2);
      
      // Progress indicator
      if (totalBytesRead % (SAMPLE_RATE * 2) < sizeof(playback_buffer)) {
        Serial.print(".");
      }
    }
  }
  
  digitalWrite(LED_PIN, LOW);
  Serial.println("\n");
  
  wavFile.close();
  
  // Uninstall I2S driver
  i2s_driver_uninstall(I2S_NUM_1);
  
  Serial.printf("✓ Playback complete!\n");
  Serial.printf("  Bytes played: %d\n", totalBytesRead);
}

// ============================================
// UTILITY FUNCTIONS
// ============================================
void pauseWithBlink(int seconds) {
  Serial.printf("Waiting %d seconds...\n", seconds);
  
  for (int i = seconds; i > 0; i--) {
    Serial.printf("  %d... ", i);
    
    // Slow blink
    for (int j = 0; j < 4; j++) {
      digitalWrite(LED_PIN, HIGH);
      delay(125);
      digitalWrite(LED_PIN, LOW);
      delay(125);
    }
    
    Serial.println();
  }
  
  Serial.println("Done!\n");
}

void blinkError() {
  // Fast blink for error
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
  delay(100);
}
