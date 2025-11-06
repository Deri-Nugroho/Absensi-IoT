#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// === KONFIGURASI PIN ===
#define SS_PIN  D2
#define RST_PIN D1
#define BUZZER  D8   // GPIO15

MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// === KONFIGURASI WIFI & API ===
const char* ssid = "Infinix";
const char* password = "yttaytta";

// Ganti dengan endpoint POST API Gateway kamu
const char* apiURL = "https://YOUR_API_GATEWAY_URL/siswa";

// === SETUP ===
void setup() {
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n=== IoT Absensi AWS ===");
  Serial.println("Inisialisasi perangkat...");

  Wire.begin(D4, D3); // SDA = D4 (GPIO2), SCL = D3 (GPIO0)
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IoT Absensi AWS");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");

  // Inisialisasi WiFi
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan WiFi");
  lcd.clear();
  lcd.print("WiFi Connecting");
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    Serial.print(".");
    retry++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi Terhubung!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    lcd.clear();
    lcd.print("WiFi Connected");
    delay(1000);
  } else {
    Serial.println("\n❌ GAGAL: Tidak bisa terhubung WiFi!");
    lcd.clear();
    lcd.print("WiFi Failed!");
    while (true); // berhenti total
  }

  // Inisialisasi RFID
  SPI.begin();
  rfid.PCD_Init();
  delay(50);

  byte v = rfid.PCD_ReadRegister(MFRC522::VersionReg);
  if (v == 0x00 || v == 0xFF) {
    Serial.println("❌ GAGAL: Modul RFID tidak terdeteksi!");
    lcd.clear();
    lcd.print("RFID Not Found!");
    while (true); // stop agar mudah debug
  } else {
    Serial.println("✅ RFID OK!");
  }

  lcd.clear();
  lcd.print("Scan your card...");
}

// === LOOP ===
void loop() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  String uidString = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    uidString += String(rfid.uid.uidByte[i], HEX);
  }

  Serial.println("==============================");
  Serial.print("UID: ");
  Serial.println(uidString);
  lcd.clear();
  lcd.print("Card Detected!");
  lcd.setCursor(0, 1);
  lcd.print(uidString);

  tone(BUZZER, 3000, 400);
  delay(500);

  // === KIRIM DATA KE API ===
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, apiURL);
    http.addHeader("Content-Type", "application/json");

    // Data JSON
    String body = "{\"nama\":\"" + uidString + "\",\"pesan\":\"Telah Absen!\"}";
    int httpCode = http.POST(body);

    if (httpCode > 0) {
      Serial.printf("HTTP Response: %d\n", httpCode);
      String response = http.getString();
      Serial.println("Response Body:");
      Serial.println(response);

      if (httpCode == 200) {
        lcd.clear();
        lcd.print("✅ Absen Success");
        tone(BUZZER, 1500, 200);
      } else {
        lcd.clear();
        lcd.print("❌ Absen Failed!");
        tone(BUZZER, 1000, 200);
      }
    } else {
      Serial.println("❌ GAGAL: Tidak dapat mengirim data ke API!");
      Serial.println(http.errorToString(httpCode));
      lcd.clear();
      lcd.print("API Error!");
    }

    http.end();
  } else {
    Serial.println("❌ GAGAL: WiFi tidak aktif!");
    lcd.clear();
    lcd.print("WiFi Lost!");
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  delay(2000);
  lcd.clear();
  lcd.print("Scan next card...");
}
