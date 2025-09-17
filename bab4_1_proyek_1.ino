// ==== BAGIAN 0: KONFIGURASI BLYNK & WIFI ====
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID" // Ganti dari Blynk Console
#define BLYNK_DEVICE_NAME "Kelas Cerdas"     // Ganti dari Blynk Console

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// --- KREDENSIAL PENGGUNA (WAJIB DIGANTI!) ---
char auth[] = "YOUR_BLYNK_AUTH_TOKEN"; 
char ssid[] = "NAMA_WIFI_ANDA";
char pass[] = "PASSWORD_WIFI_ANDA";





// --- KONFIGURASI PIN & THRESHOLD ---
#define DHTPIN 4
#define DHTTYPE DHT11
const int PIN_LDR = 5;
const int PIN_RELAY_LAMPU = 18;
const int PIN_RELAY_KIPAS = 19;
const float SUHU_PANAS = 29.0; // Naikkan sedikit untuk mencegah sering menyala

// --- INISIALISASI OBJEK ---
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

void setup() {
  Serial.begin(115200);
  
  // Inisialisasi komponen
  lcd.init();
  lcd.backlight();
  dht.begin();
  pinMode(PIN_LDR, INPUT);
  pinMode(PIN_RELAY_LAMPU, OUTPUT);
  pinMode(PIN_RELAY_KIPAS, OUTPUT);
  
  // Set kondisi awal relay mati (HIGH = OFF)
  digitalWrite(PIN_RELAY_LAMPU, HIGH);
  digitalWrite(PIN_RELAY_KIPAS, HIGH);
  
  lcd.setCursor(0, 0);
  lcd.print("Connecting...");
  Blynk.begin(auth, ssid, pass);

  // Set timer untuk mengirim data setiap 3 detik
  timer.setInterval(3000L, kirimDataSensor);
  
  lcd.clear();
  lcd.print("System Online!");
}

void loop() {
  Blynk.run();
  timer.run();
  
  jalankanLogikaOtomatis();
  tampilkanDataKeLayar();
}

// Fungsi untuk mengirim data ke Blynk
void kirimDataSensor() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) return;

  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  
  // Kirim status relay ke LED widget di Blynk
  Blynk.virtualWrite(V2, (digitalRead(PIN_RELAY_LAMPU) == LOW) ? 255 : 0); // 255=ON, 0=OFF
}

// Fungsi yang dipanggil saat tombol lampu di Blynk (V3) ditekan
BLYNK_WRITE(V3) {
  int statusTombol = param.asInt();
  digitalWrite(PIN_RELAY_LAMPU, (statusTombol == 1) ? LOW : HIGH);
}

// Fungsi yang dipanggil saat tombol kipas di Blynk (V4) ditekan
BLYNK_WRITE(V4) {
  int statusTombol = param.asInt();
  digitalWrite(PIN_RELAY_KIPAS, (statusTombol == 1) ? LOW : HIGH);
}

void jalankanLogikaOtomatis() {
  // Logika ini bisa dikembangkan agar tidak bentrok dengan kontrol manual
  // Untuk saat ini, kita biarkan kontrol manual dari Blynk menjadi prioritas
  int statusLDR = digitalRead(PIN_LDR);
  float suhu = dht.readTemperature();
  
  // Contoh: jika LDR gelap, nyalakan lampu
  if (statusLDR == 0) {
      // digitalWrite(PIN_RELAY_LAMPU, LOW); // Logika ini bisa diaktifkan/dinonaktifkan
  }
}

void tampilkanDataKeLayar() {
  float suhu = dht.readTemperature();
  
  lcd.setCursor(0, 0);
  lcd.print("Suhu: ");
  lcd.print(suhu, 1);
  lcd.print((char)223);
  lcd.print("C   ");
  
  lcd.setCursor(0, 1);
  lcd.print("L:");
  lcd.print((digitalRead(PIN_RELAY_LAMPU) == LOW) ? "ON " : "OFF");
  lcd.print(" K:");
  lcd.print((digitalRead(PIN_RELAY_KIPAS) == LOW) ? "ON " : "OFF");
}
