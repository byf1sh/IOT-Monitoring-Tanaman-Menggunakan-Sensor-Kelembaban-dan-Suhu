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
Pada bagian ini kami akan membahas seputar kode yang kami gunakan pada perangkat IoT yang kami buat, pertama tama kami akan membahas library yang kami gunakan yang ter lampir pada foto 4 sebagai berikut
```
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>    
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
```

Kemudian kami juga menambahkan kode untuk koneksi ke wifi dan MQTT Broker, ketika device berusaha konek ke wifi kami akan menampilkan "Connecting to wifi.." pada terminal, dan "Connected to the wifi network" jika berhasil konek, sesaat setelah konek ke wifi kami akan langsung mengkonekan device ke mqtt broker dan melakukan delay(2000) ketika gagal melakukan koneksi ke mqttbroker.
```
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
 dht.begin();   
 client.publish(topic, "Hi EMQX I'm esp ^^");
 client.subscribe(topic);
}
```
Gambar selanjutnya dibawah ini adalah fungsi utama dari kode yang bertujuan untuk mengukur data dari sensor, menampilkan nilai-nilai tersebut, serta mengirimkan data tersebut ke platform Node-red melalui MQTT Broker. Selain itu, kode ini dirancang untuk menangani berbagai pengecualian yang mungkin terjadi selama proses pengiriman, memastikan integritas dan kehandalan operasi
```
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

  // Task scheduler untuk menyalakan pompa setiap jam 7 pagi selama 2 menit
  unsigned long currentMillis = millis();
  if (lastPumpStartTime == 0) {
    lastPumpStartTime = currentMillis;  // Inisialisasi waktu pertama kali
  }

  // Hitung jam dalam millis (1 jam = 3600000 millis)
  if ((currentMillis - lastPumpStartTime >= 7 * 3600000) && (currentMillis - lastPumpStartTime < 7 * 3600000 + 120000)) {
    // Jika waktu saat ini adalah jam 7 pagi dan belum lewat 2 menit
    digitalWrite(relayPin, HIGH);  // Nyalakan pompa
  } else {
    // Jika bukan jam 7 pagi, atau sudah lewat 2 menit
    digitalWrite(relayPin, LOW);  // Matikan pompa
    lastPumpStartTime = 0;  // Reset waktu untuk siklus berikutnya
  }

 delay(2000);
}
```


# Transmisi Data / Konektivitas
I decided to send data once every 15 minutes since the purpose of this project is to know when i need to water my plant and since the moisture in the soil does not change that fast then 15 seemed like a good time beetween each measuring. 

Wifi was the wireless protocols used for this project because the micocontroller setup is in my home close to my router and do not need any protocol with longer range because of that. Wifi also has no recurring costs, low latency and less bandwidth restrictions so it seemed like to best options.

As for the transport protocols MQTT and webhooks is used in this project. MQTT is used for sending the data measured by the sensors to adafruit. It was choosen because it is a lightweight, energy-efficient and easy to use Transport protocol. Webhooks is also used for when the moisture level in the soil reaches under 30 percent to send a message to my discord that it is time to water my plant.

# Menyajikan Data
Dasbor pada platform Nodered dikonfigurasi dengan memanfaatkan tiga aliran data (feeds) yang masing-masing mengkategorikan kelembapan udara, kelembapan tanah, dan suhu. Setiap dari ketiga aliran data ini dilengkapi dengan satu diagram yang merepresentasikan perubahan nilai dari masing-masing sensor.

Untuk air level dan suhu, pilihan jatuh pada penggunaan sebuah meteran (gauge) yang menampilkan persentase kelembapan pada saat pengukuran terakhir. Sementara itu, untuk tingkat kebasahan, sebuah bidang teks digunakan untuk menunjukkan nilai suhu saat pengukuran terakhir
![Screenshot 2022-07-03 191415](https://github.com/byf1sh/IOT-Monitoring-Tanaman-Menggunakan-Sensor-Kelembaban-dan-Suhu/blob/main/assets/Dashboard.jpg?raw=true)



# Menyelesaikan Desain
Setelah merakit komponen ini hasil yang bisa kami tampilkan.
![20220704_204437](https://github.com/byf1sh/IOT-Monitoring-Tanaman-Menggunakan-Sensor-Kelembaban-dan-Suhu/blob/main/assets/Alat%201.jpg?raw=true)

![20220704_204443](https://github.com/byf1sh/IOT-Monitoring-Tanaman-Menggunakan-Sensor-Kelembaban-dan-Suhu/blob/main/assets/Alat%202.jpg?raw=true)

![20220704_204429](https://github.com/byf1sh/IOT-Monitoring-Tanaman-Menggunakan-Sensor-Kelembaban-dan-Suhu/blob/main/assets/Alat%203.jpg?raw=true)

![20220704_204429](https://github.com/byf1sh/IOT-Monitoring-Tanaman-Menggunakan-Sensor-Kelembaban-dan-Suhu/blob/main/assets/Alat%204.jpg?raw=true)

**Kata Penutup**

Saya berpendapat bahwa hasil dari proyek ini memenuhi ekspektasi saya, mengingat bahwa saya berhasil mencapai tujuan yang telah saya tetapkan. Dengan pengalaman sebelumnya dalam pemrograman, saya mengamati bahwa aspek pemrograman dalam proyek ini relatif tidak menantang. Namun, tantangan muncul saat mengkalibrasi sensor kelembapan tanah berbasis kapasitif untuk mengukur data dengan akurasi yang memadai. Proses ini memerlukan pencarian informasi melalui berbagai sumber dan referensi. Selain itu, saya mengapresiasi kualitas instruksi dari dosen IOT yang membimbing saya, yang memberikan informasi yang informatif dan mendukung, sehingga memudahkan jalannya proyek tanpa hambatan signifikan


