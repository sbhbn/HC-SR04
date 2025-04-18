#define trigPin 12
#define echoPin 14
#define ledPin 13  // 車格被占用就亮燈

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266HTTPClient.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "55Y8817";
const char* password = "HN73108378";

ESP8266WebServer server(80);
long duration;
float distance;

void testdrawstyles() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(1);
  display.setCursor(5, 0);
  display.print("Hello OLED");
  display.setCursor(26, 40);
  display.print("MiroTek");
  display.display();
  delay(1000);
}

void setup() {
  Serial.begin(115200);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);

  WiFi.begin(ssid, password);
  Serial.print("正在連接 WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi 已連線");
  Serial.print("IP 地址：");
  Serial.println(WiFi.localIP());

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true); // 卡住
  }

  display.clearDisplay();
  testdrawstyles();

  // 處理 HTTP 網頁請求
  server.on("/", []() {
    long d = getDistance();
    String html = "<!DOCTYPE html><html><head><meta http-equiv='refresh' content='2'/>"
                "<meta charset='UTF-8'/>"
                "<style>"
                "body {"
                "  margin: 0;"
                "  padding: 0;"
                "  font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;"
                "  background: linear-gradient(135deg, #74ebd5 0%, #ACB6E5 100%);"
                "  display: flex;"
                "  align-items: center;"
                "  justify-content: center;"
                "  height: 100vh;"
                "  color: #333;"
                "}"
                ".card {"
                "  background-color: white;"
                "  padding: 30px 50px;"
                "  border-radius: 15px;"
                "  box-shadow: 0 8px 20px rgba(0,0,0,0.2);"
                "  text-align: center;"
                "}"
                "h1 {"
                "  font-size: 32px;"
                "  margin-bottom: 20px;"
                "}"
                "h2 {"
                "  font-size: 24px;"
                "  color: #4CAF50;"
                "  margin-top: 10px;"
                "}"
                "</style></head><body>"
                "<div class='card'>"
                "<h1>Car Slot Detecting System</h1>"
                "<h2>The current distance：" + String(d) + " cm</h2>"
                "</div>"
                "</body></html>";
    server.send(200, "text/html", html);
  });

  server.begin();
  Serial.println("Web Server 啟動完成");
  Serial.println(WiFi.localIP());

  // 呼叫 API
if (WiFi.status() == WL_CONNECTED) {
  WiFiClient client;        // 建立 WiFiClient 實例
  HTTPClient http;

  String url = "http://jsonplaceholder.typicode.com/posts";
  http.begin(client, url);  // 使用新版 API，需要傳入 client

  http.addHeader("Content-Type", "application/json");

  String jsonPayload = "{\"title\":\"foo\",\"body\":\"bar\",\"userId\":1}";
  int httpCode = http.POST(jsonPayload);

  if (httpCode > 0) {
    Serial.println("HTTP Response Code: " + String(httpCode));
    String payload = http.getString();
    Serial.println("Response Payload: " + payload);
  } else {
    Serial.println("Error on HTTP request");
  }

  http.end();
}
}

long getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration * 0.034) / 2;
  return distance;
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

 long duration = pulseIn(echoPin, HIGH, 30000);
if (duration == 0) {
  Serial.println("⚠️ 沒有接收到回波（timeout）");
} else {
  float distance = duration * 0.034 / 2;
  Serial.print("距離: ");
  Serial.print(distance);
  Serial.println(" cm");
}
  delay(500);

  server.handleClient(); // 處理瀏覽器請求
  delay(500);
}
