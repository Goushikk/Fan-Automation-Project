# 💡 Fan Automation with ESP8266 (IR & Wi-Fi Control)

## Table of Contents

* [🌟 Overview](#-overview)
* [✨ Features](#-features)
* [🛠️ Hardware Requirements](#️-hardware-requirements)
* [🔌 Wiring Diagram](#-wiring-diagram)
* [💻 Software Requirements](#-software-requirements)
* [🚀 Getting Started](#-getting-started)
    * [1. Install Arduino IDE & ESP8266 Board Support](#1-install-arduino-ide--esp8266-board-support)
    * [2. Install Libraries](#2-install-libraries)
    * [3. Wire Up the Components](#3-wire-up-the-components)
    * [4. Customize & Upload Code](#4-customize-%-upload-code)
    * [5. Identify IR Remote Code](#5-identify-ir-remote-code)
    * [6. Final Code & Upload](#6-final-code--upload)
* [🖥️ Usage](#️-usage)
* [⚠️ Safety Notes](#️-safety-notes)
* [🤝 Contributing](#-contributing)
* [📄 License](#-license)
* [⭐ Acknowledgments](#-acknowledgments)

---

## 🌟 Overview

This project demonstrates how to make a traditional, non-Wi-Fi fan (or any simple ON/OFF appliance like a lamp) "smart" using an ESP8266 NodeMCU microcontroller. It allows you to control the appliance using an old, unused IR remote control (like from a TV, DVD player, or set-top box) and also provides a basic web interface for control via Wi-Fi from any smartphone or computer on the same network.

This is an excellent entry-level IoT project for those starting with microcontrollers, especially when on a budget, repurposing existing components.

## ✨ Features

* **IR Remote Control:** Use any button from an old IR remote (TV, DVD, Set-top Box) to toggle the fan/lamp ON/OFF.
* **Wi-Fi Web Control:** Control the fan/lamp from any device connected to the same Wi-Fi network via a simple web page.
* **Cost-Effective:** Built with readily available and inexpensive components, leveraging existing "junk" remotes.
* **Scalable:** The foundation for more complex home automation projects.

## 🛠️ Hardware Requirements

* **ESP8266 NodeMCU Development Board** (e.g., ESP-12E/F)
* **TSOP1738 IR Receiver** (or compatible 3-pin IR receiver)
* **1-Channel 5V Relay Module**
* **Jumper Wires** (Male-to-Female recommended)
* **Mini Breadboard** (Highly recommended for easy prototyping)
* **Micro-USB cable** (for powering and programming the NodeMCU)
* **5V Phone Charger / Power Bank** (for powering the NodeMCU after programming)
* **Your non-Wi-Fi Retro Fan / Lamp**
* **An old IR Remote** (from TV, DVD player, Set-top box, etc.)

## 🔌 Wiring Diagram

**(You'll want to add an image here later. For now, text description is fine.)**

A simple Fritzing diagram or a hand-drawn sketch (photographed) would be very helpful.

* **IR Receiver (TSOP1738) to NodeMCU:**
    * TSOP1738 OUT (Signal) $\rightarrow$ NodeMCU **D2 (GPIO4)**
    * TSOP1738 GND $\rightarrow$ NodeMCU **GND**
    * TSOP1738 VCC $\rightarrow$ NodeMCU **3V3** (or 5V, check your TSOP datasheet for ideal voltage)
* **Relay Module to NodeMCU:**
    * Relay IN (Signal) $\rightarrow$ NodeMCU **D1 (GPIO5)**
    * Relay VCC $\rightarrow$ NodeMCU **VIN / 5V**
    * Relay GND $\rightarrow$ NodeMCU **GND**
* **Fan/Lamp to Relay (DANGER - AC Mains!):**
    * **Ensure the fan/lamp is UNPLUGGED from the wall.**
    * Cut **one** of the two wires from the fan/lamp's power cord.
    * Connect one end of the cut wire to the **COM** (Common) terminal of the relay.
    * Connect the other end of the cut wire to the **NO** (Normally Open) terminal of the relay.
    * When the relay is activated by the NodeMCU, it will close the circuit and turn on the fan/lamp.

## 💻 Software Requirements

* **Arduino IDE** (Download from [arduino.cc](https://www.arduino.cc/en/software))
* **ESP8266 Board Package** for Arduino IDE
* **Libraries (via Arduino Library Manager):**
    * `IRremoteESP8266`
    * `ESPAsyncWebServer`
    * `ESPAsyncTCP` (dependency for ESPAsyncWebServer)

## 🚀 Getting Started

Follow these steps to get your Retro Fan Automation up and running:

### 1. Install Arduino IDE & ESP8266 Board Support

1.  Download and install the [Arduino IDE](https://www.arduino.cc/en/software).
2.  Open Arduino IDE. Go to `File > Preferences`.
3.  In "Additional Boards Manager URLs", add:
    ```
    [http://arduino.esp8266.com/stable/package_esp8266com_index.json](http://arduino.esp8266.com/stable/package_esp8266com_index.json)
    ```
4.  Go to `Tools > Board > Boards Manager...`. Search for "esp8266" and install the "ESP8266 by ESP8266 Community" package.
5.  Select your board: `Tools > Board > ESP8266 Boards > NodeMCU 1.0 (ESP-12E Module)`.

### 2. Install Libraries

1.  In Arduino IDE, go to `Sketch > Include Library > Manage Libraries...`.
2.  Search for and install the following libraries:
    * `IRremoteESP8266`
    * `ESPAsyncWebServer`
    * `ESPAsyncTCP` (This will usually be installed automatically when you install `ESPAsyncWebServer`, but double-check).

### 3. Wire Up the Components

Refer to the [Wiring Diagram](#-wiring-diagram) section above to connect your ESP8266, IR receiver, and relay module. **Remember the safety precautions for AC wiring!**

### 4. Customize & Upload Code (Initial Upload)

1.  Open the provided Arduino sketch (`.ino` file) in the Arduino IDE.
2.  **Crucially, update your Wi-Fi credentials:**
    ```cpp
    const char* ssid = "YOUR_WIFI_SSID";         // REPLACE with your WiFi network name
    const char* password = "YOUR_WIFI_PASSWORD"; // REPLACE with your WiFi password
    ```
3.  Connect your NodeMCU to your computer via the micro-USB cable.
4.  Select the correct COM port: `Tools > Port`.
5.  Upload the code to your NodeMCU.

### 5. Identify IR Remote Code

1.  After the initial upload, open the `Tools > Serial Monitor` in the Arduino IDE.
2.  Set the baud rate to **115200**.
3.  Point your old IR remote at the TSOP1738 IR receiver and press the button you want to use for toggling the fan/lamp (e.g., "Power").
4.  Observe the output in the Serial Monitor. You will see lines like:
    ```
    Received IR code: 0xFFA25D Protocol: NEC
    ```
    **Note down the hexadecimal code (e.g., `0xFFA25D`).** This is the unique code for that button on your remote.

### 6. Final Code & Upload

1.  In the `loop()` function of your Arduino sketch, find this section:
    ```cpp
    // --- YOUR CUSTOM IR CODE HERE ---
    // ...
    if (results.value == 0xFFA25D || results.value == 0xFFFFFFFF) { 
        toggleFan();
    }
    ```
2.  **Replace `0xFFA25D` with the actual hexadecimal code you noted from your remote.** (The `0xFFFFFFFF` part often indicates a "repeat" signal; you can keep it for reliability or remove it if it causes unintended toggles).
3.  **Re-upload the modified code** to your NodeMCU.

## 🖥️ Usage

Once the code is uploaded and running:

* **IR Remote Control:** Point your old, configured IR remote at the IR receiver. Press the button you assigned, and your fan/lamp should toggle ON/OFF.
* **Wi-Fi Web Control:**
    1.  Open the `Serial Monitor` to find the IP Address assigned to your ESP8266 (e.g., `IP Address: 192.168.1.102`).
    2.  On any smartphone or computer connected to the *same Wi-Fi network*, open a web browser.
    3.  Enter the IP address (e.g., `http://192.168.1.102/`) into the address bar.
    4.  You will see a simple web page with a "Toggle Fan" button. Click it to control your fan/lamp. The status on the page will update.

## ⚠️ Safety Notes

* **AC Mains Voltage is Dangerous:** The relay module interfaces with high-voltage AC electricity (230V in India). **Always unplug the appliance from the wall before making or changing any wiring connections to the relay.**
* **Insulate Connections:** Ensure all connections, especially those handling mains power, are properly insulated with electrical tape or heat shrink tubing to prevent accidental shorts or shocks.
* **Start Simple:** If you are new to electronics, it's highly recommended to first test the relay with a low-voltage DC circuit (e.g., an LED connected to a battery pack) before moving to AC appliances.
* **Seek Help:** If you are ever unsure about wiring or safety, consult someone experienced in electrical work.

## 🤝 Contributing

Contributions, issues, and feature requests are welcome! Feel free to open an issue or submit a pull request on this GitHub repository.

## 📄 License

This project is open-source and released under the [MIT License](https://www.google.com/search?q=LICENSE).

## ⭐ Acknowledgments

* The Arduino and ESP8266 open-source communities for providing excellent tools and libraries.
* The developers of the `IRremoteESP8266`, `ESPAsyncWebServer`, and `ESPAsyncTCP` libraries.
* Your ingenuity for making things "automative" on a budget!
