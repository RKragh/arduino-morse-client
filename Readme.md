# Arduino Morse Client

This project will be a library for a Arduino Morse Client.
It will be able to send a morse signal made from button presses through the TCP protocol,
a server will afterwards be able to receive and display the signal.
In the example we use an Arduino MEGA 2560 and an Arduino Ethernet Shield 2.

## Getting Started

Download this project, unpack the zip and load it up in the Arduino IDE of your choice.

If you do not already have an Arduino IDE installed you can get the latest from the following website:
https://www.arduino.cc/en/Main/Software

In this example we're using Arduino v1.8.8.

### Prerequisites

To get started you need the following hardware:

``` - 2x Arduino MEGA 2560(Or something like it).
 - 2x Arduino Ethernet Shield 2
 - 1x Button
 - Wires
```

See the ClientHwSetup.jpg for how to connect peripherals for the client.
See the ServerHwSetup.jpg for how to connect peripherals for the server.

### Installing

Download the project and save it in a fitting location. Unpack the zip file and open the project through your Arduino IDE.

Upload it to the Arduino after connecting the hardware as described in Prerequisites.

For testing purposes you will find a setting that while true, it will output morse signal to terminal. Once you set it to false it will send it to the given IP-address.
```
bool testEnvActive = true;
IPAddress ip(192.168.1.1);
```