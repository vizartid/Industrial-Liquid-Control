# Sistem Pengendalian Cairan Industri Otomatis (Automatic Industrial Liquid Control System)

![Arduino](https://img.shields.io/badge/Platform-Arduino-blue)
![Status](https://img.shields.io/badge/Status-Prototype-green)

Proyek ini adalah simulasi sistem otomasi industri untuk pengendalian
tangki cairan (*Process Tank*) berbasis mikrokontroler Arduino Uno.
Sistem ini dirancang untuk mengintegrasikan kontrol Level, Tekanan, dan
Suhu secara simultan dalam siklus tertutup (*Closed Loop System*).

![Cover](Circuit-Schematic.png)

Proyek ini dibuat untuk memenuhi tugas mata kuliah **Sistem Kontrol /
Otomasi Industri**.

------------------------------------------------------------------------

## 📋 Daftar Isi

-   [Fitur Utama](#-fitur-utama)
-   [Diagram P&ID](#-diagram-instrumentasi-pid)
-   [Komponen & Hardware](#-komponen--hardware)
-   [Wiring & Pin Mapping](#-wiring--pin-mapping)
-   [Logika Kontrol](#-logika-kontrol)
-   [Instalasi & Simulasi](#-instalasi--simulasi)
-   [Author](#-author)

------------------------------------------------------------------------

## 🚀 Fitur Utama

Sistem ini menerapkan prinsip **Distributed Control System (DCS)**
sederhana dengan fitur:

1.  **Level Control (LIC-101):**
    -   Mengisi air otomatis saat level rendah (\< 20%).
    -   Berhenti otomatis saat level penuh (\> 80%) menggunakan logika
        *Hysteresis* untuk mencegah *chattering* pada pompa.
2.  **Pressure Relief (PIC-101):**
    -   Sistem proteksi tekanan berlebih. Katup servo akan terbuka
        otomatis (Venting) jika tekanan tangki \> 80%.
3.  **Safety Interlock & Alarm (TIC-101 & PAH-101):**
    -   Sistem keamanan prioritas tinggi. Alarm (Buzzer & LED) akan
        aktif jika:
        -   Suhu cairan \> 40°C (Overheat).
        -   **ATAU** Tekanan \> 80% (Overpressure).
4.  **Real-time Monitoring:**
    -   Menampilkan data Suhu, Level, dan Tekanan secara *real-time*
        pada LCD 16x2.

------------------------------------------------------------------------

## 📊 Diagram Instrumentasi (P&ID)

Diagram ini disusun berdasarkan standar **ISA-5.1** (*Instrumentation
Symbols and Identification*).

![P&ID Diagram](PID-Diagram.png)

**Identifikasi Instrumen:** \* **LIC-101:** Level Indicator Controller
(Ultrasonic + Arduino). \* **PIC-101:** Pressure Indicator Controller
(Potentiometer + Arduino). \* **TIC-101:** Temperature Indicator
Controller (TMP36 + Arduino). \* **CV-101:** Control Valve (Servo). \*
**P-101:** Pump (Motor DC).

------------------------------------------------------------------------

## 🛠 Komponen & Hardware

Simulasi ini dibuat menggunakan **Autodesk Tinkercad** dengan komponen
berikut:

-   **Mikrokontroler:** Arduino Uno R3 (ATmega328P)
-   **Input (Sensor):**
    -   Sensor Level: Ultrasonic HC-SR04
    -   Sensor Suhu: TMP36
    -   Sensor Tekanan: Potensiometer 10kΩ (Simulasi Transduser)
-   **Output (Aktuator):**
    -   Motor DC (Pompa Air) + Driver L293D/Transistor
    -   Micro Servo SG90 (Katup Pembuangan)
    -   Piezo Buzzer & LED Merah (Indikator Alarm)
-   **Display:** LCD 16x2 (I2C Interface)

------------------------------------------------------------------------

## 🔌 Wiring & Pin Mapping

Berikut adalah konfigurasi pin yang digunakan dalam kode program:

| Komponen                    | Pin Arduino | Tipe Sinyal      |
|-----------------------------|-------------|------------------|
| **TMP36 (Suhu)**            | A0          | Analog Input     |
| **Potensiometer (Tekanan)** | A1          | Analog Input     |
| **Servo Valve**             | D3          | PWM Output       |
| **Pompa (Motor DC)**        | D4          | Digital Output   |
| **Ultrasonic Trig**         | D5          | Digital Output   |
| **Ultrasonic Echo**         | D6          | Digital Input    |
| **LED Alarm**               | D7          | Digital Output   |
| **Buzzer**                  | D9          | Digital Output   |
| **LCD SDA**                 | A4          | I2C Data         |
| **LCD SCL**                 | A5          | I2C Clock        |


------------------------------------------------------------------------

## 🧠 Logika Kontrol

Sistem bekerja berdasarkan prinsip *Closed Loop Feedback*. Berikut adalah alur diagram blok sistem:

![Block Diagram](System-Block-Diagram.png)
*Gambar 1. Diagram Blok Input-Proses-Output*

### 1. Pengisian Air (Hysteresis Loop)

Logika ini mencegah pompa hidup-mati terlalu cepat.

``` cpp
// Logic Pseudo-code
if (LevelAir <= 20%) {
    Pompa = ON; // Mulai mengisi
} else if (LevelAir >= 80%) {
    Pompa = OFF; // Stop mengisi
}
// Di antara 20% - 80%, pompa mempertahankan status terakhirnya.
```

2.  Pengaman Tekanan (Proportional Logic) C++

```{=html}
<!-- -->
```
    if (Tekanan > 80%) {
        Servo.write(90); // Buka Katup (Venting)
    } else {
        Servo.write(0);  // Tutup Katup (Normal)
    }

3.  Safety Interlock (OR Logic) Alarm aktif jika salah satu kondisi
    bahaya terpenuhi. C++

```{=html}
<!-- -->
```
    if (Suhu > 40 || Tekanan > 80) {
        Alarm = ON; // Buzzer Bunyi + LED Nyala
    } else {
        Alarm = OFF;
    }

## 💻 Instalasi & Simulasi

Clone Repository ini: Bash

    git clone https://github.com/username-kamu/repo-name.git

Buka Kode: Buka file main.ino (atau nama file kodemu) menggunakan
Arduino IDE.

Library yang Dibutuhkan: Pastikan kamu telah menginstall library berikut
di Arduino IDE:

-   LiquidCrystal_I2C\
-   Servo

Simulasi: - Copy kode program.\
- Buka Tinkercad Circuits.\
- Buat rangkaian sesuai skema.\
- Paste kode dan klik Start Simulation.

------------------------------------------------------------------------

## 👤 Author

M Yusuf Aditiya

Mahasiswa Teknik Robotika & Instrumentasi\
NIM: 125490072

Dibuat untuk keperluan Tugas Kuliah Semester 1 - 2025
