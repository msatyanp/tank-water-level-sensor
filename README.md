 ## Tank Level sensor

This is an IOT project to check and notify water level to the user using NodeMCU, Arduino and Blink app
### How the tool works

We put the microcontroller in the water Pump power supply section with Relay.If water switch in trigger(FULL) then it will cut of the relay power(Machine off) with Buzzer indicating the water tank is full. We use blynk to monitoring the value. The microcontroller which connected to internet connection trigger ON/OFF to blynk cloud and the cloud will send the data to our app(remotely turn on/off the machine).

### Hardware Component
```bash
ESP8266 module
USB Cable
Water switch
Buzzer
Wires
Button
5V Relay switch
```

### Software and Libraries
```bash
Arduino IDE
DHTesp
BlynkSimpleEsp8266
SimpleTimer
```

### Further info
[Satya](https://satya.com.np/)
