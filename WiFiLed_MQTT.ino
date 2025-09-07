#include <WiFi.h>
#include <PubSubClient.h>

const char* WIFI_SSID = "AB_WIFI_5B0C";
const char* WIFI_PASS = "12345678";

const char* MQTT_BROKER = "broker.hivemq.com";
const int   MQTT_PORT   = 1883;

const int LED_PIN = 2;

WiFiClient espClient;
PubSubClient client(espClient);

String deviceId;

String topicPrefix() {
  return String("maq/led/") + deviceId;
}

void publishState(bool on, bool retained=true) {
  String t = topicPrefix() + "/state";
  const char* payload = on ? "ON" : "OFF";
  client.publish(t.c_str(), payload, retained);
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
    publishState(true);
  } else if (msg.equalsIgnoreCase("OFF")) {
    digitalWrite(LED_PIN, LOW);
    publishState(false);
  } else if (msg.equalsIgnoreCase("TOGGLE")) {
    bool now = digitalRead(LED_PIN);
    digitalWrite(LED_PIN, !now);
    publishState(!now);
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
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker ");
    Serial.print(MQTT_BROKER);
    Serial.print(":");
    Serial.println(MQTT_PORT);
    if (client.connect(clientId.c_str(), nullptr, nullptr, lwtTopic.c_str(), 0, true, "offline")) {
      Serial.println("MQTT connected");
      String setTopic = topicPrefix() + "/set";
      client.subscribe(setTopic.c_str());
      publishState(digitalRead(LED_PIN), true);
      client.publish((topicPrefix() + "/lwt").c_str(), "online", true);
    } else {
      Serial.print("MQTT connect failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // deviceId from MAC
  uint8_t mac[6];
  WiFi.macAddress(mac);
  char macStr[13];
  snprintf(macStr, sizeof(macStr), "%02X%02X%02X%02X%02X%02X",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  deviceId = String(macStr);

  Serial.println("\n--- MQTT TEST BOOT ---");
  Serial.print("Device ID: "); Serial.println(deviceId);

  connectWiFi();
  if (WiFi.status() == WL_CONNECTED) {
    connectMQTT();
  } else {
    Serial.println("Skipping MQTT connect due to WiFi failure.");
  }
}

unsigned long lastReconnectAttempt = 0;
void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      unsigned long now = millis();
      if (now - lastReconnectAttempt > 5000) {
        lastReconnectAttempt = now;
        connectMQTT();
      }
    } else {
      client.loop();
    }
  }
}
