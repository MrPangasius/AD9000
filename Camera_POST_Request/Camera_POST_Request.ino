#include <Wire.h>
#include <SoftwareSerial.h>

// Define pins for OV7670
#define VSYNC 3
#define HREF 4
#define PCLK 5
#define D0 6
#define D1 7
#define D2 8
#define D3 9
#define D4 10
#define D5 11
#define D6 12
#define D7 13

// Initialize ESP8266
SoftwareSerial espSerial(2, 3); // RX, TX

// Function prototypes
void connectToWiFi(const char* ssid, const char* password);
bool captureImage();
void sendImageToServer();

#define IMAGE_WIDTH  160  // QCIF resolution width
#define IMAGE_HEIGHT 120  // QCIF resolution height
#define BUFFER_SIZE  (IMAGE_WIDTH * IMAGE_HEIGHT) // Raw image buffer size

uint8_t imageBuffer[BUFFER_SIZE]; // Array to store raw image data

void setup() {
  Serial.begin(9600); // Serial for debugging
  Serial.println("Serial Has Begun \n\r");
  espSerial.begin(115200); // ESP8266 baud rate

  // Initialize OV7670
  Wire.begin();
  pinMode(VSYNC, INPUT);
  pinMode(HREF, INPUT);
  pinMode(PCLK, INPUT);
  for (int i = D0; i <= D7; i++) pinMode(i, INPUT);
  Serial.println("Pin Modes Defined \n\r");
  connectToWiFi("test", "12345678");
}

void loop() {
  // Capture image
  if (captureImage()) {
    sendImageToServer();
  }
  Serial.println('Picture Captured');
  delay(15000);
}

bool captureImage() {
  Serial.println("Starting image capture...");

  int bufferIndex = 0;

  // Wait for a VSYNC signal to start frame capture
  while (digitalRead(VSYNC) == LOW);
  while (digitalRead(VSYNC) == HIGH);

  // Capture the image row by row
  for (int y = 0; y < IMAGE_HEIGHT; y++) {
    while (digitalRead(HREF) == LOW); // Wait for a valid row

    for (int x = 0; x < IMAGE_WIDTH; x++) {
      while (digitalRead(PCLK) == HIGH); // Wait for PCLK low
      uint8_t pixelData = 0;

      // Read pixel data from D0-D7 pins
      for (int i = 0; i < 8; i++) {
        pixelData |= (digitalRead(D0 + i) << i);
      }

      // Store the pixel data in the buffer
      if (bufferIndex < BUFFER_SIZE) {
        imageBuffer[bufferIndex++] = pixelData;
      } else {
        Serial.println("Buffer overflow! Image too large.");
        return false;
      }

      while (digitalRead(PCLK) == LOW); // Wait for PCLK high
    }
  }

  Serial.println("Image capture completed.");
  return true;
}

void sendImageToServer() {
  // Send image data to server using ESP8266
  espSerial.println("AT+CIPSTART=\"TCP\",\"192.168.151.31\",5000");
  delay(2000);
  
  // Prepare HTTP POST request
  String httpRequest = "POST /upload HTTP/1.1\r\n";
  httpRequest += "Host: 192.168.151.31\r\n";
  httpRequest += "Content-Type: image/jpeg\r\n";
  httpRequest += "Content-Length: <IMAGE_SIZE>\r\n";
  httpRequest += "\r\n";
  httpRequest += "<IMAGE_DATA>";

  // Send HTTP request
  espSerial.println("AT+CIPSEND=" + String(httpRequest.length()));
  delay(1000);
  espSerial.print(httpRequest);
  delay(2000);

  // Check response
  while (espSerial.available()) {
    Serial.write(espSerial.read());
  }

  espSerial.println("AT+CIPCLOSE");
}

void connectToWiFi(const char* ssid, const char* password) {
  espSerial.println("AT");
  delay(1000);
  espSerial.println("AT+CWMODE=1");
  delay(1000);
  espSerial.println("AT+CWJAP=\"" + String(ssid) + "\",\"" + String(password) + "\"");
  delay(5000);
  Serial.println("Connected to WiFi.");
}
