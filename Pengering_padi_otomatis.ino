#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h> 
#include <PID_v1.h>
#include <dimmable_light.h>


/* Definisi pin */
#define DIMMER_PIN 9
#define RELAY1_PIN 5
#define LDR_PIN A0
#define SOIL_MOISTURE_PIN A1
#define TEMP_PIN 2 

/* Konfigurasi objek perangkat */
RTC_DS3231 rtc;
OneWire oneWire(TEMP_PIN);
DallasTemperature tempSensor(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2);

/* Konfigurasi pin dimmer */
const int syncPin = 3;
const int thyristorPin = 9;
DimmableLight light(thyristorPin);

/* Konfigurasi PID untuk pengontrolan suhu */
double Ku = 2.5 , Tu = 2.0;
double Kp = 0.6 * Ku;
double Ki = 1.2 * Ku / Tu;
double Kd = 0.075 * Ku * Tu;
double Setpoint, tempInput, tempOutput;
PID tempPID(&tempInput, &tempOutput, &Setpoint, Kp, Ki, Kd, DIRECT);

/* Variabel waktu */
unsigned long previousSerialTime = 0;
unsigned long previousLCDTime = 0;

void setup() {
  Serial.begin(9600);
  delay(1000); // Beri waktu untuk inisialisasi Serial Monitor

  lcd.begin(16, 2);
  lcd.backlight();
  tempSensor.begin();

  if (!rtc.begin()) {
    Serial.println("RTC tidak ditemukan");
    lcd.print("RTC Error");
  }

  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(SOIL_MOISTURE_PIN, INPUT);
  digitalWrite(RELAY1_PIN, LOW);

  /* Inisialisasi PID */
  Setpoint = 50.0;
  tempPID.SetMode(AUTOMATIC);
  tempPID.SetOutputLimits(0, 255);

  /* Inisialisasi dimmer */
  DimmableLight::setSyncPin(syncPin);
  DimmableLight::begin();

  Serial.println("Sistem Siap!");
}

void loop() {
  unsigned long currentMillis = millis();

  int ldrValue = analogRead(LDR_PIN);
  int soilMoisture = analogRead(SOIL_MOISTURE_PIN);
  tempSensor.requestTemperatures();
  tempInput = tempSensor.getTempCByIndex(0);

  /* Kontrol mode berdasarkan nilai LDR */
  controlMode(ldrValue);

  /* Periksa kelembaban tanah */
  checkSoilMoisture(soilMoisture);

  /* Update Serial Monitor setiap 1 detik */
  if (currentMillis - previousSerialTime >= 1000) {
    previousSerialTime = currentMillis;
    printSerial(ldrValue, soilMoisture, tempOutput);
  }

  /* Update LCD setiap 1 detik */
  if (currentMillis - previousLCDTime >= 1000) {
    previousLCDTime = currentMillis;
    updateLCD(tempInput, tempOutput);
  }
}

/* Fungsi untuk mengontrol mode berdasarkan nilai LDR */
void controlMode(int ldrValue) {
  if (ldrValue > 300) {
    tempPID.Compute();
    light.setBrightness(tempOutput);
    Serial.println("Mode Malam - PID aktif");
  } else if (ldrValue < 200) {
    digitalWrite(RELAY1_PIN, HIGH);
    Serial.println("Mode Siang - Relay ON");
  } else {
    digitalWrite(RELAY1_PIN, LOW);
  }
}

/* Fungsi untuk memperbarui tampilan LCD */
void updateLCD(double tempInput, double tempOutput) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(tempInput);
  lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("PWM: ");
  lcd.print(tempOutput);
}

/* Fungsi untuk mencetak data ke Serial Monitor */
void printSerial(int ldrValue, int soilMoisture, double tempOutput) {
  Serial.print("PWM: ");
  Serial.print(tempOutput);
  Serial.print(" | LDR: ");
  Serial.print(ldrValue);
  Serial.print(" | Soil: ");
  Serial.println(soilMoisture);
}

/* Fungsi untuk memeriksa kelembaban tanah */
void checkSoilMoisture(int soilMoisture) {
  if (soilMoisture >= 950) {
    Serial.println("Tanah terlalu basah, sistem berhenti.");
    while (true) {
      Serial.println("Sistem berhenti. Cek kelembaban.");
      delay(2000); // Beri waktu agar Serial tetap bisa mengirimkan data
    }
  }
}
