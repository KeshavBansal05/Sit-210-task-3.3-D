#include <WiFiNINA.h>
#include <PubSubClient.h>

// WiFi and MQTT Broker Settings
const char* ssid = "Bansal";
const char* password = "bansaljikeshavji";
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char* mqtt_wave_topic = "SIT210/wave";
const char* mqtt_pat_topic = "SIT210/pat";

// LED Pin
const int ledPin = 8;

// WiFi and MQTT Client
WiFiClient wifiClient;
PubSubClient client(wifiClient);

// Function prototypes for setup and MQTT handling
void setup_wifi();
void connect_mqtt();
void callback(char* topic, byte* payload, unsigned int length);
void flashLED(int times, int onDuration, int offDuration);

void setup() {
  // Initialize Serial communication and WiFi connection
  Serial.begin(115200);
  setup_wifi();
  
  // Configure the MQTT client and set the callback function to handle incoming messages
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
  // Set the LED pin as output
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // Ensure the client is connected to the MQTT broker
  if (!client.connected()) {
    connect_mqtt();
  }
  
  // Process incoming MQTT messages
  client.loop();
}

void setup_wifi() {
  // Connect to the specified WiFi network
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  // Wait until the WiFi connection is established
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void connect_mqtt() {
  // Attempt to connect to the MQTT broker
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker...");
    
    // If connection is successful, subscribe to the relevant topics
    if (client.connect("ArduinoNanoSubscriber")) {
      Serial.println("connected");
      client.subscribe(mqtt_wave_topic);
      client.subscribe(mqtt_pat_topic);
    } else {
      // Retry connection after a delay if failed
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Handle incoming MQTT messages and determine the action based on the topic
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Flash the LED in a specific pattern based on the received topic
  if (strcmp(topic, mqtt_wave_topic) == 0) {
    flashLED(3, 500, 500); // Flash the LED 3 times with 500ms on/off intervals
  } else if (strcmp(topic, mqtt_pat_topic) == 0) {
    flashLED(6, 250, 250); // Flash the LED 6 times with 250ms on/off intervals
  }
}

void flashLED(int times, int onDuration, int offDuration) {
  // Flash the LED with specified times and durations for on and off states
  for (int i = 0; i < times; i++) {
    digitalWrite(ledPin, HIGH);
    delay(onDuration);
    digitalWrite(ledPin, LOW);
    delay(offDuration);
  }
}
