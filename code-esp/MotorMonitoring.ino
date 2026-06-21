#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

#define DHTPIN 15
#define DHTTYPE DHT22

#define VIBRATION_PIN 34
#define CURRENT_PIN 35

#define BUZZER_PIN 18

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {

  delay(10);

  Serial.println();
  Serial.print("Connecting to WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");
}

void reconnect() {

  while (!client.connected()) {

    Serial.print("Connecting MQTT...");

    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {

      Serial.println("Connected");
      client.subscribe("alika2026/iot/motor1/ai");

    } else {

      Serial.print("Failed, rc=");
      Serial.print(client.state());

      delay(2000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {

  String prediction = "";

  for (int i = 0; i < length; i++) {
    prediction += (char)payload[i];
  }

  Serial.print("AI Prediction: ");
  Serial.println(prediction);

  if (prediction == "NORMAL") {

    noTone(BUZZER_PIN);

  }
  else if (prediction == "WARNING") {

    tone(BUZZER_PIN, 1000);
    delay(300);
    noTone(BUZZER_PIN);

  }
  else if (prediction == "FAULT") {

    tone(BUZZER_PIN, 2000);

  }
}

void setup() {

  Serial.begin(115200);

  dht.begin();

  pinMode(BUZZER_PIN, OUTPUT);

  setup_wifi();

  client.setServer(mqtt_server, 1883);

  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  int vibration = analogRead(VIBRATION_PIN);
  int currentRaw = analogRead(CURRENT_PIN);

  float current = currentRaw * 10.0 / 4095.0;

  StaticJsonDocument<256> doc;

  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["vibration"] = vibration;
  doc["current"] = current;

  char buffer[256];

  serializeJson(doc, buffer);

  if (client.publish("alika2026/iot/motor1", buffer)) {
    Serial.print("Published: ");
    Serial.println(buffer);
  } else {
    Serial.println("Publish FAILED!");
  }

  delay(5000);
}