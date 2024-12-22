# PromUDP 👉🌞

**Bridge UDP metrics from IoT/Embedded systems to Prometheus Standard**  

PromUDP is a lightweight UDP proxy designed to help limited/constrained devices—like IoT sensors or embedded systems interface with Prometheus. 

It is excellent for low-resource IoT/Embedded devices where an HTTP server is not practical or even possible. If TCP is not available, if latency matters more than delivery, or if maintaining a TCP Connection will drain its batteries.

---

## ✨ Why PromUDP?  
- **Embedded Systems** 🛠️: Devices like ESP8266, ESP32, and Raspberry Pi Zero that prioritize minimal power and resource usage.  
- **Battery-Powered IoT Devices** 🔋: Sensors with strict energy constraints where UDP is possibly all you got.  
- **No HTTP Overhead** 🌐: PromUDP bridges UDP and HTTP, letting devices skip the overhead of hosting an HTTP server.

---

## 🔧 Features
- **💡 Lightweight Design**: KISS methodology, a simple UDP endpoint and a Minimal HTTP Server.  
- **🔄 Easy Integration**: UDP in - Prometheus Out.  
---

## 🚀 Example Use Cases
- 🏠 **Smart Home Devices**: Battery-powered temperature or humidity sensors transmitting metrics to Prometheus.
- 🌡️ **Industrial IoT**: Lightweight pressure and flow sensors in pipelines.  
- 📡 **Remote Monitoring**: Satellite-connected embedded systems transmitting sparse but critical data.  
- 🌲 **Environmental Monitoring**: Devices in forests or remote areas collecting data for climate analysis.  

---

## 🛠️ Getting Started

### 1️⃣ Installation
Clone the repository and build the project:
```bash
git clone https://github.com/layer07/PromUDP.git
cd PromUDP
make
```

### 2️⃣ Running the Server
Run PromUDP with default or custom parameters:
```bash
./PromUDP --port 8080 --udp 9999 --verbose
```

PromUDP listens as an `HTTP server` on port `8080` and a UDP server on port `9999`. Point your Prometheus scraper to the `HTTP` endpoint at `localhost:8080`, and offload your IoT/Embedded device's metrics to `UDP` port `9999`.  PromUDP will automatically update its `HTTP` server with the received `UDP data`, so Prometheus can crawl and grab the latest metrics.



### 3️⃣ Sending Metrics
Send UDP metrics using a compatible device or script:
```bash
echo "temperature,room42,22.50,1671623858000" | nc -u -w1 127.0.0.1 9999
echo "humidity,room42,55.30,1671623900000" | nc -u -w1 127.0.0.1 9999
echo "pressure,room42,1013.20,1671623920000" | nc -u -w1 127.0.0.1 9999
echo "temperature,room43,24.00,1671624000000" | nc -u -w1 127.0.0.1 9999
```

### 4️⃣ Verifying Metrics with `curl`
You can verify the received metrics by querying the HTTP endpoint directly:
```bash
curl http://localhost:8080/metrics
```

**Output**:
```text
temperature{tag="room42"} 22.50 1671623858000
humidity{tag="room42"} 55.30 1671623900000
pressure{tag="room42"} 1013.20 1671623920000
temperature{tag="room43"} 24.00 1671624000000
```

### 5️⃣ Scraping Metrics
Configure Prometheus to scrape metrics:
```yaml
scrape_configs:
  - job_name: 'iot-metrics'
    static_configs:
      - targets:
          - 'localhost:8080'
```

---

## 🌍 Target Platforms
PromUDP is optimized for devices like:
- **ESP32**: A low-cost, low-power microcontroller with Wi-Fi and Bluetooth.  
- **ESP8266**: Compact Wi-Fi modules ideal for IoT projects.  
- **Raspberry Pi Zero**: Lightweight computing with limited power requirements.  
- **STM32**: Popular microcontrollers for embedded applications.  
- **Arduino**: Suitable with UDP libraries for constrained devices.  

---

## 🛠️ Core Data Structure

The only thing you have to know to UDPSend to **PromUDP**, is the `Metric` structure represents each individual data point that is processed and exposed for Prometheus scraping. Here's a breakdown:

```c
// Metric structure
typedef struct Metric {
    char name[50];       // Metric name (e.g., "temperature")
    char tag[100];       // Associated tag or label (e.g., "room42")
    double value;        // Metric value (e.g., 22.5)
    time_t timestamp;    // UNIX timestamp of when the metric was recorded
} Metric;
```

### 🔍 Field Descriptions
1. **`name[50]`**:  
   - Stores the name of the metric, such as `temperature`, `humidity`, or `pressure`.  
   - Max length: 50 characters.

2. **`tag[100]`**:  
   - Represents labels or tags associated with the metric. These are used to differentiate between sources (e.g., `room42`, `room43`).  
   - Max length: 100 characters.

3. **`value`**:  
   - Stores the numeric value of the metric (e.g., `22.5°C` for temperature or `1013.2hPa` for pressure).  
   - Type: `double` to accommodate both integers and floating-point values.

4. **`timestamp`**:  
   - A `time_t` value representing the UNIX timestamp of when the metric was generated.  
   - Used for proper time alignment in Prometheus.

---

## 🤔 "I Still Don't Get It"

### Sometimes UDP is all you have, and it can be all you need.

Some devices, especially those with limited resources like IoT sensors or embedded systems, are designed to support **UDP only**. 

### Reasons:

- **Simplicity**: UDP is stateless, requiring fewer resources compared to TCP.  
- **Power Efficiency**: For battery-powered devices, avoiding TCP’s connection management saves energy.  
- **Hardware Constraints**: Many embedded systems cannot afford the memory or processing power to maintain a full TCP stack.  

### For example:
- **GPS trackers**: Often use UDP to send location updates with minimal delay.  
- **Environmental sensors**: Rely exclusively on UDP to send periodic data without maintaining a connection.  
- **Wearables and custom microcontrollers**: Use UDP for simplicity and low overhead.  



## 📜 License
PromUDP is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.

---

## 🌟 Contributing
Contributions are welcome! Please submit a pull request or open an issue if you'd like to help improve PromUDP.

---

## 🧠 Acknowledgments
- Inspired by the Prometheus project for redefining monitoring standards.  
- Built with love and a focus on efficient IoT solutions. ❤️  

---

### 🔗 Links
- [Prometheus Documentation](https://prometheus.io/docs/)  
- [GitHub Repository](https://github.com/your-username/PromUDP)  
