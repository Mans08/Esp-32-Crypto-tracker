#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ================= WIFI =================
const char* WIFI_SSID = "Your wifi name";
const char* WIFI_PASSWORD = "your wifi password";

// ================= OLED =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C

#define OLED_SDA 5
#define OLED_SCL 4

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

// ================= PHYSICAL BUTTONS =================
// These remain available for the final version
#define BUTTON_PREV 25
#define BUTTON_NEXT 26

// ================= CRYPTOCURRENCIES =================

const char* coinIDs[] = {
  "bitcoin",
  "ethereum",
  "solana",
  "cardano"
};

const char* coinNames[] = {
  "BITCOIN",
  "ETHEREUM",
  "SOLANA",
  "CARDANO"
};

const int numberOfCoins = 4;

int currentCoin = 0;

// ================= DATA =================

float currentPrice = 0;
float change24h = 0;

unsigned long lastUpdate = 0;

const unsigned long UPDATE_INTERVAL = 30000;

// ======================================================
// SETUP
// ======================================================

void setup() {

  Serial.begin(115200);

  // OLED
  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {

    Serial.println("OLED failed!");

    while (true);
  }

  // Physical buttons
  pinMode(BUTTON_PREV, INPUT_PULLUP);
  pinMode(BUTTON_NEXT, INPUT_PULLUP);

  // Startup screen

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);

  display.setCursor(5, 10);
  display.println("CRYPTO");

  display.setCursor(5, 35);
  display.println("TICKER");

  display.display();

  delay(1500);

  // WiFi
  connectWiFi();

  // Get first crypto price
  getCryptoData();

  lastUpdate = millis();
}

// ======================================================
// MAIN LOOP
// ======================================================

void loop() {

  // ----------------------------------------------------
  // LAPTOP KEYBOARD CONTROL
  // A = PREVIOUS
  // D = NEXT
  // ----------------------------------------------------

  if (Serial.available() > 0) {

    char key = Serial.read();

    // Previous crypto
    if (key == 'a' || key == 'A') {

      currentCoin--;

      if (currentCoin < 0) {
        currentCoin = numberOfCoins - 1;
      }

      Serial.println("Previous crypto selected");

      getCryptoData();

      lastUpdate = millis();
    }

    // Next crypto
    if (key == 'd' || key == 'D') {

      currentCoin++;

      if (currentCoin >= numberOfCoins) {
        currentCoin = 0;
      }

      Serial.println("Next crypto selected");

      getCryptoData();

      lastUpdate = millis();
    }
  }

  // ----------------------------------------------------
  // PHYSICAL BUTTONS
  // These will work when you connect them later
  // ----------------------------------------------------

  if (digitalRead(BUTTON_PREV) == LOW) {

    currentCoin--;

    if (currentCoin < 0) {
      currentCoin = numberOfCoins - 1;
    }

    getCryptoData();

    lastUpdate = millis();

    delay(300);
  }

  if (digitalRead(BUTTON_NEXT) == LOW) {

    currentCoin++;

    if (currentCoin >= numberOfCoins) {
      currentCoin = 0;
    }

    getCryptoData();

    lastUpdate = millis();

    delay(300);
  }

  // ----------------------------------------------------
  // AUTOMATIC PRICE UPDATE
  // ----------------------------------------------------

  if (millis() - lastUpdate >= UPDATE_INTERVAL) {

    getCryptoData();

    lastUpdate = millis();
  }
}

// ======================================================
// WIFI CONNECTION
// ======================================================

void connectWiFi() {

  Serial.print("Connecting to WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");
  }

  Serial.println();

  Serial.println("WiFi connected!");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// ======================================================
// GET CRYPTO DATA
// ======================================================

void getCryptoData() {

  if (WiFi.status() != WL_CONNECTED) {

    Serial.println("WiFi disconnected");

    connectWiFi();
  }

  HTTPClient http;

  String url =
    "https://api.coingecko.com/api/v3/simple/price"
    "?ids=" + String(coinIDs[currentCoin]) +
    "&vs_currencies=usd"
    "&include_24hr_change=true";

  Serial.println();
  Serial.print("Requesting: ");
  Serial.println(url);

  http.begin(url);

  int httpCode = http.GET();

  if (httpCode == 200) {

    String payload = http.getString();

    Serial.println(payload);

    DynamicJsonDocument doc(2048);

    DeserializationError error =
      deserializeJson(doc, payload);

    if (!error) {

      currentPrice =
        doc[coinIDs[currentCoin]]["usd"];

      change24h =
        doc[coinIDs[currentCoin]]["usd_24h_change"];

      displayCrypto();

    } else {

      Serial.println("JSON parsing failed");

      showError("JSON ERROR");
    }

  } else {

    Serial.print("HTTP error: ");
    Serial.println(httpCode);

    showError("API ERROR");
  }

  http.end();
}

// ======================================================
// DISPLAY CRYPTO
// ======================================================

void displayCrypto() {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  // Coin name

  display.setTextSize(1);

  display.setCursor(0, 0);

  display.print(coinNames[currentCoin]);

  // Coin number

  display.setCursor(105, 0);

  display.print(currentCoin + 1);

  display.print("/");

  display.print(numberOfCoins);

  // Price

  display.setTextSize(2);

  display.setCursor(0, 14);

  if (currentPrice >= 1000) {

    display.print("$");

    display.print(currentPrice, 0);

  } else if (currentPrice >= 1) {

    display.print("$");

    display.print(currentPrice, 2);

  } else {

    display.print("$");

    display.print(currentPrice, 4);
  }

  // 24 hour change

  display.setTextSize(1);

  display.setCursor(0, 39);

  display.print("24H: ");

  if (change24h >= 0) {

    display.print("+");
  }

  display.print(change24h, 2);

  display.print("%");

  // Movement indicator

  display.setCursor(75, 39);

  if (abs(change24h) < 1.0) {

    display.print("LOW");

  } else if (abs(change24h) < 3.0) {

    display.print("MED");

  } else {

    display.print("HIGH");
  }

  // Controls

  display.setCursor(0, 55);

  display.print("< A");

  display.setCursor(92, 55);

  display.print("D >");

  display.display();

  // Serial output

  Serial.println("--------------------");

  Serial.print("Coin: ");

  Serial.println(coinNames[currentCoin]);

  Serial.print("Price: $");

  Serial.println(currentPrice);

  Serial.print("24h change: ");

  Serial.print(change24h);

  Serial.println("%");

  Serial.println("Press A = previous");

  Serial.println("Press D = next");
}

// ======================================================
// ERROR DISPLAY
// ======================================================

void showError(const char* message) {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);

  display.setCursor(0, 10);

  display.println("CRYPTO TICKER");

  display.setTextSize(2);

  display.setCursor(0, 30);

  display.println(message);

  display.display();
}