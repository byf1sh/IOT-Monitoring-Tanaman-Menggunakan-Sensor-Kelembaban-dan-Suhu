#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <RunningMedian.h>  // Library untuk smoothing nilai analog

// WiFi
const char *ssid = "bian"; // Ganti dengan nama WiFi Anda
const char *password = "12345678";  // Ganti dengan kata sandi WiFi Anda

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "testtopic/tubes";
const char *topic2 = "testtopic/moisture";
const char *topic3 = "testtopic/pumpindex";
const char *mqtt_username = "mqtt";
const char *mqtt_password = "123";
const int mqtt_port = 1883;

// Sensor Kelembaban Tanah
const int soilMoisturePin = A0;  // Pin analog kelembaban tanah
RunningMedian soilMoistureValues = RunningMedian(10);  // Objek RunningMedian untuk menghaluskan nilai analog

// Relay untuk Mini Pump
const int relayPin = D6;  // Pin digital untuk mengontrol relay

WiFiClient espClient;
PubSubClient client(espClient);
uint32_t counter;
char str[80];

void callback(char *topic, byte *payload, unsigned int length) {
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");
 for (int i = 0; i < length; i++) {
     Serial.print((char) payload[i]);
 }
 Serial.println();
 Serial.println("-----------------------");
}

void setup() {
 Serial.begin(115200);

 // Inisialisasi pin relay
 pinMode(relayPin, OUTPUT);
 digitalWrite(relayPin, LOW);  // Inisialisasi relay dalam keadaan mati

 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.println("Connecting to WiFi..");
 }
 Serial.println("Connected to the WiFi network");

 client.setServer(mqtt_broker, mqtt_port);
 client.setCallback(callback);
 while (!client.connected()) {
     String client_id = "esp32-client-";
     client_id += String(WiFi.macAddress());
     Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
     if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
         Serial.println("Public emqx mqtt broker connected");
     } else {
         Serial.print("failed with state ");
         Serial.print(client.state());
         delay(2000);
     }
 }

 client.publish(topic, "Hi EMQX I'm esp ^^");
 client.subscribe(topic);
}

void loop() {
 client.loop();

 int soilMoistureValue = analogRead(soilMoisturePin);
 soilMoistureValues.add(soilMoistureValue);
 float smoothedMoisture = soilMoistureValues.getMedian();

 Serial.println("Soil Moisture: " + String(smoothedMoisture));
 sprintf(str, "%.2f", smoothedMoisture);
 client.publish(topic2, str);

 // Kontrol Relay berdasarkan Kelembaban Tanah
 const int moistureThreshold = 940;  // Ganti dengan ambang batas yang sesuai
 if (smoothedMoisture < moistureThreshold) {
   digitalWrite(relayPin, HIGH);  // Aktifkan relay untuk menyalakan mini pump
   Serial.println("Mini Pump is OFF");
   client.publish(topic3, "Mini Pump is OFF");
 } else {
   digitalWrite(relayPin, LOW);  // Matikan relay
   Serial.println("Mini Pump is ON");
   client.publish(topic3, "Mini Pump is ON");
 }

 delay(2000);
}
