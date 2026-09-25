# ESP32 Crypto Ticker

An ESP32-based cryptocurrency ticker that retrieves live cryptocurrency data from an online API and displays it on a 0.96-inch OLED display.

## Features

* Displays cryptocurrency prices in USD
* Displays 24-hour price change
* Supports multiple cryptocurrencies
* Navigate between cryptocurrencies using physical buttons
* Serial Monitor keyboard control for testing
* Automatically refreshes data every 30 seconds
* Uses Wi-Fi to retrieve online data
* Displays a simple LOW / MED / HIGH price-movement indicator

## Cryptocurrencies

The current version supports:

* Bitcoin
* Ethereum
* Solana
* Cardano

## Hardware

* ESP32 development board
* 0.96-inch OLED display
* Breadboard

## Wiring

### OLED Display

| OLED | ESP32  |
| ---- | ------ |
| VCC  | 3.3V   |
| GND  | GND    |
| SDA  | GPIO 5 |
| SCL  | GPIO 4 |

Software
-------------------------------------
* Arduino IDE
* ESP32 Arduino Core
* C++
* Wi-Fi
* HTTP
* JSON
* I²C

## Libraries

The project uses:

* `WiFi.h`
* `HTTPClient.h`
* `ArduinoJson.h`
* `Wire.h`
* `Adafruit_GFX.h`
* `Adafruit_SSD1306.h`

 How It Works
--------------------------------------------------------------------
The ESP32 connects to a Wi-Fi network and sends an HTTP request to the cryptocurrency API.

The API returns cryptocurrency information in JSON format.

The ESP32 then:

1. Receives the API response.
2. Parses the JSON data.
3. Extracts the current price and 24-hour percentage change.
4. Displays the information on the OLED.
5. Refreshes the information periodically.

To switch between the cryptocurrency :

* `A` → Previous cryptocurrency
* `D` → Next cryptocurrency

These keys can be sent through the Arduino IDE Serial Monitor.

 Price Movement Indicator
---------------------------------
The current prototype uses the 24-hour percentage change to provide a simple movement indicator:

* Below 1% → LOW
* 1% to below 3% → MED
* 3% or higher → HIGH

These thresholds are simple prototype thresholds and are not intended to represent a formal financial volatility model.

A future version could calculate volatility using a rolling standard deviation of price returns .

Why I Built This
------------------------------
I built this to understand and learn more about iot and embedded while i faced many challenges i learnt a lot .It combines embedded programming, networking, APIs, JSON processing, and hardware interaction in one project.

## Future Improvements

* Add more cryptocurrencies
* Add configurable refresh intervals
* Improve button debouncing
* Add historical price information
* Implement statistically based volatility calculation
* Add graphical price trends
* Store API credentials securely
* Add more display pages

 Project Status
------------------------------
Completed prototype with Wi-Fi data retrieval, OLED display, multiple cryptocurrency selection and serial control.
