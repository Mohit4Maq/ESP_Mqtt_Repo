# ESP32 LED MQTT Controller - Deployment Guide

## ✅ Test Results Summary

**Local Testing**: ✅ LED toggle works on local server  
**Public MQTT Broker**: ✅ broker.hivemq.com is accessible  
**Web Interface**: ✅ Ready for deployment  

## 🚀 GitHub Pages Deployment

### Step 1: Upload Files to GitHub
1. Upload both files to your GitHub repository:
   - `WiFiLed_MQTT.ino` (ESP32 code)
   - `index.html` (Web interface)

### Step 2: Enable GitHub Pages
1. Go to your repository settings
2. Scroll to "Pages" section
3. Select "Deploy from a branch"
4. Choose "main" branch and "/ (root)" folder
5. Click "Save"

### Step 3: Access Your Web Interface
Your web interface will be available at:
```
https://[your-username].github.io/[repository-name]/
```

## 🔧 ESP32 Setup Instructions

### 1. Update WiFi Credentials
In `WiFiLed_MQTT.ino`, update these lines:
```cpp
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";
```

### 2. Upload Code to ESP32
1. Open Arduino IDE
2. Install required libraries:
   - WiFi (built-in)
   - PubSubClient
3. Select your ESP32 board
4. Upload the code

### 3. Get Device ID
1. Open Serial Monitor (115200 baud)
2. Look for: `Device ID: [YOUR_DEVICE_ID]`
3. Copy this 12-character ID

## 🌐 Web Interface Usage

### 1. Open Web Interface
- Local: Open `index.html` in browser
- GitHub Pages: Visit your GitHub Pages URL

### 2. Configure Connection
- **Device ID**: Enter your ESP32's Device ID
- **Broker URL**: `wss://broker.hivemq.com:8884/mqtt` (default)
- Click "Watch" button

### 3. Control LED
- **Turn ON**: Click "Turn ON" button
- **Turn OFF**: Click "Turn OFF" button  
- **Toggle**: Click "Toggle" button

## 🧪 Testing Commands

### Test MQTT Broker Connection
```bash
# Test basic connectivity
mosquitto_pub -h broker.hivemq.com -p 1883 -t "test/connection" -m "Hello"

# Test LED commands (replace DEVICE_ID with your actual ID)
mosquitto_pub -h broker.hivemq.com -p 1883 -t "maq/led/DEVICE_ID/set" -m "ON"
mosquitto_pub -h broker.hivemq.com -p 1883 -t "maq/led/DEVICE_ID/set" -m "TOGGLE"
mosquitto_pub -h broker.hivemq.com -p 1883 -t "maq/led/DEVICE_ID/set" -m "OFF"
```

### Monitor MQTT Messages
```bash
# Subscribe to state topic
mosquitto_sub -h broker.hivemq.com -p 1883 -t "maq/led/DEVICE_ID/state"

# Subscribe to all device messages
mosquitto_sub -h broker.hivemq.com -p 1883 -t "maq/led/DEVICE_ID/+"
```

## 🔍 Troubleshooting

### ESP32 Issues
1. **LED not responding**: Check Serial Monitor for error messages
2. **WiFi connection failed**: Verify SSID and password
3. **MQTT connection failed**: Check internet connection

### Web Interface Issues
1. **"Not connected"**: Check broker URL and internet connection
2. **No response from ESP32**: Verify Device ID matches exactly
3. **Browser console errors**: Press F12 to check for JavaScript errors

### Common Solutions
- **LED pin issues**: Try changing `LED_PIN` to `5`, `18`, `19`, `21`, `22`, or `23`
- **Inverted LED logic**: Some boards use `LOW` for ON, `HIGH` for OFF
- **Firewall issues**: Some networks block MQTT ports

## 📱 Mobile Access

The web interface is mobile-friendly and works on:
- Smartphones
- Tablets  
- Any device with a modern web browser

## 🔒 Security Notes

- The current setup uses a public MQTT broker (no authentication)
- For production use, consider:
  - Using a private MQTT broker
  - Adding username/password authentication
  - Using TLS/SSL encryption

## 📊 Expected Serial Output

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

## 🎯 Success Indicators

✅ **ESP32**: Serial Monitor shows "MQTT broker connected"  
✅ **Web Interface**: Shows "connected" status  
✅ **LED Control**: Buttons respond and LED toggles  
✅ **GitHub Pages**: Web interface accessible via URL  

Your ESP32 LED MQTT controller is now ready for deployment! 🎉
