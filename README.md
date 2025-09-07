# ESP32 LED MQTT Controller

A complete IoT solution for controlling an LED on an ESP32 microcontroller via MQTT protocol with a beautiful web interface.

## 🌟 Features

- **Remote LED Control**: Turn ON, OFF, or toggle LED via web interface
- **MQTT Communication**: Uses HiveMQ public broker for reliable messaging
- **Real-time Status**: Live LED state updates and connection monitoring
- **Mobile Friendly**: Responsive web interface works on all devices
- **Auto-reconnection**: Automatic WiFi and MQTT reconnection
- **Debug Logging**: Comprehensive serial output for troubleshooting
- **GitHub Pages Ready**: Deploy web interface directly from GitHub

## 🚀 Quick Start

### 1. Hardware Setup
- ESP32 development board
- Built-in LED (or external LED on GPIO 2)
- WiFi connection

### 2. Software Setup
1. **Upload ESP32 Code**:
   - Open `WiFiLed_MQTT.ino` in Arduino IDE
   - Install required libraries: `PubSubClient`
   - Update WiFi credentials in the code
   - Upload to your ESP32

2. **Get Device ID**:
   - Open Serial Monitor (115200 baud)
   - Look for: `Device ID: [YOUR_DEVICE_ID]`
   - Copy the 12-character ID

3. **Access Web Interface**:
   - Open `index.html` in your browser (local)
   - Or visit your GitHub Pages URL (deployed)
   - Enter your Device ID and click "Watch"
   - Control your LED with the buttons!

## 📁 Project Structure

```
WiFiLed_MQTT/
├── WiFiLed_MQTT.ino    # ESP32 Arduino code
├── index.html          # Web interface
├── README.md           # This file
└── DEPLOYMENT_GUIDE.md # Detailed deployment instructions
```

## 🔧 Configuration

### ESP32 Code Configuration
```cpp
// Update these with your WiFi credentials
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";

// MQTT Broker (default: HiveMQ public broker)
const char* MQTT_BROKER = "broker.hivemq.com";
const int   MQTT_PORT   = 1883;

// LED Pin (default: GPIO 2 - built-in LED)
const int LED_PIN = 2;
```

### Web Interface Configuration
- **Default Broker**: `wss://broker.hivemq.com:8884/mqtt`
- **Topic Structure**: `maq/led/{DEVICE_ID}/set` and `maq/led/{DEVICE_ID}/state`
- **Commands**: `ON`, `OFF`, `TOGGLE`

## 🌐 MQTT Topics

| Topic | Direction | Purpose | Example |
|-------|-----------|---------|---------|
| `maq/led/{DEVICE_ID}/set` | ESP32 ← Web | Commands to ESP32 | `ON`, `OFF`, `TOGGLE` |
| `maq/led/{DEVICE_ID}/state` | ESP32 → Web | LED state updates | `ON`, `OFF` |
| `maq/led/{DEVICE_ID}/lwt` | ESP32 → Web | Connection status | `online`, `offline` |

## 🧪 Testing

### Test MQTT Commands
```bash
# Install mosquitto tools
brew install mosquitto  # macOS
# or
sudo apt-get install mosquitto-clients  # Ubuntu

# Test LED control (replace DEVICE_ID)
mosquitto_pub -h broker.hivemq.com -p 1883 -t "maq/led/DEVICE_ID/set" -m "ON"
mosquitto_pub -h broker.hivemq.com -p 1883 -t "maq/led/DEVICE_ID/set" -m "TOGGLE"
mosquitto_pub -h broker.hivemq.com -p 1883 -t "maq/led/DEVICE_ID/set" -m "OFF"
```

### Monitor MQTT Messages
```bash
# Subscribe to LED state
mosquitto_sub -h broker.hivemq.com -p 1883 -t "maq/led/DEVICE_ID/state"

# Subscribe to all device messages
mosquitto_sub -h broker.hivemq.com -p 1883 -t "maq/led/DEVICE_ID/+"
```

## 📱 Web Interface Features

- **Connection Status**: Real-time MQTT broker connection indicator
- **LED State Display**: Visual LED status with color-coded indicators
- **Control Buttons**: Turn ON, Turn OFF, and Toggle LED
- **Device ID Input**: Easy device identification
- **Settings Panel**: Configurable broker URL and credentials
- **Activity Log**: Real-time message logging
- **Local Storage**: Remembers your settings
- **Responsive Design**: Works on desktop, tablet, and mobile

## 🔍 Troubleshooting

### Common Issues

1. **LED Not Responding**:
   - Check Serial Monitor for error messages
   - Verify Device ID matches exactly
   - Try different LED pins (5, 18, 19, 21, 22, 23)

2. **WiFi Connection Failed**:
   - Verify SSID and password
   - Check WiFi signal strength
   - Ensure 2.4GHz network (ESP32 doesn't support 5GHz)

3. **MQTT Connection Failed**:
   - Check internet connection
   - Verify broker URL: `broker.hivemq.com:1883`
   - Check firewall settings

4. **Web Interface Issues**:
   - Check browser console (F12) for errors
   - Verify broker WebSocket URL: `wss://broker.hivemq.com:8884/mqtt`
   - Try different browser or incognito mode

### Debug Information

The ESP32 code provides detailed serial output:
```
--- MQTT LED CONTROLLER BOOT ---
Device ID: A1B2C3D4E5F6
LED Pin: 2
MQTT Broker: broker.hivemq.com:1883
Testing LED...
LED should be ON now
LED should be OFF now
WiFi connected. IP: 192.168.1.100
MQTT broker connected
Successfully subscribed to set topic
Published initial state: OFF
```

## 🚀 Deployment

### GitHub Pages Deployment
1. Upload files to GitHub repository
2. Enable GitHub Pages in repository settings
3. Select "Deploy from a branch" → "main" → "/ (root)"
4. Access your web interface at: `https://[username].github.io/[repository]/`

### Local Development
- Open `index.html` directly in your browser
- No server required - works with file:// protocol

## 🔒 Security Considerations

- **Public Broker**: Currently uses HiveMQ public broker (no authentication)
- **Device ID**: Acts as a simple access control mechanism
- **For Production**: Consider using private MQTT broker with authentication

## 📚 Dependencies

### ESP32 Libraries
- `WiFi` (built-in)
- `PubSubClient` (install via Arduino Library Manager)

### Web Interface
- `mqtt.js` (loaded from CDN)
- No additional dependencies required

## 🛠️ Hardware Compatibility

- **ESP32**: All variants (ESP32, ESP32-S2, ESP32-S3, ESP32-C3)
- **LED Pins**: GPIO 2 (built-in), or any GPIO pin
- **WiFi**: 2.4GHz networks only

## 📄 License

This project is open source and available under the MIT License.

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## 📞 Support

If you encounter any issues:
1. Check the troubleshooting section above
2. Review the Serial Monitor output
3. Check the browser console for web interface errors
4. Open an issue in this repository

## 🎯 Success Indicators

✅ **ESP32**: Serial Monitor shows "MQTT broker connected"  
✅ **Web Interface**: Shows "connected" status  
✅ **LED Control**: Buttons respond and LED toggles  
✅ **GitHub Pages**: Web interface accessible via URL  

---

**Happy IoT Hacking!** 🚀✨
