# Arduino Morse Client

This project will be a project for a Arduino Morse Client.
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

``` - 1x Arduino MEGA 2560(Or something like it).
 - 1x Arduino Ethernet Shield 2
 - 1x Button
 - 1x LED
 - 2x 220v Resistor
 - Wires
```

See the ClientHwSetup.jpg for how to connect peripherals for the client.

### Installing

Download the project and save it in a fitting location. Unpack the zip file and open the project through your Arduino IDE.

Upload it to the Arduino after connecting the hardware as described in Prerequisites.

Set the IP-address for the server in the ipServer variable.
Set the IP-address for the client in the ipClient variable

### How to use

Set up your arduino and peripherals as shown in the picture.

Upload the code to the arduino and use the following button commands:
```Quick press( < 150ms ): Gives a '.'
Medium press ( < 500ms ): Gives a '-'
Long press ( > 500ms ): Converts captured morse to a Char.
2x Long press: Sends the message to the server defined in the code.
```

The LED will light up for 1 second after a message has been sent to indicate it's trying to send.
After the LED turns off you can send the next message.

The code sends a HTTP POST packet with a query saying ?message=[converted morse] to the webserver.
The webserver will then have to grab this packet and extract the query for it to be used.

### Debug messages

The Arduino will display through the serial port the morse signals that it will use depending on how long you press the button, as well as messages about converting or sending.
It will display either of the following things:
```.
-
Captured morse converted to message

**If it can connect to the server:**
About to send
connected to Server
Sending following to server: [[Captured and converted morse]]
POST /?message=E HTTP/1.1

**If it can not connect to the server**
About to send
"connection failed"
```

If you hold the button for more than 500ms and release it, it will display that it is trying to convert the currently capture morse code to a char.
After it says ***Captured morse converted to message*** you can either make a new character for the message or send it by holding the button for 500ms again.
