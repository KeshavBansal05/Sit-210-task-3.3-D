#include <WiFiNINA.h>
#include <PubSubClient.h>

// WiFi and MQTT Broker Settings
// Define the necessary credentials for WiFi connection and MQTT broker details.
const char* ssid = "Bansal";
const char* password = "bansaljikeshavji";
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char* mqtt_wave_topic = "SIT210/wave";
const char* mqtt_pat_topic = "SIT210/pat";

// Ultrasonic Sensor Pins
// Define the pins used to control the ultrasonic sensor.
const int trigPin = 5;
const int echoPin = 4;

// WiFi and MQTT Client
// Instantiate WiFi and MQTT client objects to handle network and messaging operations.
WiFiClient wifiClient;
PubSubClient client(wifiClient);

// Function prototypes
// Declare the functions used to set up WiFi, connect to the MQTT broker, 
// read distance from the sensor, publish messages to the MQTT broker, and 
// check distance conditions to determine which message to send.
void setup_wifi();
void connect_mqtt();
long readDistance();
void publishMessage(const char* topic, const char* message);
void checkAndPublish();

void setup() {
  // Initialize the serial communication for debugging purposes.
  Serial.begin(115200);
  
  // Connect to WiFi and set up the MQTT client.
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  
  // Set the ultrasonic sensor pins as input and output.
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  // Ensure the device remains connected to the MQTT broker.
  if (!client.connected()) {
    connect_mqtt();
  }
  client.loop();
  
  // Check the distance measured by the sensor and publish the appropriate message.
  checkAndPublish();
  
  // Delay to limit the rate of sensor checks and message publications.
  delay(2000);  // Check every 2 seconds
}

void setup_wifi() {
  // Establish a connection to the specified WiFi network.
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  // Wait for the WiFi connection to be established.
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void connect_mqtt() {
  // Attempt to connect to the MQTT broker, retrying until a connection is made.
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker...");
    if (client.connect("ArduinoNanoPublisher")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

long readDistance() {
  // Measure the distance using the ultrasonic sensor by sending a pulse 
  // and measuring the time it takes for the echo to return.
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Calculate distance based on the time it takes for the echo to return.
  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;
  return distance;
}

void publishMessage(const char* topic, const char* message) {
  // Ensure the device is connected to the MQTT broker before attempting 
  // to publish a message, then send the message to the specified topic.
  if (!client.connected()) {
    connect_mqtt();
  }
  client.loop();
  client.publish(topic, message);
  Serial.print("Message published to ");
  Serial.println(topic);
}

void checkAndPublish() {
  // Check the distance measured by the ultrasonic sensor and determine 
  // if it corresponds to a "pat" or a "wave", then publish the appropriate 
  // message to the MQTT broker.
  long distance = readDistance();
  Serial.print("Distance: ");
  Serial.println(distance);

  if (distance < 20) { // Detects a "pat"
    publishMessage(mqtt_pat_topic, "pat from [Keshav]");
  } else if (distance < 40) { // Detects a "wave"
    publishMessage(mqtt_wave_topic, "wave from [Keshav]");
  }
}
