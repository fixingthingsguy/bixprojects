#include "secrets.h"
#include <WiFiClientSecure.h>
#include <MQTT.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include "esp_camera.h"
#include <HTTPClient.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#define AWS_IOT_PUBLISH_TOPIC   "$aws/things/iot2/shadow/update"
#define AWS_IOT_SUBSCRIBE_TOPIC "$aws/things/iot2/shadow/update"
#include "stream.h"
const char* S3_BUCKET_NAME = "xxxxxxxxx";
const int pirPin = 13;
const int ledPin = 2 ;
volatile bool pirTriggered = false;
unsigned long lastPirTrigger = 0;
const unsigned long pirCooldownTime = 100;
const unsigned long ledOnTime = 2000;
bool ledState = false;
unsigned long ledOffTime = 0;

void IRAM_ATTR pirISR() {
 Serial.println("PIR triggered");
 // if (millis() - lastPirTrigger > pirCooldownTime) {
    pirTriggered = true;
  //  lastPirTrigger = millis();
 // }
}
void stopCameraServer();
void startCameraServer();
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client;

void connectAWS() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  client.begin(AWS_IOT_ENDPOINT, 8883, net);
  client.onMessage(messageHandler);

  Serial.print("Connecting to AWS IoT");
  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }

  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
  Serial.println("AWS IoT Connected and Subscribed!");
}

void publishMessage() {
  client.publish(AWS_IOT_PUBLISH_TOPIC, "{\"state\":{\"reported\":{\"value\":73}}}");
  Serial.println("Sent to MQTT with payload");
}

void messageHandler(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  int value = doc["state"]["reported"]["value"];
  if (value == 72) {
    Serial.println("Received value 72, capturing image");
    captureImage();
  } else {
    Serial.println("Received different value");
  }
}

void setup() {
  Serial.begin(115200);
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  //pinMode(pirPin, INPUT);
  pinMode(pirPin, INPUT_PULLDOWN);
  pinMode(ledPin, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(pirPin), pirISR, RISING  );
  connectAWS();

  

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  config.frame_size = FRAMESIZE_240X240;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return;
  } else {
    Serial.println("Camera initialized successfully");
  }

  sensor_t* s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_240X240);

 // startCameraServer();
}

void captureImage() {
  Serial.println("Preparing to capture image...");
  delay(1000);  // Delay to ensure camera is ready

  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  Serial.println("Image captured successfully");

  if (uploadImageToS3(fb->buf, fb->len)) {
    Serial.println("Image uploaded successfully");
  } else {
    Serial.println("Image upload failed");
  }

  esp_camera_fb_return(fb);
}

bool uploadImageToS3(const uint8_t* image_data, size_t image_length) {
  //  WiFi.disconnect(true);
  //  Serial.println("WiFi disconnected");
  //
  //  delay(100);
  //
  //  WiFi.mode(WIFI_STA);
  //  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  //
  //  Serial.println("Reconnecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nReconnected to Wi-Fi");
  connectAWS();

  HTTPClient http;
  String url = "https://" + String(S3_BUCKET_NAME) + ".s3.us-east-1.amazonaws.com/upload.jpg";
  http.begin(url.c_str());
  http.addHeader("Content-Type", "image/jpeg");

  int httpResponseCode = http.PUT((uint8_t*)image_data, image_length);

  if (httpResponseCode == 200) {
    Serial.println("Image uploaded successfully");
    publishMessage();
    http.end();
    return true;
  } else {
    Serial.print("Error uploading image: ");
    Serial.println(httpResponseCode);
    http.end();
    return false;
  }
}

void loop() {
  client.loop();
  int sensorValue = digitalRead(pirPin);
  Serial.println(sensorValue); delay(1000);
  if (pirTriggered) {Serial.println(pirTriggered); 
    pirTriggered = false;
    ledState = true;
    digitalWrite(ledPin, HIGH);
    ledOffTime = millis() + ledOnTime;
    isStreaming = false;  Serial.print("                     Is_stream= ");Serial.println(isStreaming);
    
    captureImage();isStreaming = true;Serial.print("                     Is_stream= ");Serial.println(isStreaming);
    Serial.println("PIR sensor triggered, LED turned on");
  }

  if (ledState && millis() >= ledOffTime) {
    ledState = false;
    digitalWrite(ledPin, LOW);
    Serial.println("LED turned off");
  }
Serial.print("Pin 13 val="); Serial.println(digitalRead(pirPin));
Serial.print("Is_stream= ");Serial.println(isStreaming);
  delay(1000);
  
}
