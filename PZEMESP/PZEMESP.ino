#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SoftwareSerial.h>
#include <PZEM004Tv30.h>

// Define PZEM Pins
#define PZEM_RX_PIN 12  // Connect to TX on PZEM
#define PZEM_TX_PIN 13  // Connect to RX on PZEM

// Initialize SoftwareSerial for PZEM
SoftwareSerial pzemSWSerial(PZEM_RX_PIN, PZEM_TX_PIN);
PZEM004Tv30 pzem(pzemSWSerial);

// Create AsyncWebServer on port 80
AsyncWebServer server(80);

// Function to get sensor readings as JSON
String getPzemData() {
    float voltage = pzem.voltage();
    float current = pzem.current();
    float power = pzem.power();
    float energy = pzem.energy();
    float frequency = pzem.frequency();
    float pf = pzem.pf();

    if (isnan(voltage) || isnan(current) || isnan(power) || isnan(energy) || isnan(frequency) || isnan(pf)) {
        return "{\"error\": \"Failed to read data\"}";
    }

    String json = "{";
    json += "\"voltage\":" + String(voltage, 2) + ",";
    json += "\"current\":" + String(current, 2) + ",";
    json += "\"power\":" + String(power, 2) + ",";
    json += "\"energy\":" + String(energy, 3) + ",";
    json += "\"frequency\":" + String(frequency, 1) + ",";
    json += "\"pf\":" + String(pf, 2);
    json += "}";

    return json;
}

void setup() {
    Serial.begin(115200);

    // Start Access Point
    WiFi.softAP("PZEM_Server", "12345678");  // Change SSID & Password if needed
    Serial.println("Access Point Started!");
    Serial.print("AP IP Address: ");
    Serial.println(WiFi.softAPIP());

    // Define API endpoint for sensor data
    server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "application/json", getPzemData());
    });

    // Serve HTML page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        String html = R"rawliteral(
        <!DOCTYPE html>
        <html>
        <head>
            <title>PZEM-004T Web Server</title>
            <meta name="viewport" content="width=device-width, initial-scale=1">
            <style>
                body { font-family: Arial, sans-serif; text-align: center; }
                .card { background: #f8f9fa; padding: 20px; margin: 10px; border-radius: 10px; }
                .value { font-size: 24px; font-weight: bold; }
            </style>
            <script>
                function updateData() {
                    fetch('/data')
                    .then(response => response.json())
                    .then(data => {
                        if (data.error) {
                            document.getElementById("error").innerText = data.error;
                        } else {
                            document.getElementById("voltage").innerText = data.voltage + " V";
                            document.getElementById("current").innerText = data.current + " A";
                            document.getElementById("power").innerText = data.power + " W";
                            document.getElementById("energy").innerText = data.energy + " kWh";
                            document.getElementById("frequency").innerText = data.frequency + " Hz";
                            document.getElementById("pf").innerText = data.pf;
                        }
                    })
                    .catch(error => console.log("Error:", error));
                }
                setInterval(updateData, 1000);
            </script>
        </head>
        <body>
            <h1>PZEM-004T Web Server</h1>
            <div class="card">
                <p>Voltage: <span id="voltage" class="value">--</span></p>
                <p>Current: <span id="current" class="value">--</span></p>
                <p>Power: <span id="power" class="value">--</span></p>
                <p>Energy: <span id="energy" class="value">--</span></p>
                <p>Frequency: <span id="frequency" class="value">--</span></p>
                <p>Power Factor: <span id="pf" class="value">--</span></p>
            </div>
            <p id="error" style="color: red;"></p>
        </body>
        </html>
        )rawliteral";
        request->send(200, "text/html", html);
    });

    // Start server
    server.begin();
}

void loop() {
    // No need for code in loop() as AsyncWebServer handles requests
     delay(10); // Allows background WiFi tasks to run
}
