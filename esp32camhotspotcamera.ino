#include <WiFi.h>
#include "esp_camera.h"
#include <WebServer.h>

#define SSID "ESP32-CAM-HOTSPOT"
#define PWD  "12345678"
#define LED_GPIO 4

// AI-Thinker Camera Pins
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

WebServer server(80);
bool ledState = false;

void setupWiFi() {
  WiFi.softAP(SSID, PWD);
  Serial.print("Hotspot IP: ");
  Serial.println(WiFi.softAPIP());
}

void setupCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  config.frame_size = FRAMESIZE_SVGA;      // 800x600 (clearer)
  config.jpeg_quality = 8;                 // Better quality
  config.fb_count = 2;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed 0x%x", err);
    return;
  }
}

void handleRoot() {
  String html = R"rawliteral(
    <html>
      <head>
        <title>ESP32-CAM Security Cam</title>
        <style>
          body {
            background: #111;
            color: white;
            text-align: center;
            font-family: Arial;
          }
          img {
            width: 95%%;
            max-width: 480px;
            border-radius: 10px;
            box-shadow: 0 0 20px #000;
          }
          button {
            padding: 14px 30px;
            font-size: 18px;
            background: #00c853;
            color: white;
            border: none;
            border-radius: 8px;
            margin-top: 20px;
            cursor: pointer;
            transition: 0.2s;
          }
          button:hover {
            background: #009624;
          }
        </style>
      </head>
      <body>
        <h1>ESP32-CAM Live</h1>
        <img src="/stream"><br>
        <button onclick="toggleLED()">Toggle Light</button>
        <script>
          function toggleLED() {
            fetch('/led', { method: 'POST' });
          }
        </script>
      </body>
    </html>
  )rawliteral";
  server.send(200, "text/html", html);
}

void handleStream() {
  WiFiClient client = server.client();
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
  server.sendContent(response);

  while (1) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      continue;
    }
    response = "--frame\r\nContent-Type: image/jpeg\r\n\r\n";
    server.sendContent(response);
    server.sendContent((const char *)fb->buf, fb->len);
    server.sendContent("\r\n");
    esp_camera_fb_return(fb);

    if (!client.connected()) break;
  }
}

void handleLED() {
  ledState = !ledState;
  digitalWrite(LED_GPIO, ledState ? HIGH : LOW);
  Serial.println(ledState ? "LED ON" : "LED OFF");
  server.send(200, "text/plain", ledState ? "ON" : "OFF");
}

void setup() {
  Serial.begin(115200);
  setupWiFi();
  setupCamera();

  pinMode(LED_GPIO, OUTPUT);
  digitalWrite(LED_GPIO, LOW);

  server.on("/", handleRoot);
  server.on("/stream", HTTP_GET, handleStream);
  server.on("/led", HTTP_POST, handleLED);

  server.begin();
}

void loop() {
  server.handleClient();
}
