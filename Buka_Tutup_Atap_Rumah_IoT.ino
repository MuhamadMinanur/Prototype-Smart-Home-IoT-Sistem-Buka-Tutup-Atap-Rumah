#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6emT8B2vB" //sesuaikan pada blynk
#define BLYNK_TEMPLATE_NAME "YU" //sesuaikan pada blynk
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char auth[] = "xq2qmMQ2hHscHQ-V37z0w3DDXEfKdETS"; // sesuaikan pada blynk kalian
char ssid[] = "PUNK"; //sesuaikan nama wifi kalian
char pass[] = "ibnuganteng"; // sesuaikan password kalian

const int led1 = 18;  // Ganti dengan pin yang sesuai
const int led2 = 5;   // Ganti dengan pin yang sesuai
const int rainSensorPin = 34;

int prevRainValue = -1;  // Inisialisasi dengan nilai yang tidak mungkin untuk sensor hujan
unsigned long motorStartTime = 0;
const long motorDuration = 1000;  // Waktu motor bergerak dalam milidetik (5 detik)
volatile int motorSpeed = 50;  // Kecepatan motor, nilai antara 0 (mati) dan 255 (penuh)

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(rainSensorPin, INPUT);
}

void putar_motor_kanan() {
  digitalWrite(led1, motorSpeed );
  digitalWrite(led2, LOW);
}

void putar_motor_kiri() {
  digitalWrite(led1, LOW);
  digitalWrite(led2, motorSpeed );
}

void stop_motor() {
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
}

BLYNK_WRITE(V0) {
  motorSpeed = param.asInt();  // Mendapatkan nilai kecepatan dari Slider V0
  putar_motor_kanan();
  delay(500);  // Hindari penggunaan delay di sini
  stop_motor();
}

BLYNK_WRITE(V1) {
  motorSpeed = param.asInt();  // Mendapatkan nilai kecepatan dari Slider V1
  putar_motor_kiri();
  delay(500);  // Hindari penggunaan delay di sini
  stop_motor();
}


void loop() {
  Blynk.run();
  readRainSensor();
}

void readRainSensor() {
  int rainValue = digitalRead(rainSensorPin);
  Blynk.virtualWrite(V2, map(rainValue, HIGH, LOW, 0, 100));

  if (prevRainValue == -1) {
    prevRainValue = rainValue;
    return;
  }

  if (rainValue != prevRainValue) {
    if (rainValue == HIGH) {
      putar_motor_kanan();
      motorStartTime = millis();
      Serial.println("Hujan terdeteksi. Motor bergerak ke kanan.");
    } else {
      putar_motor_kiri();
      motorStartTime = millis();
      Serial.println("Tidak ada hujan. Motor bergerak ke kiri.");
    }
  }

  // Periksa apakah sudah waktunya mematikan motor
  if (millis() - motorStartTime >= motorDuration) {
    stop_motor();
    Serial.println(rainValue == HIGH ? "tidak hujan!" : "hujan.");
  }

  // Simpan pembacaan saat ini sebagai pembacaan sebelumnya untuk perbandingan berikutnya
  prevRainValue = rainValue;
}