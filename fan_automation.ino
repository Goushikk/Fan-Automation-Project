#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <IRrecv.h>
#include <IRutils.h> // For useful IR functions like dumpInfo

// --- Wi-Fi Configuration ---
const char* ssid = "YOUR_WIFI_SSID";         // REPLACE with your WiFi network name
const char* password = "YOUR_WIFI_PASSWORD"; // REPLACE with your WiFi password

// --- Pin Definitions ---
const int RECV_PIN = D2; // IR Receiver data pin connected to NodeMCU D2 (GPIO4)
const int RELAY_PIN = D1; // Relay control pin connected to NodeMCU D1 (GPIO5)

// --- IR Receiver Setup ---
IRrecv irrecv(RECV_PIN);
decode_results results; // Stores the decoded IR signal

// --- Relay State ---
bool fanState = false; // true = ON, false = OFF

// --- Web Server Setup ---
AsyncWebServer server(80); // Create AsyncWebServer object on port 80

// --- Function to toggle the relay ---
void toggleFan() {
  fanState = !fanState; // Invert the state
  if (fanState) {
    digitalWrite(RELAY_PIN, LOW); // Most 5V relay modules are active LOW
    Serial.println("Fan ON");
  } else {
    digitalWrite(RELAY_PIN, HIGH); // Turn off
    Serial.println("Fan OFF");
  }
}

// --- HTML for the Web Page ---
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 2.5rem;}
    p {font-size: 1.5rem;}
    .button {background-color: #4CAF50; border: none; color: white; padding: 15px 32px; text-align: center; text-decoration: none; display: inline-block; font-size: 24px; margin: 4px 2px; cursor: pointer; border-radius: 8px;}
    .button.off {background-color: #f44336;}
  </style>
</head>
<body>
  <h2>Retro Fan Control</h2>
  <p>Fan Status: <span id="fanStatus">%STATUS%</span></p>
  <p>
    <button class="button" onclick="toggleFan()">Toggle Fan</button>
  </p>
  <script>
    function toggleFan() {
      var xhr = new XMLHttpRequest();
      xhr.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("fanStatus").innerHTML = xhr.responseText;
        }
      };
      xhr.open("GET", "/toggle", true);
      xhr.send();
    }

    // Function to get initial status or update periodically
    function getFanStatus() {
      var xhr = new XMLHttpRequest();
      xhr.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("fanStatus").innerHTML = xhr.responseText;
        }
      };
      xhr.open("GET", "/status", true);
      xhr.send();
    }

    window.onload = getFanStatus; // Get status when page loads
    setInterval(getFanStatus, 5000); // Update status every 5 seconds
  </script>
</body>
</html>
)rawliteral";

// --- Function to process placeholders in HTML (e.g., %STATUS%) ---
String processor(const String& var){
  if(var == "STATUS"){
    if(fanState){
      return "ON";
    }
    else{
      return "OFF";
    }
  }
  return String();
}

void setup() {
  Serial.begin(115200);
  delay(100);

  // --- Pin Setup ---
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Ensure fan is OFF initially (active LOW relay)

  // --- Wi-Fi Connection ---
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // --- Start IR Receiver ---
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("IR receiver enabled. Point your remote here.");

  // --- Web Server Routes ---
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request){
    toggleFan();
    request->send(200, "text/plain", fanState ? "ON" : "OFF");
  });

  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", fanState ? "ON" : "OFF");
  });

  server.begin(); // Start the web server
}

void loop() {
  // --- IR Remote Decoding ---
  if (irrecv.decode(&results)) {
    Serial.print("Received IR code: ");
    serialPrintUint64(results.value, HEX); // Print the raw hex code
    Serial.print(" Protocol: ");
    Serial.println(IRremoteESP8266::get  (results.protocol)); // Print the protocol (NEC, SONY, etc.)

    // --- YOUR CUSTOM IR CODE HERE ---
    // You'll need to identify the exact code from your remote's power button.
    // Use the Serial Monitor to find this code.
    // Example: If your power button gives '0xFFA25D' (often for toggle) or '0xXXXXXXXX' (specific code)
    
    // Example: Assuming your remote sends 0xFFA25D for "Power" button (NEC protocol example)
    if (results.value == 0xFFA25D || results.value == 0xFFFFFFFF) { // 0xFFFFFFFF often means "repeat" or "toggle"
        toggleFan();
    }
    
    // IMPORTANT: After processing, resume receiving
    irrecv.resume(); 
  }
}

// Helper function to print uint64_t in HEX (IR codes can be long)
// This is needed because Serial.print doesn't directly support uint64_t by default
void serialPrintUint64(uint64_t data, int base) {
  if (data == 0) {
    Serial.print("0");
    return;
  }
  char buf[20]; // Max 64-bit number in base 2 is 64 chars, plus null terminator
  int i = 0;
  while (data > 0) {
    int rem = data % base;
    data /= base;
    if (rem < 10) {
      buf[i++] = rem + '0';
    } else {
      buf[i++] = rem + 'A' - 10;
    }
  }
  for (int j = i - 1; j >= 0; j--) {
    Serial.print(buf[j]);
  }
}
