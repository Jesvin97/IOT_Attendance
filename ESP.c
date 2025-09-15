#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN D1
#define SS_PIN  D2

MFRC522 mfrc522(SS_PIN, RST_PIN);

const char* ssid = "Aswin";        
const char* password = "012345678";
const char* host = "script.google.com";

// CORRECT: Your URL is right, but store only the path part
String scriptPath = "/macros/s/AKfycbzOLiwfNcfg8XhAATN-0B1dxcOKtzz_mjFOtoP3j0Dizwkg2NJOqGwq1tJShtxWOh_4oA/exec";

WiFiClientSecure client;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); 
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  Serial.println("IP address: " + WiFi.localIP().toString());
  
  client.setInsecure(); // Skip SSL certificate check
  
  Serial.println("RFID Attendance System Ready");
  Serial.println("Present your card to mark attendance...");
}

void loop() {
  // Check for new RFID card
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
  // Read UID
  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  
  Serial.println("____________________________________________________");
  Serial.println("Card Detected: " + uid);
  
  // Send to Google Apps Script
  sendAttendance(uid);
  
  // Halt the card to prevent multiple reads
  mfrc522.PICC_HaltA();
  
  delay(2000); // Prevent duplicate reads
}

void sendAttendance(String uid) {
  Serial.println("Connecting to Google Apps Script...");
  
  if (client.connect(host, 443)) {
    Serial.println("Connected to server ");
    
    // Construct the full request path with UID parameter
    String fullPath = scriptPath + "?uid=" + uid;
    
    // Send HTTP GET request
    client.print(String("GET ") + fullPath + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "User-Agent: ESP8266-RFID-Attendance\r\n" +
                 "Connection: close\r\n\r\n");
    
    Serial.println("Request sent. Waiting for response...");
    
    // Wait for response
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 10000) { // 10 second timeout
        Serial.println("Timeout waiting for response ");
        client.stop();
        return;
      }
      delay(100);
    }
    
    // Read response headers
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        break; // End of headers
      }
      // Optional: Print headers for debugging
      // Serial.println("Header: " + line);
    }
    
    // Read response body
    String response = "";
    while (client.available()) {
      response += client.readString();
    }
    
    client.stop();
    
    // Process and display response
    if (response.length() > 0) {
      Serial.println("Response received:");
      Serial.println("─────────────────────────");
      Serial.println(response);
      Serial.println("─────────────────────────");
      
      // Parse response for user feedback
      if (response.indexOf("Time In marked") > -1) {
        Serial.println("CHECK-IN SUCCESSFUL");
      } else if (response.indexOf("Time Out updated") > -1) {
        Serial.println(" CHECK-OUT SUCCESSFUL");
      } else if (response.indexOf("New Time In marked") > -1) {
        Serial.println("RETURN FROM BREAK");
      } else if (response.indexOf("Error: Unauthorized UID") > -1) {
        Serial.println("UNAUTHORIZED CARD");
      } else if (response.indexOf("Error") > -1) {
        Serial.println("ERROR OCCURRED");
      } else {
        Serial.println("ATTENDANCE RECORDED");
      }
    } else {
      Serial.println("Empty response received");
    }
    
  } else {
    Serial.println("Connection to server failed");
    
    // Optional: Try to reconnect WiFi if connection fails repeatedly
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Reconnecting to WiFi...");
      WiFi.reconnect();
    }
  }
}

// Optional: Function to test with a specific UID
void testAttendance() {
  Serial.println("Testing with sample UID...");
  sendAttendance("TEST123");
}

// Optional: Function to check WiFi status
void checkWiFiStatus() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi Status: Connected ");
    Serial.println("IP: " + WiFi.localIP().toString());
  } else {
    Serial.println("WiFi Status: Disconnected ");
    Serial.println("Attempting to reconnect...");
    WiFi.reconnect();
  }
}
