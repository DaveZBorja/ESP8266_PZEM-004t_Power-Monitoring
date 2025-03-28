#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>
#include <SoftwareSerial.h>
#include <PZEM004Tv30.h>



#if defined(ESP32)
    #error "Software Serial is not supported on the ESP32"
#endif

 #if !defined(PZEM_RX_PIN) && !defined(PZEM_TX_PIN)
#define PZEM_RX_PIN 12
#define PZEM_TX_PIN 13
#endif

// WiFi Credentials
const char* ssid = "MGT";
const char* password = "Algorithm@root";

// PZEM004T Serial pins for ESP8266
//#define PZEM_RX_PIN 12  // GPIO4 (D2)
//#define PZEM_TX_PIN 13  // GPIO5 (D1)

// Initialize SoftwareSerial for PZEM
SoftwareSerial pzemSerial(PZEM_RX_PIN, PZEM_TX_PIN);
PZEM004Tv30 pzem(pzemSerial);

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Global variables for sensor readings
float voltage = 0.0, current = 0.0, power = 0.0, energy = 0.0, frequency = 0.0, pf = 0.0;

// HTML Webpage
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>ESP8266 Power Monitor</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial, sans-serif; text-align: center; background-color: #f0f0f0; padding: 20px; }
    .container { max-width: 500px; margin: auto; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }
    h2 { color: #2c3e50; }
    .value { font-size: 24px; color: #2980b9; font-weight: bold; }
  </style>
  <script>
    function updateData() {
      fetch("/data").then(response => response.json()).then(data => {
        document.getElementById("voltage").innerHTML = data.voltage + " V";
        document.getElementById("current").innerHTML = data.current + " A";
        document.getElementById("power").innerHTML = data.power + " W";
        document.getElementById("energy").innerHTML = data.energy + " kWh";
        document.getElementById("frequency").innerHTML = data.frequency + " Hz";
        document.getElementById("pf").innerHTML = data.pf;
      });
    }
    setInterval(updateData, 2000);
    window.onload = updateData;
  </script>
</head>
<body>
  <div class="container">
    <h2>ESP8266 Power Monitor</h2>
    <p>Voltage: <span class="value" id="voltage">--</span></p>
    <p>Current: <span class="value" id="current">--</span></p>
    <p>Power: <span class="value" id="power">--</span></p>
    <p>Energy: <span class="value" id="energy">--</span></p>
    <p>Frequency: <span class="value" id="frequency">--</span></p>
    <p>Power Factor: <span class="value" id="pf">--</span></p>
  </div>
</body>
</html>
)rawliteral";

// Function to replace placeholders with actual sensor values
String processor(const String& var) {
  if (var == "VOLTAGE") return String(voltage, 1);
  if (var == "CURRENT") return String(current, 2);
  if (var == "POWER") return String(power, 1);
  if (var == "ENERGY") return String(energy, 3);
  if (var == "FREQUENCY") return String(frequency, 1);
  if (var == "PF") return String(pf, 2);
  return String();
}

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Web server routes
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = "{";
    json += "\"voltage\":\"" + String(voltage, 1) + "\",";
    json += "\"current\":\"" + String(current, 2) + "\",";
    json += "\"power\":\"" + String(power, 1) + "\",";
    json += "\"energy\":\"" + String(energy, 3) + "\",";
    json += "\"frequency\":\"" + String(frequency, 1) + "\",";
    json += "\"pf\":\"" + String(pf, 2) + "\"";
    json += "}";
    request->send(200, "application/json", json);
  });

  server.begin();
}

void loop() {
     Serial.print("Custom Address: ");
    Serial.println(pzem.readAddress(), HEX);

    // Read the data from the sensor
    voltage = pzem.voltage();
    current = pzem.current();
    power = pzem.power();
    energy = pzem.energy();
    frequency = pzem.frequency();
    pf = pzem.pf();

    // Check if the data is valid
    if (isnan(voltage)) {
        Serial.println("Error reading voltage");
    } else if (isnan(current)) {
        Serial.println("Error reading current");
    } else if (isnan(power)) {
        Serial.println("Error reading power");
    } else if (isnan(energy)) {
        Serial.println("Error reading energy");
    } else if (isnan(frequency)) {
        Serial.println("Error reading frequency");
    } else if (isnan(pf)) {
        Serial.println("Error reading power factor");
    } else {
        // Print the values to the Serial console
        Serial.print("Voltage: ");      Serial.print(voltage);      Serial.println(" V");
        Serial.print("Current: ");      Serial.print(current);      Serial.println(" A");
        Serial.print("Power: ");        Serial.print(power);        Serial.println(" W");
        Serial.print("Energy: ");       Serial.print(energy, 3);    Serial.println(" kWh");
        Serial.print("Frequency: ");    Serial.print(frequency, 1); Serial.println(" Hz");
        Serial.print("PF: ");           Serial.println(pf);
    }

    Serial.println();
    delay(2000);
}
