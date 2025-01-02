#include <WiFiClient.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#define TRIG_PIN 24
#define ECHO_PIN 25
#define BEEP_PIN 3

SoftwareSerial ESP_Serial(31,30);

#define ssid "Duckie";
#define password "12345678";


// Cam Pins
#define OV7670_XCLK 3
#define OV7670_SIOD 4
#define OV7670_SIOC 5
#define OV7670_RESET 6
#define OV7670_PWDN 7
#define OV7670_VSYNC 8
#define OV7670_HREF 9
#define OV7670_PCLK 10
#define OV7670_D0 22
#define OV7670_D1 23
#define OV7670_D2 24
#define OV7670_D3 25
#define OV7670_D4 26
#define OV7670_D5 27
#define OV7670_D6 28
#define OV7670_D7 29



const char* server = "192.168.149.31"; // Flask server IP
const int port = 5000;
const char* endpoint = "/upload";

void connecttowifi();
void setupCam();


void setup()
{
  // Make Pin_modes
  Serial.begin(115200);
  ESP_Serial(115200);
  Serial.println('Connecting to WiFi')

  // OV7670 pin setup
  pinMode(OV7670_XCLK, OUTPUT);
  pinMode(OV7670_SIOD, OUTPUT);
  pinMode(OV7670_SIOC, OUTPUT);
  pinMode(OV7670_RESET, OUTPUT);
  pinMode(OV7670_PWDN, OUTPUT);
  pinMode(OV7670_VSYNC, INPUT);
  pinMode(OV7670_HREF, INPUT);
  pinMode(OV7670_PCLK, INPUT);

  for (int i = OV7670_D0; i <= OV7670_D7; i++)
  {
    pinMode(i, INPUT);
  }

  connecttowifi();
  setupCam();

void loop() {
if (ESP_Serial.available()) {
    String command = ESP_Serial.readStringUntil('\n');
    if (command == "CAPTURE") {
      captureImage();
    }
  }
}
}
/*
// Helper function to calculate Content-Length
int calculateContentLength(uint8_t* frame, size_t frameSize) {
  int length = 0;
  length += String("------ArduinoBoundary\r\n").length();
  length += String("Content-Disposition: form-data; name=\"file\"; filename=\"image.jpg\"\r\n").length();
  length += String("Content-Type: image/jpeg\r\n\r\n").length();
  length += frameSize;
  length += String("\r\n------ArduinoBoundary--\r\n").length();
  return length;
}

// Helper function to send the image data
void sendImage(uint8_t* frame, size_t frameSize) {
  client.println("------ArduinoBoundary");
  client.println("Content-Disposition: form-data; name=\"file\"; filename=\"image.jpg\"");
  client.println("Content-Type: image/jpeg");
  client.println();
  client.write(frame, frameSize);
  client.println();
  client.println("------ArduinoBoundary--");
}
*/
void connectToWiFi() {
  espSerial.println("AT");
  delay(1000);
  espSerial.println("AT+CWMODE=1");
  delay(1000);
  espSerial.println("AT+CWJAP=\"" + String(ssid) + "\",\"" + String(password) + "\"");
  delay(5000);
  Serial.println("Connected to WiFi.");
}
