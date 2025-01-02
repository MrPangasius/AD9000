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
#define esp8266 Serial1
/*
// ESP8266 Serial communication
SoftwareSerial esp8266(32, 33); // RX, TX
*/
void writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(0x42 >> 1); // OV7670 I2C address
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

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
  captureImage();
  delay(15000);
  Serial.println("Done");
}

void initCamera() {
  digitalWrite(OV7670_RESET, LOW);
  delay(10);
  digitalWrite(OV7670_RESET, HIGH);
  digitalWrite(OV7670_PWDN, LOW);

  Wire.begin();

  // Write camera settings for full-color images (RGB565)
  writeRegister(0x12, 0x14); // RGB mode, RGB565 format
  writeRegister(0x40, 0xD0); // RGB565 mode
  writeRegister(0x11, 0x01); // Set clock prescaler
  writeRegister(0x6B, 0x4A); // PLL control
  writeRegister(0x3A, 0x04); // Output format control
  writeRegister(0x3E, 0x19); // PCLK settings
  writeRegister(0x70, 0x3A); // Scaling settings
  writeRegister(0x71, 0x35); // Scaling settings
  writeRegister(0x72, 0x11); // Scaling settings
  writeRegister(0x73, 0xF0); // Scaling settings
  writeRegister(0xA2, 0x02); // PCLK settings
  writeRegister(0x15, 0x20); // Output drive strength

  Serial.println("Camera configured for full-color images.");
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
      esp8266.print(320 * 240 * 2); // Approximate size for QVGA raw data (320x240)
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