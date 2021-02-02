Using a NodeMCU ESP8266 to change lights with a customized web application hosted on the ESP8266.

### Features
- Lights on/off
- Change color
- Morning light (Set a wake up time and it will slowly change the lights color until it is bright white)

### Stuff to change before use
- Change SSID & password
- Change `NTP_ADDRESS` to your region
- Give it a static IP (recommended)
- Change ports (optional)

### Example video
https://www.youtube.com/watch?v=qLgzeUmqXxM

### Schemetics / PCB layout
![PCB layout](https://i.imgur.com/D3nU7sZ.png)

### Parts used
- NodeMCU ESP8266
- 3x 220Ω resistors
- 3x IRLZ44N transistors
- RGB-5v separated LED strip (No data pin)

### Warning
You have to modify the code a bit if you are using LED strips with "data" pin 
(ws2812/ws2812b) instead of RGB separated pin.
