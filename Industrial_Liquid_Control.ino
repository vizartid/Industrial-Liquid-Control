#include <LiquidCrystal_I2C.h>
#include <Servo.h>
// ==================================
// KONFIGURASI DAN DEFINISI VARIABEL
// ==================================

// Inisialisasi Layar LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Inisialisasi Servo (Sebagai Katup Kontrol)
Servo valveServo; 

// Definisi Pin Kabel (Hardware)
// Kelompok Input Sensor
const int tmp36Pin = A0;          // Kabel tengah sensor suhu masuk ke A0
const int pressurePin = A1;       // Kabel tengah potensiometer masuk ke A1
const int trigPin = 5;            // Kabel Trigger ultrasonik masuk ke Pin 5
const int echoPin = 6;            // Kabel Echo ultrasonik masuk ke Pin 6

// Kelompok Output Penggerak/Indikator
const int pumpPin = 4;            // Kabel Motor DC (Pompa) masuk ke Pin 4
const int buzzerPin = 9;          // Kabel Positif Buzzer masuk ke Pin 9
const int ledPin = 7;             // Kabel Positif LED masuk ke Pin 7
const int servoPin = 3;           // Kabel Servo (Signal/Orange) masuk ke Pin 3

// Variabel Data Penyimpanan Sementara Di set 0 
float temperature = 0;            // Menyimpan hasil hitungan suhu (°C)
float level = 0;                  // Menyimpan hasil hitungan jarak ultrasonik (cm)
int tekanan = 0;                 // Menyimpan hasil persentase tekanan (%)

// Catatan 
// CATATAN TIPE DATA & FUNGSI:
// const = Variabel bersifat TETAP (tidak bisa diubah-ubah lagi nilainya).
// int   = Tipe data untuk Bilangan Bulat (1, 10, 100).
// float = Tipe data untuk Desimal/Pecahan (3.14, 20.5).
// bool = Tipe data true or false (0,1)

// Nilai Batas Aman / Ambang batas (Threshold)
const float BATAS_TEMPERATURE = 40.0;    // Batas suhu maksimal (40 derajat)
const float BATAS_LEVEL = 67.2;   // Batas jarak pompa nyala (20% dari 336cm)
const int BATAS_TEKANAN = 80;    // Batas Tekanan dalam Persen (80%)


// Alarm berbunyi jika Tekanan > 80% ATAU suhu > 40 derajat

// void  = Jenis fungsi yang hanya menjalankan perintah (aksi) tanpa mengembalikan nilai hasil (return).
void setup() {  // Mengatur arah fungsi setiap pin

  // Sensor (Ultrasonik)
  pinMode(trigPin, OUTPUT);       // Mengirim sinyal suara (Output)
  pinMode(echoPin, INPUT);        // Menerima pantulan suara (Input)
  
  // Aktuator (Penggerak)
  pinMode(pumpPin, OUTPUT);       // Set Pompa sebagai Output
  pinMode(buzzerPin, OUTPUT);     // Set Buzzer sebagai Output
  pinMode(ledPin, OUTPUT);        // Set LED sebagai Output
  
  // INISIALISASI KATUP (SERVO)
  valveServo.attach(servoPin);    // Sambungkan servo ke Pin 3
  valveServo.write(90);            // Posisi awal servo TERTUTUP (0 derajat) -> Perbaikan agar sinkron dengan komentar
  
  // Memastikan semua komponen mati saat listrik pertama kali masuk
  digitalWrite(pumpPin, 0);       // Memastikan Pompa Mati
  digitalWrite(buzzerPin, 0);     // Memastikan Buzzer Diam
  digitalWrite(ledPin, 0);        // Memastikan LED Mati
  
  // MULAI SISTEM KOMUNIKASI & LAYAR
  Serial.begin(9600);             // Memulai komunikasi ke sistem
  lcd.init();                     // Menyalakan LCD
  lcd.backlight();                // Menyalakan lampu latar LCD
  
  // Menampilkan pesan booting
  lcd.print("M Yusuf Aditiya");   
  lcd.setCursor(0, 1);
  lcd.print("125490072");
  delay(1000); 
  lcd.clear();  
  lcd.print("LIQUID CONTROL");  
  lcd.setCursor(0, 1);
  lcd.print("SYSTEM V1.0");      
  delay(2000);                    // delay 2 detik 
  lcd.clear();                    // membersihkan layar untuk mulai program utama
}

// ====================
// PROGRAM UTAMA (LOOP= Pembacaan Terus berulang)
// ====================
// Kode ini akan berulang terus menerus selama devicenya tidak dimatikan (Close loop)
void loop() {
  // Membaca kondisi lingkungan lewat sensor
  PembacaanLevel();           // Membaca ketiggian air di dalam tangi
  PembacaanTemperature();     // Membaca suhu didalam tangki
  PembacaanTekanan();        // membaca tekanan didalam tangki  
  
  // Alur Logika untuk megambil keputusan (Otak sistem)
  LogikaPump();        // Menentukan apakah pompa harus nyala atau mati jika batas levelnya di atas 20% (67.2)
  LogikaAlarm();       // Mementukan alarm perlu menyala jika dalam kondisi berbahaya tekanan 80% dan temperatur diatas 40%
  LogikaValve();        // Menentukan apakah katup perlu dibuka saat tekanan diatas 80%
  
  // Melaporkan hasil ke manusia
  UpdateLCD();          // Menampilkan data status terbaru ke Layar LCD
  UpdateSerial();      
  
  delay(1000);           // delay 1 detik sebelum mengulang proses pembacaan lagi
}

// ============================
// RINCIAN FUNGSI (ALUR LOGIKA)
// ============================

// Fungsi Pembacaan Sensor 

void PembacaanLevel() {
  // Proses menembakkan gelombang suara ultrasonik
  digitalWrite(trigPin, 0);
  delayMicroseconds(2);
  digitalWrite(trigPin, 1);
  delayMicroseconds(10);
  digitalWrite(trigPin, 0);
  // Menghitung berapa lama suara kembali (Echo)
  long duration = pulseIn(echoPin, 1);
  // Mengubah waktu pantulan menjadi Jarak (cm)
  level = duration * 0.034 / 2;
  // Jika "level" atau jarak (> 67.2 cm) , artinya air jauh dari sensor (Tangki Kosong/Sedikit)  
  // Jika "level" atau jarak) (< 67.2 cm) , artinya air dekat dari sensor (Tangki penuh)  
}

void PembacaanTemperature() {
  // Membaca data listrik dari sensor TMP36 
  int tempValue = analogRead(tmp36Pin);  
  // Mengubah data listrik menjadi Tegangan (V)
  float voltage = tempValue * 5.0 / 1024.0;
  // Mengubah Tegangan menjadi Suhu Celcius (Datasheet TMP36)
  temperature = (voltage - 0.5) * 100;
}

void PembacaanTekanan() {
  // Membaca putaran potensiometer
  int tekananValue = analogRead(pressurePin);
  // Mengubah skala 0-1000 menjadi Persen 0-100%
  tekanan = map(tekananValue, 0, 1000, 0, 100);
}

// Fungsi Kendali Otomatis

void LogikaPump() {
  // Jika 'level' (jarak) > 67.2 cm, artinya air jauh dari sensor (Tangki Kosong/Sedikit)
  if (level > BATAS_LEVEL) { 
    digitalWrite(pumpPin, 1);  // Menyalakan Pompa untuk mengisi
  } else {
  // Jika 'level' (jarak) < 67.2 cm , artinya air dekat dari sensor (Tangki penuh)  
    digitalWrite(pumpPin, 0);   //  Mematikan Pompa
  }
}

void LogikaValve() {
  // Jika Tekanan lebih dari (> 80%), maka servo otomatis akan terbuka untuk membuang tekanan pada tangki)
  if (tekanan > BATAS_TEKANAN) {
    valveServo.write(0); // Servo 0 derajat (Buka)
  } else {
  // Jika Tekanan Kurang dari (< 80%) , maka servo otomatis akan tertutup
    valveServo.write(90);// Servo 0 derajat (Tutup)
  }
}

void LogikaAlarm() {
  // Fungsi Alarm akan nyala jika Suhu di atas 40 derajat atau Tekanan di atas 80 persen 
  if (temperature > BATAS_TEMPERATURE || tekanan > BATAS_TEKANAN) {
    digitalWrite(buzzerPin, 1); // Menyalakan suara peringatan 
    digitalWrite(ledPin, 1);    // Menyalakan lampu peringatan 
  } else {
    digitalWrite(buzzerPin, 0);  // Mematikan sirine (Aman)
    digitalWrite(ledPin, 0);     // Mematikan lampu merah
  }
}

// Fungsi Tampilan Layar 

void UpdateLCD() {
  //  Menampilkan Suhu (T) dan Jarak Air (L)
  lcd.setCursor(0, 0);
  lcd.print("T:"); 
  lcd.print(temperature, 1);      // Tampilkan suhu 1 angka belakang koma
  lcd.print("C ");
  lcd.print("L:"); 
  lcd.print(level, 1);            // Tampilkan jarak 1 angka belakang koma
  lcd.print("cm");
  
  // Menampilkan Persentase Tekanan dan Status Sistem
  lcd.setCursor(0, 1);
  lcd.print("P:"); 
  lcd.print(tekanan);           // Tampilkan tekanan tanpa koma
  lcd.print("% ");
  
  // Logika Menampilkan Pesan Status(LCD) (Belum selesai komentarnya)
  if (tekanan > BATAS_TEKANAN) {
    lcd.print("VALVE OPEN ");// Tampil jika Katup terbuka
  } else if (temperature > BATAS_TEMPERATURE) {
    lcd.print("ALERT!    ");      // Tampil jika kepanasan
  } else if (level > BATAS_LEVEL) {
    lcd.print("PUMP ON   ");      // Tampil jika sedang mengisi air
  } else {
    lcd.print("NORMAL    ");      // Tampil jika standby (penuh & aman)
  }
}

void UpdateSerial() {
  // Fungsi ini bertugas mengirim laporan ke layar Laptop (Serial Monitor)
  
  // Menampilkan Data Sensor (Suhu, Level, Tekanan)
  Serial.print("Suhu: "); 
  Serial.print(temperature, 1);
  Serial.print("C | Level: "); 
  Serial.print(level, 1);
  Serial.print("cm | Tekanan: "); 
  Serial.print(tekanan);
  Serial.print("% | Pompa: "); 
  
  // Menampilkan Status Pompa
  Serial.print(digitalRead(pumpPin) ? "ON" : "OFF");
  Serial.print(" | Katup: "); 
  
  // Menampilkan Status Servo
  // Jika > 80% tulis 'OPEN', selain itu tulis 'CLOSED'"
  Serial.println(tekanan > BATAS_TEKANAN ? "OPEN" : "CLOSED");
}
