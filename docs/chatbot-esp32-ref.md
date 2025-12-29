<img src="https://r2cdn.perplexity.ai/pplx-full-logo-primary-dark%402x.png" style="height:64px;margin-right:32px"/>

# Tôi muốn vẽ luồng hoạt động của 1 mô hình embedded gọi cloud thì nên dùng sơ đồ gì

Nên dùng sơ đồ **activity/sequence + architecture** (hoặc data-flow) tùy mức chi tiết, nhưng cho “luồng hoạt động” giữa embedded và cloud thì UML Activity hoặc Sequence là hợp lý nhất.[^1_1][^1_2][^1_3]

## Khi nào dùng Activity diagram

Activity diagram mô tả workflow / business flow, rất hợp để vẽ “thiết bị làm gì – gửi gì – cloud xử lý ra sao” theo luồng.[^1_4][^1_1]

- Vẽ các bước: Sensor sample → MCU xử lý → đóng gói MQTT/HTTP → gửi lên cloud → cloud validate → lưu DB → phản hồi xuống.
- Có thể thêm nhánh if/else (mất mạng, lỗi auth) và các luồng song song (nhiều sensor, nhiều service cloud).

Nếu bạn muốn trình bày flow logic tổng quát, không cần timing chi tiết, hãy chọn **Activity diagram**.

## Khi nào dùng Sequence diagram

Sequence diagram tập trung vào tương tác theo thời gian giữa các “đối tượng”: thiết bị, gateway, MQTT broker, API, DB, web app,…[^1_5][^1_2][^1_3]

- Trục dọc là thời gian, trục ngang là các entity: Embedded node, Cloud API, DB, User app.
- Thể hiện rõ thứ tự: Device connect → publish → cloud subscribe/ xử lý → ghi DB → push notification → device nhận command.

Nếu bạn muốn nhấn mạnh thứ tự message/request–response, hãy dùng **Sequence diagram**.

## Khi nào thêm Architecture / Deployment diagram

Để cho người xem hiểu bức tranh tổng quan (thiết bị ở đâu, cloud gồm những service nào), nên có thêm 1 sơ đồ kiến trúc.[^1_6][^1_3][^1_5]

- Vẽ các khối: Sensor/MCU → Gateway (nếu có) → Internet → Cloud (API, message queue, DB, dashboard).
- Dùng như “overview”, sau đó đi sâu logic bằng Activity/Sequence.


## Data Flow Diagram (DFD) có phù hợp không?

DFD tập trung vào luồng **dữ liệu** giữa các process, data store, external entity.[^1_7][^1_8]

- Hợp khi bạn muốn mô tả dữ liệu (measurement, config, log) đi qua những process nào, lưu ở đâu, chứ không tập trung vào control flow chi tiết.
- Có thể dùng nếu bạn đang viết tài liệu an toàn/threat model (DFD cho thiết bị và cloud).


## Gợi ý thực tế cho mô hình embedded–cloud

- 1 sơ đồ kiến trúc (architecture/deployment) để show MCU, network, cloud services.
- 1 sơ đồ Activity **hoặc** Sequence cho luồng hoạt động chính (measure → send → process → respond).
- Nếu cần security/spec compliance, thêm 1 DFD cho luồng dữ liệu.

Nếu bạn mô tả sơ qua mô hình (MCU nào, giao thức gì: MQTT/HTTP, có gateway hay không), có thể gợi ý luôn một bộ ký hiệu cụ thể để bạn vẽ trong draw.io / diagrams.net.
<span style="display:none">[^1_10][^1_11][^1_12][^1_13][^1_14][^1_15][^1_16][^1_17][^1_18][^1_19][^1_20][^1_9]</span>

<div align="center">⁂</div>

[^1_1]: https://dev.to/edgaras/introduction-to-uml-for-system-and-process-design-3l1m

[^1_2]: https://www.figma.com/resource-library/what-is-a-uml-diagram/

[^1_3]: https://www.geeksforgeeks.org/system-design/how-to-draw-architecture-diagrams/

[^1_4]: https://ideascale.com/vi/viblog/so-do-cac-loai-uml-khac-nhau/

[^1_5]: https://vfunction.com/blog/architecture-diagram-guide/

[^1_6]: https://www.exoway.io/blog/cloud-architecture-diagram

[^1_7]: https://www.paloaltonetworks.com/cyberpedia/data-flow-diagram

[^1_8]: https://cloud.gov/docs/compliance/diagrams/

[^1_9]: https://dejazzer.com/coen4720/lectures/lec08_flowcharts.pdf

[^1_10]: https://www.dejazzer.com/coen4720/lectures/lec08_flowcharts.pdf

[^1_11]: https://creately.com/guides/types-of-flowcharts/

[^1_12]: https://www.youtube.com/watch?v=MFEXDujq6cs

[^1_13]: https://stackoverflow.com/questions/3632859/differences-between-dfd-data-flow-diagram-and-activity-diagram

[^1_14]: https://www.reddit.com/r/embedded/comments/fm5wka/code_flow_structures_for_embedded_systems/

[^1_15]: https://www.diagrams-ai.com/blog/modeling-iot-cloud-workflows-with-ai-activity-diagrams/

[^1_16]: https://creately.com/blog/diagrams/uml-diagram-types-examples/

[^1_17]: https://www.mhtechin.com/support/the-critical-role-of-flow-diagrams-in-software-development-at-mhtechin/

[^1_18]: https://www.acceldata.io/blog/visualizing-data-success-importance-of-data-architecture-diagrams

[^1_19]: https://www.geeksforgeeks.org/system-design/unified-modeling-language-uml-introduction/

[^1_20]: https://www.youtube.com/watch?v=EdnksgiUaMg


---

# folder chứa những hình ảnh diagram nên đặt tên là gì trong github

Nên đặt tên folder ngắn, toàn chữ thường, có gạch nối và mang tính mô tả rõ ràng, ví dụ: **`docs/diagrams`** hoặc **`assets/diagrams`**.[^2_1][^2_2][^2_3]

## Một số tên gợi ý

- `docs/diagrams` nếu diagram chủ yếu phục vụ tài liệu kỹ thuật, README, wiki.[^2_2][^2_1]
- `docs/architecture-diagrams` nếu chủ yếu là kiến trúc hệ thống / embedded–cloud.[^2_1][^2_2]
- `assets/diagrams` nếu bạn dùng hình trong nhiều nơi (docs, wiki, issue template, v.v.).[^2_4][^2_2]
- `design/uml-diagrams` nếu repo có nhiều thứ liên quan thiết kế (spec, ADR, UML, v.v.).[^2_5][^2_1]


## Nguyên tắc đặt tên

- Dùng chữ thường, không dấu, không khoảng trắng; ưu tiên `lowercase-with-hyphens` (ví dụ: `architecture-diagrams`).[^2_6][^2_3][^2_1]
- Đặt folder ở chỗ dễ tìm (thường là `docs/…` hoặc `assets/…`) và nhắc đến nó trong README để người khác biết nơi xem diagram.[^2_4][^2_5][^2_1]

Nếu repo của bạn đang có `docs/` rồi thì lựa chọn gọn gàng, chuẩn nhất thường là: `docs/architecture-diagrams`.
<span style="display:none">[^2_10][^2_11][^2_12][^2_13][^2_14][^2_15][^2_16][^2_17][^2_18][^2_19][^2_20][^2_7][^2_8][^2_9]</span>

<div align="center">⁂</div>

[^2_1]: https://github.com/kriasoft/Folder-Structure-Conventions

[^2_2]: https://github.com/orgs/community/discussions/168684

[^2_3]: https://stackoverflow.com/questions/11947587/is-there-a-naming-convention-for-git-repositories

[^2_4]: https://github.com/orgs/community/discussions/173482

[^2_5]: https://stackoverflow.com/questions/76754563/where-to-place-summary-uml-diagrams-in-a-github-repository

[^2_6]: https://worldbank.github.io/template/docs/folders-and-naming.html

[^2_7]: https://github.com/stillwwater/UnityStyleGuide

[^2_8]: https://researchdata.se/en/manage-data/organize-data/folder-structure-file-names-and-versioning

[^2_9]: https://www.suitefiles.com/guide/the-guide-to-folder-structures-best-practices-for-professional-service-firms-and-more/

[^2_10]: https://github.com/ahmedkhaleel2004/gitdiagram

[^2_11]: https://bimarchive.scottishfuturestrust.org.uk/level2/stage/1/task/47/file-naming-convention

[^2_12]: https://docs.github.com/en/enterprise-cloud@latest/contributing/writing-for-github-docs/creating-diagrams-for-github-docs

[^2_13]: https://dev.to/jps27cse/github-branching-name-best-practices-49ei

[^2_14]: https://constructionpublicinfo.ua.edu/wp-content/uploads/Design-Guidelines/Section-I/10-File-and-Folder-Naming-Conventions.pdf

[^2_15]: https://www.youtube.com/watch?v=R-sJvXu-rHI

[^2_16]: https://github.com/oppia/oppia/wiki/The-File-Naming-Convention-and-Directory-Structure

[^2_17]: http://www.cccarchitecture.org/file-naming-protocols

[^2_18]: https://www.reddit.com/r/architecture/comments/2vcnfo/office_file_naming_conventions/

[^2_19]: https://docs.github.com/en/contributing/writing-for-github-docs/creating-diagrams-for-github-docs

[^2_20]: https://github.com/GoldenbergLab/naming-and-documentation-conventions


---

# tôi có thể triển khai mô hình ASR và TTS trên esp32 dev kit ko ?

Có, nhưng chỉ ở mức **giản lược** (keyword spotting + TTS đơn giản) hoặc phải offload phần nặng sang server/PC; không thể chạy full ASR/TTS kiểu cloud model trên ESP32 devkit.[^3_1][^3_2][^3_3]

## TTS trên ESP32

- Có thể làm TTS offline với các thư viện như Talkie, PicoTTS,… cho tiếng “robot”, từ vựng hạn chế, bitrate thấp; phù hợp cảnh báo, prompt ngắn.[^3_4][^3_5][^3_6][^3_7]
- Có thể dùng giải pháp “hybrid”: text gửi lên server (Python + gTTS / cloud API) sinh WAV/MP3 rồi stream về ESP32 qua I2S để phát; ESP32 chỉ làm phần I/O audio.[^3_8][^3_9][^3_10]


## ASR trên ESP32

- Có thể làm **keyword spotting / command recognition** (wake word, vài chục lệnh) bằng Edge Impulse hoặc framework ESP-SR trên ESP32/ESP32-S3, model rất nhỏ, MFCC + CNN/RNN.[^3_11][^3_12][^3_3][^3_1]
- Không khả thi để chạy **large-vocabulary continuous ASR** (nhận dạng câu dài, ngôn ngữ tự do) full trên ESP32 dev kit do hạn chế RAM, flash, compute; thường phải stream audio lên server để decode.[^3_2][^3_3][^3_1]


## Mô hình khả thi cho project

- On-device:
    - ASR: wake word + vài lệnh cố định (ví dụ “bật đèn”, “tắt quạt”) bằng ESP-SR hoặc Edge Impulse.
    - TTS: một số câu cố định (pre-generated) hoặc TTS LPC đơn giản để trả lời ngắn.
- Cloud-assisted:
    - ESP32 thu audio (I2S mic) → gửi chunk lên server.
    - Server chạy ASR/TTS (Whisper, Vosk, Coqui TTS, v.v.) → trả text/audio về ESP32.

Nếu bạn mô tả cụ thể devkit (ESP32 hay ESP32-S3, RAM/flash, có PSRAM không) và yêu cầu (ngôn ngữ, online/offline, độ dài câu), có thể cùng phác kiến trúc chi tiết và ước lượng tài nguyên.
<span style="display:none">[^3_13][^3_14][^3_15][^3_16][^3_17][^3_18][^3_19][^3_20]</span>

<div align="center">⁂</div>

[^3_1]: https://dius.com.au/machine-learning-on-the-edge-speech-command-recognition/

[^3_2]: https://www.cnx-software.com/2023/07/17/espressif-esp-sr-enables-on-device-speech-recognition-framework-on-esp32-s3-and-esp32-wisocs/

[^3_3]: https://github.com/espressif/esp-sr

[^3_4]: https://circuitdigest.com/microcontroller-projects/esp32-text-to-speech-offline-system

[^3_5]: https://github.com/bharanidharanrangaraj/converting_text_to_speech_offline_using_esp32

[^3_6]: https://maker.pro/arduino/projects/offline-esp32-text-to-speech-build-a-voice-enabled-device

[^3_7]: https://esp32.com/viewtopic.php?f=2\&t=38317

[^3_8]: https://www.elektormagazine.com/labs/esp-32-speech-function

[^3_9]: https://www.hackster.io/circuitsmiles/offline-esp32-voice-assistant-gemini-ai-i2s-audio-2a2eaa

[^3_10]: https://www.reddit.com/r/esp32/comments/1gvbkgz/diy_project_building_a_realtime_ai_voice/

[^3_11]: https://wiki.seeedstudio.com/xiao_esp32s3_keyword_spotting/

[^3_12]: https://www.hackster.io/amy/keyword-spotting-on-esp32-s3-with-inmp441-and-max7219-c3de33

[^3_13]: https://www.youtube.com/shorts/WGwBqDHqIN4

[^3_14]: https://www.reddit.com/r/esp32/comments/1o9b2yf/text_to_speech_for_offline_use/

[^3_15]: https://www.facebook.com/circuitdigest/posts/build-an-esp32-text-to-speech-offline-systemturn-your-esp32-into-a-fully-offline/1152812950352754/

[^3_16]: https://easyelecmodule.com/build-esp32-s3-voice-robot-from-0-to-1/

[^3_17]: https://forum.arduino.cc/t/can-i-use-esp8266-or-esp32-to-do-a-tts-conversion-and-reproduce-it-without-internet/1075968

[^3_18]: https://www.reddit.com/r/esp32/comments/swp0pc/esp32_voice_recognition_edge_impulse/

[^3_19]: https://forum.edgeimpulse.com/t/keyword-spotting-on-esp32/4185

[^3_20]: https://www.youtube.com/watch?v=iKaxI2F0Mik


---

# vẽ 1 architecture diagram cho chatbot dùng esp32, có mic, có speaker và gọi API online để xử lí dữ liệu, tại local thì có wakeup word và TTS phản hồi với wakeup word

Mô hình này triển khai được và kiến trúc khá giống các project “ESP32 voice assistant: local wake word + cloud LLM/STT/TTS”. Dưới đây là một **architecture diagram dạng text** để bạn dễ vẽ lại trong draw.io / diagrams.net.[^4_1][^4_2][^4_3][^4_4]

## Các khối chính

- **Hardware (ESP32 + audio)**
    - ESP32 DevKit (nên là ESP32-S3 + PSRAM nếu có).
    - Mic I2S / ADC.
    - Speaker + Class-D Amp (I2S DAC / external codec).
- **On-device AI \& logic**
    - Wake word engine (TinyML / ESP-SR / Edge Impulse) chạy liên tục, luôn nghe mic.[^4_2][^4_5][^4_6][^4_7]
    - TTS cục bộ chỉ để phát wakeup response (“Vâng, em nghe đây”, beep, v.v.).
    - State machine: IDLE → WAKE_DETECTED → RECORDING/STREAMING → WAITING_RESPONSE → PLAYING_RESPONSE.
- **Cloud side**
    - API Gateway / Backend (HTTP/WebSocket/MQTT).
    - Cloud ASR (Whisper API hoặc dịch vụ STT khác).[^4_8][^4_4][^4_1]
    - Chatbot / LLM / NLP engine (OpenAI, Gemini, v.v.).[^4_9][^4_10][^4_3][^4_4][^4_1]
    - Cloud TTS (tùy bạn có dùng text-only hay trả về cả audio).


## Architecture diagram (dạng khối)

Bạn có thể vẽ sơ đồ với các block và mũi tên như sau:

1. **User**
    - Nói: “Hey Bot … [câu hỏi]”.
2. **Mic + ESP32 (Wake word + TTS local)**
    - Mic I2S → **ESP32 Audio Frontend** (A/D, VAD, buffer).
    - **Wake Word Detector**:
        - Nếu *không* phát hiện wake word → vẫn ở trạng thái IDLE.
        - Nếu **phát hiện wake word** →
            - ESP32 phát **local TTS** / beep qua **Speaker** để báo đã sẵn sàng.
            - Chuyển sang trạng thái “Listening Command”.
3. **ESP32 → Cloud (Online processing)**
    - ESP32 bắt đầu **record** audio sau wake word (hoặc stream realtime qua chunk).
    - Gửi dữ liệu audio lên **Cloud API** qua WiFi (HTTP(S) streaming / WebSocket / MQTT).[^4_3][^4_4][^4_9][^4_2]
4. **Cloud pipeline**
    - **ASR Service**: Audio → Text input (tiếng Việt/Anh,…).
    - **Chatbot / LLM Service**: Text input + context → Text response.
    - (Option A) **Cloud TTS**: Text response → Audio (WAV/MP3/Opus).
    - Trả dữ liệu về **ESP32** (text +/hoặc audio).
5. **Cloud → ESP32 (Response)**
    - Nếu nhận **audio**: ESP32 stream trực tiếp ra **Speaker** qua I2S DAC.
    - Nếu nhận **text**:
        - ESP32 dùng **local TTS đơn giản** để đọc (nếu đủ tài nguyên và chấp nhận chất lượng).
    - Kết thúc, ESP32 quay về trạng thái IDLE và bật lại wake word engine.

## Cách đặt tên khối trong diagram

Khi vẽ architecture diagram, bạn có thể chia làm 3 cột:

- **Left (User \& I/O)**
    - User
    - Mic
    - Speaker
- **Middle (Edge Device - ESP32)**
    - Audio Frontend (I2S / ADC)
    - Wake Word Engine
    - Local TTS (Wake response)
    - Network Client (HTTP/WebSocket/MQTT)
    - State Machine / App Logic
- **Right (Cloud Services)**
    - API Gateway
    - ASR Service
    - Chatbot / LLM Service
    - Cloud TTS Service
    - Optional: Logging / DB / User profile

Nếu bạn nói rõ muốn vẽ bằng kiểu UML component, deployment hay block diagram, có thể viết luôn mô tả chi tiết để bạn copy vào tool vẽ.
<span style="display:none">[^4_11][^4_12][^4_13][^4_14][^4_15][^4_16][^4_17][^4_18][^4_19][^4_20]</span>

<div align="center">⁂</div>

[^4_1]: https://blog.siqma.com/diy-smart-chatbot-lamp-esp32-openai-voice-assistant/

[^4_2]: https://easyelecmodule.com/build-esp32-s3-voice-robot-from-0-to-1/

[^4_3]: https://www.hackster.io/ElectroScopeArchive/esp32-ai-voice-assistant-with-mcp-integration-2598c8

[^4_4]: https://www.emqx.com/en/blog/esp32-and-mcp-over-mqtt-4

[^4_5]: https://www.home-assistant.io/blog/2024/02/21/voice-chapter-6/

[^4_6]: https://circuitdigest.com/microcontrollers-projects/esp32-offline-voice-recognition-using-edge-impulse

[^4_7]: https://dev.to/zediot/esp32-s3-tensorflow-lite-micro-a-practical-guide-to-local-wake-word-edge-ai-inference-5540

[^4_8]: https://www.ijraset.com/research-paper/offline-virtual-voice-assistant-using-ai-and-nlp

[^4_9]: https://www.reddit.com/r/esp32/comments/1gvbkgz/diy_project_building_a_realtime_ai_voice/

[^4_10]: https://tunghuynh.net/programer/2591/demo-ai-voice-assistant-using-gemini-on-esp32-s3-jc3248w535c/

[^4_11]: https://www.hackster.io/circuitsmiles/offline-esp32-voice-assistant-gemini-ai-i2s-audio-2a2eaa

[^4_12]: https://projecthub.arduino.cc/circuitsmiles/offline-esp32-voice-assistant-with-gemini-ai-8b3da9

[^4_13]: https://www.electromaker.io/project/view/esp32-ai-voice-assistant-with-mcp-integration

[^4_14]: https://www.ijarsct.co.in/Paper25577.pdf

[^4_15]: https://www.ijsat.org/papers/2025/4/8555.pdf

[^4_16]: https://dev.to/m-a-h-b-u-b/95-accurate-wake-word-detection-low-power-cnn-mfcc-guide-3377

[^4_17]: https://www.home-assistant.io/voice_control/s3_box_voice_assistant/

[^4_18]: https://devinhedge.com/2025/05/02/components-of-wake-words-in-wake-word-engine-design/

[^4_19]: https://towardsdatascience.com/build-a-locally-running-voice-assistant-2f2ead904fe9/

[^4_20]: https://cs.uwaterloo.ca/~jimmylin/publications/Tang_etal_NLP-OSS2020.pdf


---

# tạo hình ảnh

Media generated: 'Sơ đồ kiến trúc chatbot ESP32 với wake word local và xử lý ASR/LLM/TTS trên cloud.'

---

## Sơ đồ kiến trúc chatbot ESP32 đã tạo

![ESP32 Voice Chatbot Architecture: Local Wake Word Detection + Cloud ASR/LLM/TTS Processing](https://user-gen-media-assets.s3.amazonaws.com/seedream_images/aa16adff-0bd6-4d05-a5dc-0c117cefb5f0.png)

ESP32 Voice Chatbot Architecture: Local Wake Word Detection + Cloud ASR/LLM/TTS Processing

Sơ đồ trên thể hiện kiến trúc đầy đủ của hệ thống chatbot voice trên ESP32 với các phần chính:

**Phía trái (User \& I/O):**

- Người dùng nói vào microphone
- Loa phát âm thanh phản hồi

**Phía giữa (ESP32 - Edge Device):**

- **Audio Frontend**: Xử lý A/D, D/A, VAD (phát hiện có tiếng nói)
- **Wake Word Engine**: Luôn lắng nghe cục bộ, chạy model TinyML/ESP-SR, không cần gửi lên cloud
- **Local TTS Response**: Phát lời xác nhận ngắn ("Vâng em nghe", beep, v.v.) khi phát hiện từ wakeup
- **Network Client**: Quản lý WiFi/MQTT/HTTP
- **State Machine**: Điều phối luồng (IDLE → WAKE_DETECTED → RECORDING → WAITING → PLAYING → IDLE)

**Phía phải (Cloud Services):**

- **API Gateway**: Tiếp nhận request từ ESP32
- **ASR Service**: Chuyển audio → text (Whisper, tùy API bạn dùng)
- **Chatbot / LLM**: Xử lý text với OpenAI, Gemini, v.v.
- **Cloud TTS**: Chuyển text phản hồi → audio hoặc text nguyên

**Luồng dữ liệu:**

- Audio từ mic → ESP32 liên tục
- Wake word phát hiện → phát local TTS acknowledgment
- Sau wakeup → stream/gửi audio lên cloud
- Cloud xử lý ASR → LLM → TTS → trả audio/text về ESP32
- ESP32 phát ra speaker

Bạn có thể dùng hình này để vẽ lại trong **draw.io / Figma / diagrams.net** hoặc lưu trực tiếp vào repo GitHub dưới folder `docs/architecture-diagrams/`.

---

# PS D:\School\Iot\final-project\Chatbot-ESP32> git checkout -b dev/design-system

fatal: cannot lock ref 'refs/heads/dev/design-system': 'refs/heads/dev' exists; cannot create 'refs/heads/dev/design-system'

Lỗi này xảy ra vì **Git không cho phép tạo branch con trong một branch đã tồn tại** (Git coi `dev` là branch thay vì folder).(see the generated image above) Để khắc phục:

## Cách 1: Đổi tên branch (nhanh nhất)

Thay vì `dev/design-system`, dùng tên khác không có `/`:

```bash
git checkout -b dev-design-system
```

hoặc:

```bash
git checkout -b feature/design-system
```


## Cách 2: Xóa branch `dev` cũ rồi tạo lại (nếu không dùng)

Kiểm tra branch hiện tại:

```bash
git branch -a
```

Nếu `dev` không dùng nữa, xóa nó:

```bash
git branch -d dev
git checkout -b dev/design-system
```


## Cách 3: Dùng branch đầy đủ với nhánh con (khuyến nghị)

Nếu bạn muốn giữ cấu trúc `dev/...`, xóa branch `dev` flat trước:

```bash
git branch -D dev
git checkout -b dev/design-system
```

Sau này tất cả feature mới đều dùng `dev/...`, `feature/...`, hoặc `fix/...` thay vì branch flat `dev`.

## Gợi ý naming convention cho branch

Với IoT chatbot project, nên dùng:

- `main` - production
- `develop` hoặc `dev` - integration branch (không tạo sub-branch con trong nó)
- `feature/wake-word-training`
- `feature/cloud-api-integration`
- `feature/audio-streaming`
- `fix/esp32-memory-leak`
- `docs/architecture-diagrams`

Thay vì nested `dev/...`, hãy dùng **prefix rõ ràng** như `feature/...`, `docs/...`, `fix/...`.

**Lệnh bạn nên chạy:**

```bash
git checkout -b feature/design-system
```

hoặc:

```bash
git checkout -b docs/design-system
```

(tùy nội dung của branch)

---

# Thiết kế activity diagram cho :chatbot dùng esp32, có mic, có speaker và gọi API online để xử lí dữ liệu, tại local thì có wakeup word và TTS phản hồi với wakeup word

Activity diagram cho chatbot ESP32 với local wake word + cloud processing nên có các state chính: **IDLE → LISTENING → RECORDING → PROCESSING → RESPONDING → IDLE**.[^8_1][^8_2][^8_3]

## Luồng chính (Main Flow)

```
[Start] 
↓
[IDLE State - Always Listening]
  ↓ (Continuously)
[Wake Word Engine Processing Mic Input]
  ├─ [NO Wake Word] ───────┐
  │                        │
  └─ [Wake Word Detected] ─┼─ Loop back to IDLE
                           ↓
[Play Local TTS Acknowledgment] ("Vâng em nghe...")
                           ↓
[RECORDING State - VAD Active]
  ↓ (Timeout / Silence Detected)
[Stream Audio to Cloud API]
                           ↓
[CLOUD PROCESSING: ASR → LLM → TTS]
  ├─ [Success - Receive Response Audio/Text]
  │    ↓
  │ [PLAY Response via Speaker]
  │    ↓
  └─ [Error / Timeout / No Response]
       ↓
[Error Handling: Play Local TTS Error Message]
       ↓
[Return to IDLE State]
```


## Chi tiết các Decision Node

### 1. Wake Word Detection

```
[Wake Word Engine]
  ├─ [Wake Word Detected?] ── NO ──→ [IDLE/Continue Listening]
  └─ YES ───────────────────────→ [Local TTS: "Em nghe đây"]
```


### 2. Recording Logic

```
[Start Recording]
  ├─ [VAD Active?] ── NO ──→ [Wait Silence / Timeout 5s]
  └─ YES ───────────────→ [Buffer Audio Chunks]
                           ↓
[End Recording Condition?]
  ├─ [Silence > 2s] ──→ [Send to Cloud]
  ├─ [Max Duration 10s] ─→ [Send to Cloud]
  └─ [User Stop?] ─────→ [Cancel → IDLE]
```


### 3. Cloud Response Handling

```
[Receive Cloud Response]
  ├─ [HTTP 200 + Audio Data?] ──→ [Stream Audio → Speaker]
  ├─ [HTTP 200 + Text Only?] ──→ [Local TTS Text Response]
  ├─ [Timeout / Network Error] ─→ [Local TTS: "Mất kết nối"]
  └─ [API Error] ───────────────→ [Local TTS: "Lỗi xử lý"]
```


## Swimlanes (Partition) gợi ý khi vẽ

```
+--------------------+--------------------+--------------------+
|   ESP32 Device     |   Network          |   Cloud Services   |
+--------------------+--------------------+--------------------+
| [Always Listening] |                    |                    |
|                    |                    |                    |
| [Wake Detected ✓]  |                    |                    |
| [Local TTS beep]   |                    |                    |
|                    | [POST Audio] ────→ | [ASR Processing]   |
| [VAD Recording]    |                    | [LLM Processing]   |
|                    | ←─── [Response]    | [TTS Processing]   |
| [Play Response]    |                    |                    |
+--------------------+--------------------+--------------------+
```


## Tool vẽ khuyến nghị

- **draw.io / diagrams.net**: Free, export PNG/SVG/PDF
- **PlantUML** (text-based, tích hợp GitHub):

```plantuml
@startuml
start
:IDLE - Always Listening;
repeat
  :Wake Word Engine;
  if (Wake Word?) then (yes)
    :Play Local TTS Ack;
    :Start VAD Recording;
    stop repeat
  endif
repeat while () is (No)
:Stream Audio → Cloud;
:Wait Response;
if (Success?) then (yes)
  :Play Cloud Response;
else (error)
  :Play Local Error TTS;
endif
:Return IDLE;
stop
@enduml
```


## Lưu ý khi implement

- **Timeout handling** rất quan trọng: 5s recording max, 10s cloud response timeout
- **Local TTS** chỉ cần 3-5 câu cố định: "Em nghe", "Mất kết nối", "Lỗi xử lý", "Đang xử lý"
- **State persistence**: ESP32 crash/reboot → tự động về IDLE state

Bạn có thể copy text flow trên vào **draw.io** hoặc **PlantUML** để vẽ nhanh.
<span style="display:none">[^8_10][^8_11][^8_12][^8_13][^8_14][^8_15][^8_16][^8_17][^8_18][^8_19][^8_20][^8_4][^8_5][^8_6][^8_7][^8_8][^8_9]</span>

<div align="center">⁂</div>

[^8_1]: https://gist.github.com/EverythingSmartHome/055fbdde31a607ef9d695d5cac780e94?permalink_comment_id=5051993

[^8_2]: https://chat.visual-paradigm.com/diagram/uml-activity-diagram/

[^8_3]: https://agilemodeling.com/style/activitydiagram.htm

[^8_4]: https://github.com/justin23456543/ESP32-Based-Voice-Assistant-with-Perplexity-AI

[^8_5]: https://community.home-assistant.io/t/my-esp32-based-voice-assistant-with-wake-word/697153

[^8_6]: https://www.hackster.io/ElectroScopeArchive/offline-esp32-voice-recognition-with-edge-impulse-fc93c9

[^8_7]: https://www.circuitschools.com/building-a-gemini-ai-voice-assistant-with-esp32-and-oled-display/

[^8_8]: https://m.media-amazon.com/images/G/01/vii/VII_Architecture_Series_Whitepapers-Voice_Input_Data_Handling_on_Multi-Agent_Devices.pdf

[^8_9]: https://www.facebook.com/groups/874687145941483/posts/9205304179546363/

[^8_10]: https://www.studocu.vn/vn/document/dai-hoc-kinh-te-quoc-dan/ngon-ngu-mo-hinh-hoa-thong-nhat/activity-diagram-tutorial/98400534

[^8_11]: https://malquepub.com/index.php/multiscience/article/view/164/156

[^8_12]: https://www.kincony.com/forum/showthread.php?tid=6904

[^8_13]: https://www.reddit.com/r/softwarearchitecture/comments/1oethu9/uml_diagramsactivity_diagram_explanation/

[^8_14]: https://huggingface.co/learn/audio-course/en/chapter7/voice-assistant

[^8_15]: https://www.youtube.com/watch?v=aPP2alIU7Gg

[^8_16]: https://www.home-assistant.io/blog/2025/06/25/voice-chapter-10/

[^8_17]: https://tristam.ie/2024/1026/

[^8_18]: https://edrawmax.wondershare.com/uml/uml-activity-diagram.html

[^8_19]: https://www.sciencedirect.com/science/article/pii/S0952197624011564

[^8_20]: https://www.facebook.com/groups/HomeAssistant/posts/3933842606887080/


---

# Thiết kế activity diagram cho :chatbot dùng esp32, có mic, có speaker và gọi API online để xử lí dữ liệu, tại local thì có wakeup word và TTS phản hồi với wakeup word. Hệ thống có thể trò chuyện thì còn có thể thực hiện những yêu cầu đơn giản như bật tắt đèn/quạt qua GPIO, có 3 chế độ hoạt động: 1. Có kết nối wifi tới cloud-> full chức năng, 2. ko có kết nối wifi -> giao tiếp cơ bản bằng wakeup word <--> TTS local + điều khiển GPIO cơ bản, 3. chế độ kết nối wifi (chuyển bằng nhấn nút) -> esp32 phát sóng bluetooth/wifi và user sẽ dùng điện thoại để thiết lập name và Password wifi cho thiết bị

Media generated: 'Activity Diagram ESP32 Chatbot: 3 Chế Độ (Full Cloud, Offline Local, WiFi Setup)'

