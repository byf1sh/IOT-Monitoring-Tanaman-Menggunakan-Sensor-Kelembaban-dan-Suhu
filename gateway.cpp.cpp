#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <espnow.h>

// ... (Kode Anda yang lain seperti definisi konstanta, variabel, dan fungsi callback tetap sama)
const char *ssid = "bian";         // Enter your WiFi name
const char *password = "12345678"; // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic_mqtt1 = "testtopic/moisture";
const char *topic_mqtt2 = "testtopic/pumpindex";
const char *topic_mqtt3 = "testtopic/humidity";
const char *topic_mqtt4 = "testtopic/temperature";
const char *mqtt_username = "mqtt";
const char *mqtt_password = "123";
const int mqtt_port = 1883;
// Structure untuk ESP-NOW

int jarak;
int status;

typedef struct struct_message {
  float temperature;
  float humidity;
  int soilMoisture;
} struct_message;

struct_message myData;

WiFiClient espClient;
PubSubClient client(espClient);
char str[80];

// ... (Kode Anda yang lain seperti setup() dan callback() tetap sama)

void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData, incomingData, sizeof(myData));

  // Mengirim data temperature ke MQTT pada topic_mqtt2
  char messageTemp[10];
  snprintf(messageTemp, sizeof(messageTemp), "%.2f", myData.temperature); // Hanya mengambil data temperature
  client.publish(topic_mqtt4, messageTemp); // Mengirim ke topic_mqtt2

  // Mengirim data humidity ke MQTT pada topic_mqtt3
  char messageHumidity[10];
  snprintf(messageHumidity, sizeof(messageHumidity), "%.2f", myData.humidity); // Hanya mengambil data humidity
  client.publish(topic_mqtt3, messageHumidity); // Mengirim ke topic_mqtt3

  // Mengirim data soilMoisture ke MQTT pada topic_mqtt4
  char messageSoilMoisture[10];
  snprintf(messageSoilMoisture, sizeof(messageSoilMoisture), "%d", myData.soilMoisture); // Mengambil data soilMoisture sebagai integer
  client.publish(topic_mqtt1, messageSoilMoisture); // Mengirim ke topic_mqtt4

  Serial.println("Data diterima dan diterbitkan ke MQTT.");
}
void callback(char *topic, byte *payload, unsigned int length)
{

  Serial.print("Pesan baru dari topik: ");
  Serial.println(topic);

  if (strcmp(topic, "kelasiotesp/PENYOLDER/distance: ") == 0)
  {
    payload[length] = '\0';        // Tambahkan null terminator untuk mengonversi ke string
    jarak = atoi((char *)payload); // Konversi string menjadi integer
    Serial.print("Jarak: ");
    Serial.println(jarak);
  }

  if (strcmp(topic, "kelasiotesp/PENYOLDER/interrupt:") == 0)
  {
    payload[length] = '\0';        // Tambahkan null terminator untuk mengonversi ke string
    status = atoi((char *)payload); // Konversi string menjadi integer
    Serial.print("Status: ");
    Serial.println(status);
  }
}


void setup() {
  Serial.begin(115200);

  // Connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  // Connecting to an MQTT broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  while (!client.connected())
  {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password))
    {
      Serial.println("Public emqx mqtt broker connected");
    }
    else
    {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  client.subscribe("kelasiotesp/PENYOLDER/distance: ");
  client.subscribe("kelasiotesp/PENYOLDER/interrupt:");
  // ... (Kode Anda yang lain seperti koneksi WiFi dan MQTT tetap sama)

  // Inisialisasi ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Setelah berhasil inisialisasi ESP-NOW, kita akan mendaftar untuk menerima callback
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  client.loop();
  // Dalam loop(), Anda bisa menambahkan kode lain jika diperlukan
}
