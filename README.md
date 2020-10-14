# It is still work in progress

The ESP8266 returns a JSON after making a request. I recommended you to a static IP for your ESP8266. You can also use "Postman" to make API request.

Changing colors might not be working at the moment.

---

This is basically the schematic that I used (D5, D6 and D7 on my nodemcu esp8266 v1)

![ESP8266 schematic](https://i0.wp.com/randomnerdtutorials.com/wp-content/uploads/2019/03/ESP32-LED-strip-schematic_f.png)


You have to modify the code a bit if you are using LED strips with "data" pin 
(ws2812/ws2812b) instead of RGB separated pin
