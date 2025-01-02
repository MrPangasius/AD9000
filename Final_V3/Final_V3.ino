// Include necessary libraries
#include <Wire.h>
#include <SoftwareSerial.h>

// Define OV7670 pins
#define OV7670_XCLK 13
#define OV7670_SIOD 20
#define OV7670_SIOC 21
#define OV7670_RESET 8
#define OV7670_PWDN 7
#define OV7670_VSYNC 2
#define OV7670_HREF 3
#define OV7670_PCLK 12
#define OV7670_D0 22
#define OV7670_D1 23
#define OV7670_D2 24
#define OV7670_D3 25
#define OV7670_D4 26
#define OV7670_D5 27
#define OV7670_D6 28
#define OV7670_D7 29

// ESP8266 Serial communication
SoftwareSerial esp8266(32, 33); // RX, TX

unsigned long lastCaptureTime = 0; // Track last capture time
const unsigned long captureInterval = 15000; // 15 seconds

void setup() {
  // Serial setup
  Serial.begin(115200);
  esp8266.begin(115200);

  // OV7670 pin setup
  pinMode(OV7670_XCLK, OUTPUT);
  pinMode(OV7670_SIOD, OUTPUT);
  pinMode(OV7670_SIOC, OUTPUT);
  pinMode(OV7670_RESET, OUTPUT);
  pinMode(OV7670_PWDN, OUTPUT);
  pinMode(OV7670_VSYNC, INPUT);
  pinMode(OV7670_HREF, INPUT);
  pinMode(OV7670_PCLK, INPUT);

  for (int i = OV7670_D0; i <= OV7670_D7; i++) {
    pinMode(i, INPUT);
  }

  // Initialize OV7670
  initCamera();

  // Initialize ESP8266 Wi-Fi
  setupWiFi();
}

void loop() {
  unsigned long currentTime = millis();

  // Capture image every 15 seconds
  if (currentTime - lastCaptureTime >= captureInterval) {
    lastCaptureTime = currentTime;
    captureImage();
  }
}

void initCamera() {
  digitalWrite(OV7670_RESET, HIGH);
  digitalWrite(OV7670_PWDN, LOW);
  // Configure camera registers (this will depend on your specific OV7670)
  Serial.println("Camera initialized.");
}

void setupWiFi() {
  esp8266.println("AT+RST");
  delay(2000);
  esp8266.println("AT+CWMODE=1"); // Set to station mode
  delay(1000);
  esp8266.println("AT+CWJAP=\"Duckie\",\"12345678\"");
  delay(5000);
  esp8266.println("AT+CIPMUX=0");
  delay(1000);
  Serial.println("Wi-Fi setup complete.");
}

void captureImage() {
  Serial.println("Capturing image...");

  // Wait for VSYNC signal
  while (digitalRead(OV7670_VSYNC) == LOW);
  while (digitalRead(OV7670_VSYNC) == HIGH);

  // Start HTTP POST request to the server
  esp8266.println("AT+CIPSTART=\"TCP\",\"192.168.149.31\",5000");
  delay(2000);
  if (esp8266.find("OK")) {
    Serial.println("Connected to server.");

    esp8266.println("AT+CIPSEND=1000"); // Adjust the size accordingly
    delay(100);
    if (esp8266.find(">")) {
      // Send HTTP POST header
      esp8266.print("POST /upload HTTP/1.1\r\n");
      esp8266.print("Host: 192.168.149.31:5000\r\n");
      esp8266.print("Content-Type: application/octet-stream\r\n");
      esp8266.print("Content-Length: ");
      esp8266.print(76800); // Approximate size for QVGA raw data (320x240)
      esp8266.print("\r\n\r\n");

      // Capture image data and send
      for (int row = 0; row < 240; row++) {
        for (int col = 0; col < 320; col++) {
          while (digitalRead(OV7670_PCLK) == LOW);
          while (digitalRead(OV7670_PCLK) == HIGH);

          uint8_t pixelData = 0;
          for (int i = 0; i < 8; i++) {
            pixelData |= (digitalRead(OV7670_D0 + i) << i);
          }
          esp8266.write(pixelData);
        }
      }

      esp8266.println();
      Serial.println("Image sent.");
    } else {
      Serial.println("Failed to send data.");
    }

    esp8266.println("AT+CIPCLOSE");
  } else {
    Serial.println("Connection failed.");
  }
}
