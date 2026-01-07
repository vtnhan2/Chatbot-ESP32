# 🤖 ESP32 AI Voice Chatbot

Complete voice-activated AI chatbot using Wit.ai STT, Gemini AI, and Google Translate TTS.

## ✨ Features

- 🎙️ **Always-listening mode** with Voice Activity Detection
- 🗣️ **Wit.ai Speech-to-Text** (Free, unlimited)
- 🤖 **Gemini AI** for intelligent responses
- 🔊 **Google Translate TTS** (Free, unlimited)
- ⚡ **240MHz CPU** overclock for maximum performance
- 💾 **SD Card** for audio buffering

## 📁 Files

- `chatbot.ino` - Main chatbot code
- `../secret.h` - API keys configuration

## 🔧 Hardware Requirements

| Component | GPIO Pins | Notes |
|-----------|-----------|-------|
| INMP441 Mic | 32, 25, 33 | I2S input |
| MAX98357 Speaker | 26, 27, 14 | I2S output |
| SD Card | 12, 13, 21, 22 | SPI |
| LED Status | 2 | Built-in LED |

## 🚀 Quick Start

### 1. Get API Keys

#### Wit.ai (STT)
1. Go to https://wit.ai
2. Login with Facebook/GitHub
3. Create new app
4. Copy **Server Access Token**

#### Gemini (AI)
1. Go to https://aistudio.google.com/app/apikey
2. Click "Get API Key"
3. Copy API key

### 2. Configure secret.h

```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* API_KEY_STT = "YOUR_WIT_AI_TOKEN";
const char* API_KEY_TTS = ""; // Not needed for Google Translate
const char* API_KEY_Gemini = "YOUR_GEMINI_KEY";
```

### 3. Upload Sketch

1. Open `chatbot.ino` in Arduino IDE
2. Select **ESP32 Dev Module**
3. Upload
4. Open Serial Monitor (115200 baud)

### 4. Test

1. Wait for "🎙️ Listening..."
2. Speak your question
3. Wait for AI response

## 📊 How It Works

```
🎙️ Listen (VAD)
    ↓
📝 Record to SD (WAV)
    ↓
☁️ Wit.ai STT → Text
    ↓
🤖 Gemini AI → Response
    ↓
🔊 Google TTS → Audio
    ↓
🔈 Play through speaker
    ↓
🔄 Back to listening
```

## ⚙️ Configuration

### Voice Activity Detection (VAD)

```cpp
#define VAD_THRESHOLD       300     // Voice detection sensitivity
#define VAD_SILENCE_MS      1500    // Silence duration = end
#define MAX_RECORD_TIME_MS  10000   // Max recording time
```

**Tuning:**
- Increase `VAD_THRESHOLD` if too sensitive (noisy environment)
- Decrease `VAD_THRESHOLD` if not detecting (quiet voice)

### CPU Performance

```cpp
setCpuFrequencyMhz(240);  // 240 MHz overclock
```

Benefits:
- Faster API processing
- Better audio streaming
- Lower latency

## 🎯 LED Status

| Blink Speed | State |
|-------------|-------|
| Slow (2s) | 🎙️ Listening |
| Fast (200ms) | 📝 Recording |
| Very fast (100ms) | ⏳ Processing |
| Medium (500ms) | 🔊 Speaking |
| Ultra fast (50ms) | ⚠️ Error |

## 🐛 Troubleshooting

### "No speech detected"
- Speak louder
- Lower `VAD_THRESHOLD`
- Check microphone connection

### "STT API error"
- Check Wit.ai API key
- Verify WiFi connection
- Check Serial Monitor for HTTP code

### "Gemini API error"
- Check Gemini API key
- Verify internet connection
- Check API quota

### No audio playback
⚠️ **Known limitation**: Google Translate TTS returns MP3 format, which ESP32 cannot decode natively.

**Solutions:**
1. Install MP3 decoder library (ESP8266Audio)
2. Use different TTS API (returns WAV)
3. Pre-convert MP3 → WAV

## 📝 API Usage & Costs

**Wit.ai STT:**
- ✅ FREE unlimited
- No credit card needed

**Google Translate TTS:**
- ✅ FREE unlimited
- Unofficial API (may have rate limits)

**Gemini:**
- ✅ FREE with Google One AI Premium
- Or free tier via AI Studio

**Total cost:** $0/month for normal usage! 🎉

## 🔒 Security

- `secret.h` is in `.gitignore`
- Never commit API keys
- Create`secret.h` from template

## 🎓 Advanced Features (TODO)

- [  ] MP3 decoder for TTS playback
- [ ] Display UI on ILI9225
- [ ] Wake word detection
- [ ] Multi-language support
- [ ] Conversation history
- [ ] Voice customization

## 📚 Dependencies

- WiFi.h (built-in)
- HTTPClient.h (built-in)
- ArduinoJson v6.x
- driver/i2s.h (built-in)
- SD.h (built-in)

## 🔗 API Documentation

- [Wit.ai API](https://wit.ai/docs/http/20220622)
- [Gemini API](https://ai.google.dev/tutorials/rest_quickstart)
- [Google Translate TTS](https://cloud.google.com/text-to-speech/docsMP3) (unofficial)

## 💡 Tips

- Use **good quality speaker** for best audio
- **Quiet environment** for better VAD
- Connect **GAIN pin to 5V** for max volume
- Monitor **Serial output** for debugging

## 🤝 Contributing

Feel free to improve:
- Add MP3 decoder support
- Implement display UI
- Add wake word detection
- Improve VAD algorithm

---

Made with ❤️ for ESP32
