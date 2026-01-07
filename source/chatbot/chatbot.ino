/**
 * @file chatbot.ino
 * @brief ESP32 AI Voice Chatbot with Wit.ai STT + Google TTS + Gemini
 * 
 * Features:
 * - Always-listening mode with Voice Activity Detection
 * - Wit.ai Speech-to-Text (Free, unlimited)
 * ESP32 AI Voice Chatbot with TFT Display
 * - Wit.ai STT (Vietnamese support)
 * - Google Gemini AI
 * - Google Translate TTS
 * - Always-listening with VAD
 * - 240MHz CPU overclock
 * - ILI9225 TFT Display
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <NetworkClientSecure.h>
#include <ArduinoJson.h>
#include <SD.h>
#include <FS.h>
#include <SPI.h>
#include <driver/i2s.h>
#include "display.h"      // TFT Display module
#include "../secret.h"

// ============================================
// PIN DEFINITIONS
// ============================================

// INMP441 Microphone
#define I2S_MIC_SCK     32
#define I2S_MIC_WS      25
#define I2S_MIC_SD      33

// MAX98357 Amplifier
#define I2S_SPK_BCLK    26
#define I2S_SPK_LRC     27
#define I2S_SPK_DIN     14

// SD Card
#define SD_MISO         12
#define SD_MOSI         13
#define SD_SCK          21
#define SD_CS           22

// TFT Display - pins defined in display.cpp

// ============================================
// CONFIGURATION
// ============================================

#define SAMPLE_RATE     16000
#define BITS_PER_SAMPLE 16
#define CHANNELS        1
#define DMA_BUF_COUNT   8
#define DMA_BUF_LEN     1024

// Voice Activity Detection
#define VAD_THRESHOLD       300     // Energy threshold for voice detection
#define VAD_SILENCE_MS      1500    // ms of silence = end of speech
#define VAD_MIN_SPEECH_MS   500     // Minimum speech duration
#define MAX_RECORD_TIME_MS  10000   // Max 10 seconds per question

// File paths
#define AUDIO_INPUT_FILE    "/question.wav"
#define AUDIO_OUTPUT_FILE   "/answer.mp3"

// Buffer sizes
#define CHUNK_SIZE          2048
#define JSON_BUFFER_SIZE    4096

// API Endpoints
#define WIT_AI_URL          "https://api.wit.ai/speech?v=20220622"
#define GEMINI_URL          "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.0-flash-exp:generateContent"
#define GOOGLE_TTS_URL      "https://translate.google.com/translate_tts"

// Mock mode - bypass Gemini API (for testing when quota exceeded)
#define MOCK_MODE true    // Set to true to use mock responses

// ============================================
// WAV FILE HEADER STRUCTURE
// ============================================

struct WAVHeader {
  char riff[4] = {'R', 'I', 'F', 'F'};
  uint32_t fileSize;
  char wave[4] = {'W', 'A', 'V', 'E'};
  char fmt[4] = {'f', 'm', 't', ' '};
  uint32_t fmtSize = 16;
  uint16_t audioFormat = 1;
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

File audioFile;
int16_t audioBuffer[CHUNK_SIZE / 2];
HTTPClient http;
NetworkClientSecure secureClient;  // Use secure client for HTTPS

enum State {
  STATE_LISTENING,
  STATE_RECORDING,
  STATE_PROCESSING_STT,
  STATE_PROCESSING_GEMINI,
  STATE_PROCESSING_TTS,
  STATE_SPEAKING,
  STATE_ERROR
};

State currentState = STATE_LISTENING;
String lastError = "";

// ============================================
// FUNCTION DECLARATIONS
// ============================================

bool initWiFi();
bool initSD();
void setupI2SMicrophone();
void setupI2SSpeaker();
bool detectVoiceActivity();
void recordQuestion();
void processSTT();
void processGemini();
void processTTS();
void playSpeech();
String urlEncode(String str);
// Display functions defined in display.h/cpp

// ============================================
// VIETNAMESE TONE REMOVAL FOR DISPLAY
// ============================================

String removeVietnameseTones(String text) {
  // Convert UTF-8 Vietnamese to non-accented ASCII
  text.replace("à", "a"); text.replace("á", "a"); text.replace("ả", "a"); text.replace("ã", "a"); text.replace("ạ", "a");
  text.replace("ă", "a"); text.replace("ằ", "a"); text.replace("ắ", "a"); text.replace("ẳ", "a"); text.replace("ẵ", "a"); text.replace("ặ", "a");
  text.replace("â", "a"); text.replace("ầ", "a"); text.replace("ấ", "a"); text.replace("ẩ", "a"); text.replace("ẫ", "a"); text.replace("ậ", "a");
  text.replace("è", "e"); text.replace("é", "e"); text.replace("ẻ", "e"); text.replace("ẽ", "e"); text.replace("ẹ", "e");
  text.replace("ê", "e"); text.replace("ề", "e"); text.replace("ế", "e"); text.replace("ể", "e"); text.replace("ễ", "e"); text.replace("ệ", "e");
  text.replace("ì", "i"); text.replace("í", "i"); text.replace("ỉ", "i"); text.replace("ĩ", "i"); text.replace("ị", "i");
  text.replace("ò", "o"); text.replace("ó", "o"); text.replace("ỏ", "o"); text.replace("õ", "o"); text.replace("ọ", "o");
  text.replace("ô", "o"); text.replace("ồ", "o"); text.replace("ố", "o"); text.replace("ổ", "o"); text.replace("ỗ", "o"); text.replace("ộ", "o");
  text.replace("ơ", "o"); text.replace("ờ", "o"); text.replace("ớ", "o"); text.replace("ở", "o"); text.replace("ỡ", "o"); text.replace("ợ", "o");
  text.replace("ù", "u"); text.replace("ú", "u"); text.replace("ủ", "u"); text.replace("ũ", "u"); text.replace("ụ", "u");
  text.replace("ư", "u"); text.replace("ừ", "u"); text.replace("ứ", "u"); text.replace("ử", "u"); text.replace("ữ", "u"); text.replace("ự", "u");
  text.replace("ỳ", "y"); text.replace("ý", "y"); text.replace("ỷ", "y"); text.replace("ỹ", "y"); text.replace("ỵ", "y");
  text.replace("đ", "d");
  // Uppercase
  text.replace("À", "A"); text.replace("Á", "A"); text.replace("Ả", "A"); text.replace("Ã", "A"); text.replace("Ạ", "A");
  text.replace("Ă", "A"); text.replace("Ằ", "A"); text.replace("Ắ", "A"); text.replace("Ẳ", "A"); text.replace("Ẵ", "A"); text.replace("Ặ", "A");
  text.replace("Â", "A"); text.replace("Ầ", "A"); text.replace("Ấ", "A"); text.replace("Ẩ", "A"); text.replace("Ẫ", "A"); text.replace("Ậ", "A");
  text.replace("È", "E"); text.replace("É", "E"); text.replace("Ẻ", "E"); text.replace("Ẽ", "E"); text.replace("Ẹ", "E");
  text.replace("Ê", "E"); text.replace("Ề", "E"); text.replace("Ế", "E"); text.replace("Ể", "E"); text.replace("Ễ", "E"); text.replace("Ệ", "E");
  text.replace("Ì", "I"); text.replace("Í", "I"); text.replace("Ỉ", "I"); text.replace("Ĩ", "I"); text.replace("Ị", "I");
  text.replace("Ò", "O"); text.replace("Ó", "O"); text.replace("Ỏ", "O"); text.replace("Õ", "O"); text.replace("Ọ", "O");
  text.replace("Ô", "O"); text.replace("Ồ", "O"); text.replace("Ố", "O"); text.replace("Ổ", "O"); text.replace("Ỗ", "O"); text.replace("Ộ", "O");
  text.replace("Ơ", "O"); text.replace("Ờ", "O"); text.replace("Ớ", "O"); text.replace("Ở", "O"); text.replace("Ỡ", "O"); text.replace("Ợ", "O");
  text.replace("Ù", "U"); text.replace("Ú", "U"); text.replace("Ủ", "U"); text.replace("Ũ", "U"); text.replace("Ụ", "U");
  text.replace("Ư", "U"); text.replace("Ừ", "U"); text.replace("Ứ", "U"); text.replace("Ử", "U"); text.replace("Ữ", "U"); text.replace("Ự", "U");
  text.replace("Ỳ", "Y"); text.replace("Ý", "Y"); text.replace("Ỷ", "Y"); text.replace("Ỹ", "Y"); text.replace("Ỵ", "Y");
  text.replace("Đ", "D");
  return text;
}


// ============================================
// SETUP
// ============================================

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Overclock CPU to 240MHz
  setCpuFrequencyMhz(240);
  Serial.printf("\n🚀 CPU Frequency: %d MHz\n", getCpuFrequencyMhz());
  // Init display
  displayInit();
  displayStatus("Initializing system  ...", COLOR_STATUS);
  
  Serial.println("\n========================================");
  Serial.println("   🤖 ESP32 AI Voice Chatbot");
  Serial.println("========================================\n");
  
  // Initialize WiFi
  if (!initWiFi()) {
    Serial.println("✗ WiFi failed");
    displayError("WiFi failed");
    while(1) delay(1000);
  }
  
  // Initialize SD Card
  if (!initSD()) {
    Serial.println("✗ SD Card failed");
    displayError("SD Card failed");
    while(1) delay(1000);
  }
  
  // Initialize I2S
  setupI2SMicrophone();
  setupI2SSpeaker();
  
  Serial.println("\n✓ System ready!");
  displayError("System ready!");
  Serial.println("🎙️  Always listening for your question...\n");
  
  currentState = STATE_LISTENING;
  displayListening();
}

// ============================================
// MAIN LOOP - ALWAYS LISTENING
// ============================================

void loop() {
  switch(currentState) {
    case STATE_LISTENING:
      // Update display animation to show system is alive
      static uint32_t lastDisplayUpdate = 0;
      if (millis() - lastDisplayUpdate > 2000) {  // Update every 2 seconds (slower to avoid SPI conflicts)
        displayListening();
        lastDisplayUpdate = millis();
      }
      
      // Continuously monitor for voice activity
      if (detectVoiceActivity()) {
        Serial.println("\n🎙️  Voice detected! Recording...");
        setDisplayBusy(true);  // Pause display before I2S recording
        displayRecording();
        currentState = STATE_RECORDING;
        recordQuestion();
      }
      break;
      
    case STATE_RECORDING:
      // Recording handled in recordQuestion()
      break;
      
    case STATE_PROCESSING_STT:
      processSTT();
      break;
      
    case STATE_PROCESSING_GEMINI:
      processGemini();
      break;
      
    case STATE_PROCESSING_TTS:
      processTTS();
      break;
      
    case STATE_SPEAKING:
      playSpeech();
      currentState = STATE_LISTENING;
      // displayListening();  // DISABLED
      Serial.println("\n🎙️  Listening...");
      break;
      
    case STATE_ERROR:
      Serial.printf("⚠️ Error: %s\n", lastError.length() > 0 ? lastError.c_str() : "Unknown error");
      if (lastError.length() > 0) {
        displayError(lastError.c_str());
      } else {
        displayError("Unknown error");
      }
      delay(3000);
      currentState = STATE_LISTENING;
      displayListening();
      break;
  }
  
  delay(10);
}

// ============================================
// WIFI INITIALIZATION
// ============================================

bool initWiFi() {
  Serial.println("\n--- WiFi Network Scan ---");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  int n = WiFi.scanNetworks();
  Serial.printf("Found %d networks:\n", n);
  for (int i = 0; i < n; i++) {
    Serial.printf("%d: %s (Ch:%d, RSSI:%d dBm) %s\n", 
                  i + 1,
                  WiFi.SSID(i).c_str(), 
                  WiFi.channel(i),
                  WiFi.RSSI(i),
                  WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "OPEN" : "ENCRYPTED");
  }
  Serial.println("------------------------\n");
  
  Serial.print("Connecting to WiFi: tnhan");
  WiFi.begin("tnhan", "tnhan211");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 200) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi connected");
    Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
    
    // Use Google DNS to bypass network DNS blocking
    IPAddress dns1(8, 8, 8, 8);
    IPAddress dns2(8, 8, 4, 4);
    WiFi.config(WiFi.localIP(), WiFi.gatewayIP(), WiFi.subnetMask(), dns1, dns2);
    Serial.println("✓ DNS set to Google (8.8.8.8)");
    
    return true;
  }
  
  Serial.printf("\n✗ WiFi connection failed! Status: %d\n", WiFi.status());
  Serial.println("\nPossible issues:");
  Serial.println("- SSID 'tnhan' not found in scan above");
  Serial.println("- WiFi is 5GHz (ESP32 only supports 2.4GHz)");
  Serial.println("- Wrong password");
  Serial.println("- Signal too weak");
  return false;
}

// ============================================
// SD CARD INITIALIZATION
// ============================================

bool initSD() {
  Serial.println("Initializing SD card...");
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  
  if (!SD.begin(SD_CS)) {
    return false;
  }
  
  Serial.printf("✓ SD Card: %llu MB\n", SD.cardSize() / (1024 * 1024));
  return true;
}

// ============================================
// I2S SETUP
// ============================================

void setupI2SMicrophone() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = DMA_BUF_COUNT,
    .dma_buf_len = DMA_BUF_LEN,
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
  
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
  i2s_zero_dma_buffer(I2S_NUM_0);
}

void setupI2SSpeaker() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = DMA_BUF_COUNT,
    .dma_buf_len = DMA_BUF_LEN,
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
  
  i2s_driver_install(I2S_NUM_1, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_1, &pin_config);
  i2s_zero_dma_buffer(I2S_NUM_1);
}

// ============================================
// VOICE ACTIVITY DETECTION
// ============================================

bool detectVoiceActivity() {
  size_t bytesRead = 0;
  i2s_read(I2S_NUM_0, audioBuffer, CHUNK_SIZE, &bytesRead, 10);
  
  if (bytesRead == 0) return false;
  
  // Calculate energy
  int32_t energy = 0;
  int samples = bytesRead / sizeof(int16_t);
  for (int i = 0; i < samples; i++) {
    energy += abs(audioBuffer[i]);
  }
  energy /= samples;
  
  return (energy > VAD_THRESHOLD);
}

// ============================================
// AUDIO RECORDING
// ============================================

void recordQuestion() {
  // Delete old file
  if (SD.exists(AUDIO_INPUT_FILE)) {
    SD.remove(AUDIO_INPUT_FILE);
  }
  
  // Create WAV file
  audioFile = SD.open(AUDIO_INPUT_FILE, FILE_WRITE);
  if (!audioFile) {
    lastError = "Cannot create audio file";
    currentState = STATE_ERROR;
    return;
  }
  
  // Write WAV header (will update later)
  WAVHeader header;
  audioFile.write((uint8_t*)&header, sizeof(WAVHeader));
  
  uint32_t totalBytes = 0;
  uint32_t silenceStart = 0;
  uint32_t recordStart = millis();
  bool speechDetected = false;
  
  while (true) {
    size_t bytesRead = 0;
    i2s_read(I2S_NUM_0, audioBuffer, CHUNK_SIZE, &bytesRead, portMAX_DELAY);
    
    if (bytesRead > 0) {
      // Calculate energy for VAD
      int32_t energy = 0;
      int samples = bytesRead / sizeof(int16_t);
      for (int i = 0; i < samples; i++) {
        energy += abs(audioBuffer[i]);
      }
      energy /= samples;
      
      // Write to file
      audioFile.write((uint8_t*)audioBuffer, bytesRead);
      totalBytes += bytesRead;
      
      // Voice activity check
      if (energy > VAD_THRESHOLD) {
        speechDetected = true;
        silenceStart = 0;
      } else if (speechDetected) {
        if (silenceStart == 0) {
          silenceStart = millis();
        } else if (millis() - silenceStart > VAD_SILENCE_MS) {
          // End of speech
          break;
        }
      }
      
      // Max time limit
      if (millis() - recordStart > MAX_RECORD_TIME_MS) {
        break;
      }
      
      // Minimum speech check - DISABLED (was cutting off valid audio)
      // if (!speechDetected && millis() - recordStart > VAD_MIN_SPEECH_MS * 2) {
      //   lastError = "No speech detected";
      //   audioFile.close();
      //   currentState = STATE_ERROR;
      //   return;
      // }
    }
  }
  
  // Update WAV header
  header.dataSize = totalBytes;
  header.fileSize = totalBytes + sizeof(WAVHeader) - 8;
  audioFile.seek(0);
  audioFile.write((uint8_t*)&header, sizeof(WAVHeader));
  
  // CRITICAL: Flush before close to ensure all data written
  audioFile.flush();
  delay(100);  // Give SD card time to finish
  audioFile.close();
  
  // NOTE: Do NOT uninstall I2S driver! detectVoiceActivity() needs it!
  // i2s_driver_uninstall(I2S_NUM_0);  // DISABLED - causes crash
  
  Serial.printf("✓ Recorded %d bytes (%.1f sec)\n", totalBytes, totalBytes / (float)(SAMPLE_RATE * 2));
  setDisplayBusy(false);  // Resume display after recording
  
  // Small delay before next state
  delay(200);
  
  currentState = STATE_PROCESSING_STT;
}

// ============================================
// SPEECH-TO-TEXT (Wit.ai)
// ============================================

String questionText = "";

void processSTT() {
  setDisplayBusy(true);  // Pause display during network operation
  Serial.println("🔄 Converting speech to text...");
  displayProcessing("STT Processing...");
  
  // Open WAV file
  audioFile = SD.open(AUDIO_INPUT_FILE, FILE_READ);
  if (!audioFile) {
    lastError = "Cannot open audio file";
    currentState = STATE_ERROR;
    return;
  }
  
  size_t audioSize = audioFile.size();  // Send entire WAV file
  Serial.printf("Audio file size: %d bytes (including WAV header)\n", audioSize);
  
  // Check first few bytes to verify it's a valid WAV
  audioFile.seek(0);
  char riffCheck[4];
  audioFile.read((uint8_t*)riffCheck, 4);
  Serial.printf("File header: %c%c%c%c\n", riffCheck[0], riffCheck[1], riffCheck[2], riffCheck[3]);
  audioFile.seek(0);  // Reset to beginning
  
  // Read and print some audio samples to verify data
  audioFile.seek(44);  // Skip to data after WAV header
  int16_t samples[10];
  audioFile.read((uint8_t*)samples, 20);
  Serial.print("Sample data: ");
  for(int i = 0; i < 10; i++) {
    Serial.printf("%d ", samples[i]);
  }
  Serial.println();
  audioFile.seek(0);  // Reset to beginning for upload
  
  // Prepare HTTP request
  Serial.println("Connecting to Wit.ai...");
  Serial.printf("URL: %s\n", WIT_AI_URL);
  
  // Test DNS resolution
  IPAddress witIP;
  if (WiFi.hostByName("api.wit.ai", witIP)) {
    Serial.printf("DNS resolved: api.wit.ai = %s\n", witIP.toString().c_str());
  } else {
    Serial.println("✗ DNS resolution failed for api.wit.ai!");
    lastError = "DNS failed";
    currentState = STATE_ERROR;
    audioFile.close();
    return;
  }
  
  // Setup secure client for HTTPS - CRITICAL CLEANUP
  secureClient.stop();  // Close any existing connection
  delay(200);  // Longer delay for cleanup
  
  // Reinitialize SSL
  secureClient = NetworkClientSecure();  // Fresh instance
  secureClient.setInsecure();  // Skip SSL certificate validation
  secureClient.setTimeout(20);  // 20 second timeout
  
  // Try direct connection first
  Serial.println("Testing direct SSL connection...");
  if (!secureClient.connect("api.wit.ai", 443)) {
    Serial.println("✗ Direct SSL connection failed!");
    Serial.printf("Error: %d\n", secureClient.lastError(NULL, 0));
    
    // Retry once
    Serial.println("Retrying SSL connection...");
    delay(500);
    secureClient.stop();
    delay(200);
    secureClient = NetworkClientSecure();
    secureClient.setInsecure();
    
    if (!secureClient.connect("api.wit.ai", 443)) {
      Serial.println("✗ SSL retry failed!");
      lastError = "SSL connect failed";
      currentState = STATE_ERROR;
      audioFile.close();
      return;
    }
  }
  
  secureClient.stop();  // Close test connection
  Serial.println("✓ SSL test connection successful");
  
  http.setTimeout(30000);  // 30 second timeout for upload
  
  if (!http.begin(secureClient, WIT_AI_URL)) {
    Serial.println("✗ Failed to begin HTTP connection");
    lastError = "HTTP begin failed";
    currentState = STATE_ERROR;
    audioFile.close();
    return;
  }
  
  http.addHeader("Authorization", String("Bearer ") + API_KEY_STT);
  http.addHeader("Content-Type", "audio/wav");  // Send as WAV file
  
  Serial.println("Sending audio to Wit.ai...");
  
  // Send audio data
  int httpCode = http.sendRequest("POST", &audioFile, audioSize);
  audioFile.close();
  
  Serial.printf("HTTP Response Code: %d\n", httpCode);
  
  if (httpCode == 200) {
    String response = http.getString();
    Serial.printf("Response: %s\n", response.c_str());
    
    // Parse JSON
    StaticJsonDocument<JSON_BUFFER_SIZE> doc;
    DeserializationError error = deserializeJson(doc, response);
    
    if (!error) {
      questionText = doc["text"].as<String>();
      
      if (questionText.length() > 0) {
        Serial.print("👤 You said: \"");
        Serial.print(questionText);
        Serial.println("\"");
        String displayText = removeVietnameseTones(questionText);
        displayQuestion(displayText.c_str());
        setDisplayBusy(false);  // Resume display after STT complete
        currentState = STATE_PROCESSING_GEMINI;
      } else {
        lastError = "No text recognized";
        currentState = STATE_ERROR;
      }
    } else {
      Serial.printf("✗ JSON parse error: %s\n", error.c_str());
      lastError = "JSON parse error";
      currentState = STATE_ERROR;
    }
  } else if (httpCode > 0) {
    String response = http.getString();
    Serial.printf("✗ STT API error %d: %s\n", httpCode, response.c_str());
    lastError = "STT API error: " + String(httpCode);
    currentState = STATE_ERROR;
  } else {
    Serial.printf("✗ Connection failed! Error: %s\n", http.errorToString(httpCode).c_str());
    lastError = "STT connection failed: " + String(httpCode);
    currentState = STATE_ERROR;
  }
  
  http.end();
}

// ============================================
// GEMINI AI
// ============================================

String answerText = "";

void processGemini() {
  // Rate limit protection - wait 5 seconds between requests
  static uint32_t lastGeminiRequest = 0;
  uint32_t timeSinceLastRequest = millis() - lastGeminiRequest;
  if (timeSinceLastRequest < 5000) {
    uint32_t waitTime = 5000 - timeSinceLastRequest;
    Serial.printf("⏳ Rate limit: waiting %d ms...\n", waitTime);
    delay(waitTime);
  }
  lastGeminiRequest = millis();
  
  setDisplayBusy(true);  // Pause display during network operation
  Serial.println("🔄 Asking Gemini AI...");
  displayProcessing("Gemini AI...");
  
  // MOCK MODE - Bypass API when quota exceeded
  #if MOCK_MODE
    Serial.println("⚠️  MOCK MODE: Using predefined response");
    delay(1000);  // Simulate network delay
    
    // Mock responses based on question keywords
    if (questionText.indexOf("chào") >= 0 || questionText.indexOf("xin") >= 0) {
      answerText = "Xin chào! Tôi là trợ lý AI. Tôi có thể giúp gì cho bạn?";
    } else if (questionText.indexOf("giờ") >= 0) {
      answerText = "Hiện tại là " + String(millis() / 1000) + " giây kể từ khi khởi động.";
    } else if (questionText.indexOf("tên") >= 0) {
      answerText = "Tôi là chatbot AI chạy trên ESP32. Rất vui được gặp bạn!";
    } else {
      answerText = "Tôi đã nhận được câu hỏi: " + questionText + ". Đây là câu trả lời test.";
    }
    
    Serial.print("🤖 Gemini (MOCK): \"");
    Serial.print(answerText);
    Serial.println("\"");
    
    String displayText = removeVietnameseTones(answerText);
    displayAnswer(displayText.c_str());
    setDisplayBusy(false);
    currentState = STATE_PROCESSING_TTS;
    return;
  #endif
  
  // Prepare request
  StaticJsonDocument<JSON_BUFFER_SIZE> requestDoc;
  JsonArray contents = requestDoc.createNestedArray("contents");
  JsonObject content = contents.createNestedObject();
  JsonArray parts = content.createNestedArray("parts");
  JsonObject part = parts.createNestedObject();
  part["text"] = questionText;
  
  String requestBody;
  serializeJson(requestDoc, requestBody);
  
  Serial.printf("Request: %s\n", requestBody.c_str());
  
  // Send to Gemini
  String url = String(GEMINI_URL) + "?key=" + API_KEY_Gemini;
  
  Serial.printf("Gemini URL: %s\n", url.substring(0, 80).c_str());  // Print first 80 chars
  Serial.printf("API Key length: %d\n", String(API_KEY_Gemini).length());
  
  // Setup fresh SSL connection
  secureClient.stop();  // Close any existing connection
  delay(100);
  secureClient.setInsecure();
  
  if (!http.begin(secureClient, url)) {
    Serial.println("✗ Failed to begin Gemini HTTP connection");
    lastError = "Gemini HTTP begin failed";
    currentState = STATE_ERROR;
    return;
  }
  
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(30000);  // 30 second timeout
  
  Serial.println("Sending request to Gemini...");
  int httpCode = http.POST(requestBody);
  
  Serial.printf("Gemini HTTP Response Code: %d\n", httpCode);
  
  if (httpCode == 200) {
    String response = http.getString();
    Serial.printf("Response preview: %s\n", response.substring(0, 200).c_str());
    
    // Parse response
    StaticJsonDocument<JSON_BUFFER_SIZE> doc;
    DeserializationError error = deserializeJson(doc, response);
    
    if (!error) {
      answerText = doc["candidates"][0]["content"]["parts"][0]["text"].as<String>();
      
      if (answerText.length() > 0) {
        Serial.print("🤖 Gemini: \"");
        Serial.print(answerText);
        Serial.println("\"");
        String displayText = removeVietnameseTones(answerText);
        displayAnswer(displayText.c_str());
        setDisplayBusy(false);  // Resume display after Gemini complete
        currentState = STATE_PROCESSING_TTS;
      } else {
        lastError = "Empty response";
        currentState = STATE_ERROR;
      }
    } else {
      Serial.printf("✗ Gemini JSON parse error: %s\n", error.c_str());
      lastError = "JSON parse error";
      currentState = STATE_ERROR;
    }
  } else if (httpCode > 0) {
    String response = http.getString();
    Serial.printf("✗ Gemini API error %d: %s\n", httpCode, response.c_str());
    currentState = STATE_ERROR;
  }
  
  http.end();
}

// ============================================
// TEXT-TO-SPEECH (Google Translate)
// ============================================

void processTTS() {
  Serial.println("🔄 Converting text to speech...");
  
  // Truncate text if too long (Google TTS has limits)
  String ttsText = answerText;
  if (ttsText.length() > 200) {
    ttsText = ttsText.substring(0, 200);
    Serial.println("⚠️  Answer truncated for TTS (max 200 chars)");
  }
  
  // URL encode the text
  String encodedText = urlEncode(ttsText);
  
  // Build TTS URL
  String ttsUrl = String(GOOGLE_TTS_URL) + 
                  "?ie=UTF-8&q=" + encodedText +
                  "&tl=vi&client=tw-ob";
  
  // Download MP3
  secureClient.setInsecure();
  http.begin(secureClient, ttsUrl);
  int httpCode = http.GET();
  
  if (httpCode == 200) {
    // Delete old file
    if (SD.exists(AUDIO_OUTPUT_FILE)) {
      SD.remove(AUDIO_OUTPUT_FILE);
    }
    
    // Save to SD
    audioFile = SD.open(AUDIO_OUTPUT_FILE, FILE_WRITE);
    if (audioFile) {
      WiFiClient *stream = http.getStreamPtr();
      uint8_t buff[128];
      int len = 0;
      
      while (http.connected() && (len = stream->available())) {
        size_t size = stream->readBytes(buff, ((len > sizeof(buff)) ? sizeof(buff) : len));
        audioFile.write(buff, size);
      }
      
      audioFile.close();
      Serial.println("✓ Audio downloaded");
      currentState = STATE_SPEAKING;
    } else {
      lastError = "Cannot create output file";
      currentState = STATE_ERROR;
    }
  } else {
    lastError = "TTS error: " + String(httpCode);
    currentState = STATE_ERROR;
  }
  
  http.end();
}

// ============================================
// AUDIO PLAYBACK
// ============================================

void playSpeech() {
  Serial.println("🔊 Playing response...");
  
  // Setup I2S speaker (based on working example)
  Serial.println("Setting up speaker...");
  
  // Note: Speaker already setup in setupI2SSpeaker() at boot
  // But we need to reinstall to ensure clean state
  i2s_driver_uninstall(I2S_NUM_1);
  
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = 16000,  // Match TTS sample rate
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
  
  i2s_driver_install(I2S_NUM_1, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_1, &pin_config);
  i2s_zero_dma_buffer(I2S_NUM_1);
  
  // Open TTS file
  File ttsFile = SD.open(AUDIO_OUTPUT_FILE, FILE_READ);
  if (!ttsFile) {
    Serial.println("⚠️  Cannot open TTS file");
    i2s_driver_uninstall(I2S_NUM_1);
    currentState = STATE_LISTENING;
    displayListening();
    return;
  }
  
  Serial.printf("Playing %d bytes from %s\n", ttsFile.size(), AUDIO_OUTPUT_FILE);
  
  // Note: Google TTS returns MP3, not WAV
  // For now, send raw MP3 (won't decode properly without ESP8266Audio)
  // TODO: Add ESP8266Audio library for proper MP3 playback
  
  const size_t AUDIO_CHUNK = 1024;
  uint8_t buffer[AUDIO_CHUNK];
  size_t bytesWritten;
  uint32_t totalBytes = 0;
  
  while (ttsFile.available()) {
    size_t bytesRead = ttsFile.read(buffer, AUDIO_CHUNK);
    if (bytesRead > 0) {
      i2s_write(I2S_NUM_1, buffer, bytesRead, &bytesWritten, portMAX_DELAY);
      totalBytes += bytesWritten;
    }
  }
  
  ttsFile.close();
  i2s_driver_uninstall(I2S_NUM_1);  // Clean up
  
  Serial.printf("✓ Sent %d bytes to speaker\n", totalBytes);
  Serial.println("💡 Note: MP3 won't sound right without decoder");
  Serial.println("   Install ESP8266Audio library for proper playback");
  
  delay(500);
  Serial.println("\n🎙️  Listening...");
  currentState = STATE_LISTENING;
  displayListening();
}

// ============================================
// UTILITY FUNCTIONS
// ============================================

String urlEncode(String str) {
  String encoded = "";
  char c;
  char code0;
  char code1;
  
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == ' ') {
      encoded += '+';
    } else if (isalnum(c)) {
      encoded += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) {
        code0 = c - 10 + 'A';
      }
      encoded += '%';
      encoded += code0;
      encoded += code1;
    }
  }
  
  return encoded;
}

// ============================================
// END OF FILE
// ============================================
