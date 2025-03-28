#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <PZEM004Tv30.h>

// WiFi Credentials
const char* ssid = "MGT";
const char* password = "Algorithm@root";


// Initialize Web Server on port 80
ESP8266WebServer server(80);

// Use Hardware Serial for PZEM
#define PZEM_SERIAL Serial1
PZEM004Tv30 pzem(PZEM_SERIAL);

void handleRoot() {
    server.send(200, "text/plain", "PZEM-004T Web Server is running!");
}

void handleData() {
    float voltage = pzem.voltage();
    float current = pzem.current();
    float power = pzem.power();
    float energy = pzem.energy();
    float frequency = pzem.frequency();
    float pf = pzem.pf();

    Serial.println("Reading PZEM Data...");
    Serial.print("Voltage: "); Serial.println(voltage);
    Serial.print("Current: "); Serial.println(current);
    Serial.print("Power: "); Serial.println(power);
    Serial.print("Energy: "); Serial.println(energy);
    Serial.print("Frequency: "); Serial.println(frequency);
    Serial.print("PF: "); Serial.println(pf);

    // Check if sensor data is valid
    if (isnan(voltage)) voltage = 0.00;
    if (isnan(current)) current = 0.00;
    if (isnan(power)) power = 0.00;
    if (isnan(energy)) energy = 0.000;
    if (isnan(frequency)) frequency = 0.0;
    if (isnan(pf)) pf = 0.00;

    // Create JSON response
    String response = "{";
    response += "\"voltage\": " + String(voltage, 2) + ",";
    response += "\"current\": " + String(current, 2) + ",";
    response += "\"power\": " + String(power, 2) + ",";
    response += "\"energy\": " + String(energy, 3) + ",";
    response += "\"frequency\": " + String(frequency, 1) + ",";
    response += "\"pf\": " + String(pf, 2);
    response += "}";

    server.send(200, "application/json", response);
}

void setup() {
    Serial.begin(115200);  // Debug Serial
    PZEM_SERIAL.begin(9600, SERIAL_8N1, SERIAL_FULL, 1); // Corrected syntax

    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi!");

    // Print ESP8266 IP address
    Serial.print("ESP8266 IP Address: ");
    Serial.println(WiFi.localIP());

    // Define API endpoints
    server.on("/", handleRoot);
    server.on("/data", handleData);

    // Start the server
    server.begin();
    Serial.println("Web Server started!");
}

void loop() {
    server.handleClient();  // Handle incoming client requests
}
