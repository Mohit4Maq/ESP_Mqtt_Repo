#include <WiFi.h>
#include <PubSubClient.h>

const char* WIFI_SSID = "AB_WIFI_5B0C";
const char* WIFI_PASS = "12345678";

const char* MQTT_BROKER = "broker.hivemq.com";
const int   MQTT_PORT   = 1883;

const int LED_PIN = 2;  // Built-in LED on most ESP32 boards
// Alternative pins to try if pin 2 doesn't work: GPIO 5, 18, 19, 21, 22, 23
// Note: Some ESP32 boards have inverted logic (LOW = ON, HIGH = OFF)
// If LED doesn't work as expected, try swapping HIGH/LOW values

WiFiClient espClient;
PubSubClient client(espClient);

String deviceId;

String topicPrefix() {
  return String("maq/led/") + deviceId;
}

void publishState(bool on, bool retained=true) {
  String t = topicPrefix() + "/state";
  const char* payload = on ? "ON" : "OFF";
  bool success = client.publish(t.c_str(), payload, retained);
  Serial.print("Published state: ");
  Serial.print(payload);
  Serial.print(" to ");
  Serial.print(t);
  Serial.print(" (retained: ");
  Serial.print(retained ? "true" : "false");
  Serial.print(", success: ");
  Serial.println(success ? "true" : "false");
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  msg.trim();
  Serial.print("Received on ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(msg);
  
  if (msg.equalsIgnoreCase("ON")) {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED turned ON");
    publishState(true);
  } else if (msg.equalsIgnoreCase("OFF")) {
    digitalWrite(LED_PIN, LOW);
    Serial.println("LED turned OFF");
    publishState(false);
  } else if (msg.equalsIgnoreCase("TOGGLE")) {
    bool currentState = digitalRead(LED_PIN);
    bool newState = !currentState;
    digitalWrite(LED_PIN, newState);
    Serial.print("LED toggled from ");
    Serial.print(currentState ? "ON" : "OFF");
    Serial.print(" to ");
    Serial.println(newState ? "ON" : "OFF");
    publishState(newState);
  } else {
    Serial.print("Unknown command: ");
    Serial.println(msg);
  }
}

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi ");
  Serial.print(WIFI_SSID);
  Serial.print(" ");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi connected. IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("RSSI: "); Serial.println(WiFi.RSSI());
  } else {
    Serial.print("WiFi failed, status=");
    Serial.println(WiFi.status());
  }
}

void connectMQTT() {
  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(callback);
  String clientId = "esp32-" + deviceId;
  String lwtTopic = topicPrefix() + "/lwt";
  
  Serial.print("Attempting MQTT connection to ");
  Serial.print(MQTT_BROKER);
  Serial.print(":");
  Serial.print(MQTT_PORT);
  Serial.print(" with client ID: ");
  Serial.println(clientId);
  
  int attempts = 0;
  while (!client.connected() && attempts < 10) {
    attempts++;
    Serial.print("MQTT connection attempt ");
    Serial.print(attempts);
    Serial.print("/10... ");
    
    if (client.connect(clientId.c_str(), nullptr, nullptr, lwtTopic.c_str(), 0, true, "offline")) {
      Serial.println("SUCCESS!");
      Serial.println("MQTT broker connected");
      
      String setTopic = topicPrefix() + "/set";
      Serial.print("Subscribing to: ");
      Serial.println(setTopic);
      
      if (client.subscribe(setTopic.c_str())) {
        Serial.println("Successfully subscribed to set topic");
      } else {
        Serial.println("Failed to subscribe to set topic");
      }
      
      // Publish current state
      bool currentState = digitalRead(LED_PIN);
      Serial.print("Publishing initial state: ");
      Serial.println(currentState ? "ON" : "OFF");
      publishState(currentState, true);
      
      // Publish online status
      client.publish((topicPrefix() + "/lwt").c_str(), "online", true);
      Serial.println("Published online status");
      
    } else {
      Serial.print("FAILED (rc=");
      Serial.print(client.state());
      Serial.println(")");
      delay(2000);
    }
  }
  
  if (!client.connected()) {
    Serial.println("MQTT connection failed after 10 attempts");
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  
  // Initialize LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.println("LED initialized on pin 2 (OFF)");

  // deviceId from MAC
  uint8_t mac[6];
  WiFi.macAddress(mac);
  char macStr[13];
  snprintf(macStr, sizeof(macStr), "%02X%02X%02X%02X%02X%02X",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  deviceId = String(macStr);

  Serial.println("\n--- MQTT LED CONTROLLER BOOT ---");
  Serial.print("Device ID: "); Serial.println(deviceId);
  Serial.print("LED Pin: "); Serial.println(LED_PIN);
  Serial.print("MQTT Broker: "); Serial.print(MQTT_BROKER); Serial.print(":"); Serial.println(MQTT_PORT);

  // Test LED functionality
  Serial.println("Testing LED...");
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  Serial.println("LED should be ON now");
  digitalWrite(LED_PIN, LOW);
  delay(500);
  Serial.println("LED should be OFF now");

  connectWiFi();
  if (WiFi.status() == WL_CONNECTED) {
    connectMQTT();
  } else {
    Serial.println("Skipping MQTT connect due to WiFi failure.");
  }
}

unsigned long lastReconnectAttempt = 0;
unsigned long lastHeartbeat = 0;

void loop() {
  // Manual LED test - uncomment to test LED without MQTT
  // static unsigned long lastTest = 0;
  // if (millis() - lastTest > 3000) {
  //   lastTest = millis();
  //   static bool testState = false;
  //   testState = !testState;
  //   digitalWrite(LED_PIN, testState ? HIGH : LOW);
  //   Serial.print("Manual test - LED: ");
  //   Serial.println(testState ? "ON" : "OFF");
  // }

  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      unsigned long now = millis();
      if (now - lastReconnectAttempt > 5000) {
        lastReconnectAttempt = now;
        Serial.println("Attempting MQTT reconnection...");
        connectMQTT();
      }
    } else {
      client.loop();
      
      // Heartbeat every 30 seconds
      if (millis() - lastHeartbeat > 30000) {
        lastHeartbeat = millis();
        Serial.println("Heartbeat - MQTT connected, LED state: " + String(digitalRead(LED_PIN) ? "ON" : "OFF"));
      }
    }
  } else {
    Serial.println("WiFi disconnected, attempting reconnection...");
    connectWiFi();
  }
}
