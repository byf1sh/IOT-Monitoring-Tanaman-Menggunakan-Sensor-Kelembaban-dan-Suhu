# Sistem Penyemprotan Tanaman Otomatis dengan DHT11 dan Resistive Soil Moisture Sensor
Proyek untuk Memenuhi Nilai Tugas Besar IoT Universitas Telkom, Desember 2023

**Nama:** Abyan Hafizh, Ahmad Adil, Geril Hidayat, Rully Lukas

# Ringkasan Tujuan
Tujuan pembuatannya untuk membuat perangkat iot sederhana dengan 2 sensor untuk penempatan di samping tanaman yang memantau suhu dan kelembaban udara di sekitar tanaman serta kelembaban di tanah tanaman untuk mengetahui kapan Anda perlu menyiram tanaman

**Perkiraan lamanya mengerjakan project:**
Sekitar 1 sampai 2 minggu


# Objektif
**Alasan memilih project ini:**
Alasan kami memilih proyek ini adalah karena kami memiliki beberapa tanaman di kontrakan dan memiliki kebiasaan terkadang lupa menyiraminya sebelum tanaman mulai layu sehingga kami dapat mengetahui kapan harus menyiram tanaman tergantung pada tingkat kelembaban tanah.

**Tujuan Proyek:**
Tujuan dari proyek ini adalah pertama-tama untuk mempelajari tentang Internet of Things, cara membuat perangkat iot dan menghubungkannya ke platform untuk memvisualisasikan datanya. Dan kedua, memantau tanaman untuk mengetahui kapan kami perlu menyiramnya.

**Wawasan yang diperoleh dengan melakukan proyek:**
Proyek ini mampu memberikan wawasan tentang konsep dasar IOT, pemrograman, dan rangkaian listrik.

# Material
Komponen | Tujuan

| Nama Komponen | Pcs | Harga|
| ------ | ------ | ------ |
| ESP12 Mini | 2 pcs |  Rp. 35.000
| Relay | 1 pcs |  Rp. 5.000
| Mini Water Pump | 1 pcs | Rp. 10.000
| Battery Storage Case Holder |1 pcs | Rp. 20.000
| USB power cable | 1 pcs | Rp. 5.000
| Jumper wires | 10 pcs |  Rp. 10.000
| 0.5m Vinyl Tubing | 1 pcs |  Rp. 12.000
| Soil Moisture Detector | 1 pcs | Rp. 10.000
| Jumper wires male to male | 12 pcs | Rp. 14.000
| Module DHT11 | 1 pcs | Rp. 10.000
| Breadboard 400 points | 1 pcs | Rp. 13.000

# Computer Setup
**Setting up the IDE:**

Pertama tama langkah yang kami lakukan adalah melakukan koneksi antara laptop dengan esp12 mini yang kami gunakan sebagai microcontroller, koneksi kami lakukan menggunakan bantuan platform.io.

langkah selanjutnya yang kita lakukan adalah melakukan instalasi library yang diperlukan seperti PubSubClient.h, Adafruit_Sensor.h, dan ESP8266WiFi.h. Hal ini penting dilakukan karena jika tidak dilakukan maka akan terjadi kesalahan dan program yang dituliskan tidak akan terbaca oleh microcontroler.

Langkah selanjutnya yang kmi lakukan adalah melakukan instalasi NodeJS (Windows download link https://nodejs.org/en/) dan pilih IDE untuk projek. kemudian sesaat setelah selesai menginstal NodeJS lanjutkan dengan menginstal Node-Red.

Semua hal hal yang diperlukan sudah terinstall pada pc, selanjutnya adalah tinggal kita lakukan running dari code code yang ada. untuk melakukan percobaan runnning IOT Prototype, pertama tama anda harus lebih dulu melakukan running node-red pada localhost anda, pastikan juga MQTT channel berjalan dengan baik, agar tidak terjadi kesalahan ketika melakukan running code.

# Menyatukan Semua Alat
Setup dari device dan perkabelan bisa dilihat kurang lebih seperti gambar dibawah ini. sebagai catatan, microcontroller yang kami tunjukan dibawah berbeda dengan yang kami gunakan, hal ini dikarenakan saya tidak dapat menemukan microcontroller yang saya gunakan pada platform tinkercard, soil moisture sensor juga berbeda, namun kurang lebih masih sama.

![WhatsApp Image 2023-12-18 at 17 02 59_0381a371](https://github.com/rullylukas/iot/assets/154656456/b184ded4-15ea-4f18-994a-473ba5efeff1)
Foto 2 : Simulasi perakitan menggunakan Tinkercad


# Platform yang Dipilih
Adapun platfrom yang kami pilih sebagai Dashboard adalah Node-red, kami memilih Node-red karena kami rasa Node-red sangat cocok untuk pembuatan projek ini. Node Red dapat dengan mudah melakukan visualisasi data secara real-time online di dashboard, kita juga dapat memilih bagaimana cara kita ingin menampilkan data tersebut. dalam kasus ini saya memilih menampilkan data menggunakan chart dan juga gauge.

![Screenshot 2022-07-03 161432](https://tangiblejs.com/wp-content/uploads/2019/03/node-red.png)
Foto 3: node-red dashboard page

# Kodingan 
**Microcontroller 1 (Sensor Kelembaban Tanah):**
Berikut kode dari sisi node dan mengirimkannya ke gateway
```
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
```

Dan kode dibawah ini kode dari sisi gateway
```
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
```

# Node-red dan Database
Berikut merupakan tampilan dari node-red flow dan juga struktur database yang kami gunaka pada projek kali ini

nodered
![Screenshot 2022-07-03 191415](https://github.com/byf1sh/IOT-Monitoring-Tanaman-Menggunakan-Sensor-Kelembaban-dan-Suhu/blob/main/assets/nodenew.jpg?raw=true)

firebaseDB
![Screenshot 2022-07-03 191415](https://github.com/byf1sh/IOT-Monitoring-Tanaman-Menggunakan-Sensor-Kelembaban-dan-Suhu/blob/main/assets/firebasenew.jpg?raw=true)

# Menyajikan Data
Dasbor pada platform Nodered dikonfigurasi dengan memanfaatkan tiga aliran data (feeds) yang masing-masing mengkategorikan kelembapan udara, kelembapan tanah, dan suhu. Setiap dari ketiga aliran data ini dilengkapi dengan satu diagram yang merepresentasikan perubahan nilai dari masing-masing sensor.

Untuk air level dan suhu, pilihan jatuh pada penggunaan sebuah meteran (gauge) yang menampilkan persentase kelembapan pada saat pengukuran terakhir. Sementara itu, untuk tingkat kebasahan, sebuah bidang teks digunakan untuk menunjukkan nilai suhu saat pengukuran terakhir
![Screenshot 2022-07-03 191415](https://github.com/byf1sh/IOT-Monitoring-Tanaman-Menggunakan-Sensor-Kelembaban-dan-Suhu/blob/main/assets/dashboardnew.jpg?raw=true)



# Menyelesaikan Desain
Setelah merakit komponen ini hasil yang bisa kami tampilkan.
![20220704_204437](https://github.com/byf1sh/IOT-Monitoring-Tanaman-Menggunakan-Sensor-Kelembaban-dan-Suhu/blob/main/assets/Alat%201.jpg?raw=true)

![20220704_204443](https://github.com/byf1sh/IOT-Monitoring-Tanaman-Menggunakan-Sensor-Kelembaban-dan-Suhu/blob/main/assets/Alat%202.jpg?raw=true)

![20220704_204429](https://github.com/byf1sh/IOT-Monitoring-Tanaman-Menggunakan-Sensor-Kelembaban-dan-Suhu/blob/main/assets/Alat%203.jpg?raw=true)

![20220704_204429](https://github.com/byf1sh/IOT-Monitoring-Tanaman-Menggunakan-Sensor-Kelembaban-dan-Suhu/blob/main/assets/Alat%204.jpg?raw=true)

**Kata Penutup**

Saya berpendapat bahwa hasil dari proyek ini memenuhi ekspektasi saya, mengingat bahwa saya berhasil mencapai tujuan yang telah saya tetapkan. Dengan pengalaman sebelumnya dalam pemrograman, saya mengamati bahwa aspek pemrograman dalam proyek ini relatif tidak menantang. Namun, tantangan muncul saat mengkalibrasi sensor kelembapan tanah berbasis kapasitif untuk mengukur data dengan akurasi yang memadai. Proses ini memerlukan pencarian informasi melalui berbagai sumber dan referensi. Selain itu, saya mengapresiasi kualitas instruksi dari dosen IOT yang membimbing saya, yang memberikan informasi yang informatif dan mendukung, sehingga memudahkan jalannya proyek tanpa hambatan signifikan


