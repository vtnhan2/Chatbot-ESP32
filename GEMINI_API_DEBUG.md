# Hướng Dẫn Kiểm Tra & Debug Gemini API

## 🔑 Bước 1: Kiểm Tra API Key

### 1.1. Xem API Key hiện tại
```bash
# Mở file secret.h
notepad d:\School\Iot\final-project\Chatbot-ESP32\source\secret.h
```

API key của bạn có dạng: `AIzaSy...` (39 ký tự)

### 1.2. Test API Key với Browser
Mở link này trong browser:
```
https://generativelanguage.googleapis.com/v1beta/models?key=API_KEY_CUA_BAN
```

**Thay `API_KEY_CUA_BAN`** bằng key thực tế từ `secret.h`

**Kết quả mong đợi:** Danh sách models available
**Nếu lỗi:** API key không hợp lệ hoặc hết quota

---

## 📋 Bước 2: Liệt Kê Models Available

### 2.1. Dùng curl (Terminal/PowerShell)
```powershell
curl "https://generativelanguage.googleapis.com/v1beta/models?key=YOUR_API_KEY"
```

### 2.2. Kết quả mẫu
```json
{
  "models": [
    {
      "name": "models/gemini-1.0-pro",
      "displayName": "Gemini 1.0 Pro",
      "supportedGenerationMethods": ["generateContent", ...]
    },
    {
      "name": "models/gemini-1.5-flash-latest",
      ...
    }
  ]
}
```

**Quan trọng:** Lưu lại tên model chính xác từ field `"name"`

---

## 🧪 Bước 3: Test Gemini API với curl

### 3.1. Test với model `gemini-1.0-pro`
```powershell
curl -X POST `
  -H "Content-Type: application/json" `
  -d '{\"contents\":[{\"parts\":[{\"text\":\"Hello\"}]}]}' `
  "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.0-pro:generateContent?key=YOUR_API_KEY"
```

### 3.2. Kết quả thành công (HTTP 200):
```json
{
  "candidates": [
    {
      "content": {
        "parts": [
          {
            "text": "Hello! How can I help you today?"
          }
        ]
      }
    }
  ]
}
```

### 3.3. Lỗi 404 - Model Not Found:
```json
{
  "error": {
    "code": 404,
    "message": "models/xxx is not found for API version v1beta..."
  }
}
```

**→ Model name sai hoặc không available cho API key của bạn**

---

## 🔍 Bước 4: Thử Các Model Khác Nhau

Test các model sau (từ phổ biến nhất):

### 4.1. Gemini 1.0 Pro (Stable)
```
https://generativelanguage.googleapis.com/v1beta/models/gemini-1.0-pro:generateContent
```

### 4.2. Gemini 1.5 Flash (Mới hơn, nhanh hơn)
```
https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent
```

### 4.3. Gemini 1.5 Flash Latest
```
https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash-latest:generateContent
```

### 4.4. Gemini 1.5 Pro
```
https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-pro:generateContent
```

**Thử từng cái một cho đến khi tìm được model working!**

---

## 🌐 Bước 5: Kiểm Tra Trên Google AI Studio

### 5.1. Truy cập AI Studio
1. Mở: https://aistudio.google.com/
2. Đăng nhập bằng tài khoản đã tạo API key
3. Click vào **"Get API Key"** ở menu bên trái

### 5.2. Kiểm tra Models
Trong AI Studio, bạn sẽ thấy:
- Models mà API key có quyền access
- Quota & limits
- Usage history

### 5.3. Test trực tiếp
- Click vào model bất kỳ (vd: Gemini 1.0 Pro)
- Trong chat interface, hỏi "Hello"
- Nếu có response → Model working
- Click "Get code" → Copy API endpoint

---

## 🆕 Bước 6: Tạo API Key Mới (Nếu Cần)

### 6.1. Tại AI Studio
1. https://aistudio.google.com/apikey
2. Click **"Create API key"**
3. Chọn Google Cloud project (hoặc tạo mới)
4. Copy API key mới

### 6.2. Enable APIs cần thiết
Vào Google Cloud Console:
```
https://console.cloud.google.com/apis/library
```

Tìm và enable:
- ✅ **Generative Language API**
- ✅ **Vertex AI API** (optional)

---

## 🔧 Bước 7: Update Code ESP32

### 7.1. Sau khi tìm được model working

Giả sử model `gemini-1.5-flash-latest` working, update code:

```cpp
// Trong chatbot.ino, line 76
#define GEMINI_URL "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash-latest:generateContent"
```

### 7.2. Test ngay
1. Upload code mới
2. Nói vào mic
3. Xem Serial Monitor → Gemini HTTP Response Code
4. **Nếu 200** → Success! 🎉
5. **Nếu 404** → Thử model khác

---

## 📊 Troubleshooting Common Issues

### Issue 1: 403 Forbidden
**Nguyên nhân:** 
- API key không hợp lệ
- Vượt quota miễn phí
- Billing chưa enable (nếu cần)

**Giải pháp:** Tạo API key mới

---

### Issue 2: 429 Too Many Requests
**Nguyên nhân:** Rate limit hit

**Giải pháp:** 
- Đợi 1 phút
- Giảm số request (add delay)

---

### Issue 3: 404 Model Not Found
**Nguyên nhân:** 
- Model name sai
- Model không available cho region/account

**Giải pháp:** 
- List models với `/v1beta/models?key=...`
- Dùng model name CHÍNH XÁC từ list

---

### Issue 4: Empty Response
**Nguyên nhân:** Request format sai

**Giải pháp:** 
Check request body:
```json
{
  "contents": [
    {
      "parts": [
        {"text": "your question here"}
      ]
    }
  ]
}
```

---

## ✅ Checklist Debug

- [ ] API key valid? (test với `/v1beta/models`)
- [ ] Models list available?
- [ ] Test model working với curl?
- [ ] Update `GEMINI_URL` in code?
- [ ] Upload & test ESP32?
- [ ] Serial Monitor shows 200 response?

---

## 💡 Quick Test Script

Save as `test_gemini.ps1`:

```powershell
# Replace with your actual API key
$API_KEY = "AIzaSy..."

# Step 1: List models
Write-Host "=== Listing Available Models ===" -ForegroundColor Green
curl "https://generativelanguage.googleapis.com/v1beta/models?key=$API_KEY"

# Step 2: Test gemini-1.0-pro
Write-Host "`n=== Testing gemini-1.0-pro ===" -ForegroundColor Green
curl -X POST `
  -H "Content-Type: application/json" `
  -d '{\"contents\":[{\"parts\":[{\"text\":\"Say hi\"}]}]}' `
  "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.0-pro:generateContent?key=$API_KEY"

# Step 3: Test gemini-1.5-flash
Write-Host "`n=== Testing gemini-1.5-flash ===" -ForegroundColor Green
curl -X POST `
  -H "Content-Type: application/json" `
  -d '{\"contents\":[{\"parts\":[{\"text\":\"Say hi\"}]}]}' `
  "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=$API_KEY"
```

Run:
```powershell
powershell -File test_gemini.ps1
```

---

## 📞 Next Steps

1. **Run quick test script** để tìm model working
2. **Update GEMINI_URL** in `chatbot.ino`
3. **Upload & test** ESP32
4. **Report results** - paste Serial Monitor output

Good luck! 🚀
