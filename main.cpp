#include <ESP8266WiFi.h>
#include <espnow.h>
#include <DHT.h>

#define DHTPIN D2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

uint8_t broadcastAddress[] = {0x8C, 0xAA, 0xB5, 0x7F, 0x21, 0x50};

typedef struct struct_message {
  float temperature;
  float humidity;
  int soilMoisture;
} struct_message;

struct_message myData;

unsigned long lastTime = 0;
unsigned long timerDelay = 5000; // send readings timer

const int soilMoisturePin = A0;

// Pin untuk relay
const int relayPin = D5;  // Ganti dengan pin yang sesuai

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0) {
    Serial.println("Delivery success");
  } else {
    Serial.println("Delivery fail");
  }
}

void setup() {
  Serial.begin(115200);
  
  dht.begin();
  
  pinMode(relayPin, OUTPUT);  // Inisialisasi pin relay

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    int soilMoistureValue = analogRead(soilMoisturePin);

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Gagal membaca dari sensor DHT11!");
      return;
    }

    myData.temperature = temperature;
    myData.humidity = humidity;
    myData.soilMoisture = soilMoistureValue;

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print("°C | Humidity: ");
    Serial.print(humidity);
    Serial.print("% | Soil Moisture: ");
    Serial.println(soilMoistureValue);

    // Kirim data melalui ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));

    // Aktivasi relay berdasarkan kondisi kelembaban tanah
    if (soilMoistureValue < 700) {
      digitalWrite(relayPin, HIGH);  // Aktifkan relay
      Serial.println("Mini Pump OFF"); 
    } else {
      digitalWrite(relayPin, LOW);  // Nonaktifkan relay
      Serial.println("Mini Pump ON");
    }

    lastTime = millis();
  }
}
