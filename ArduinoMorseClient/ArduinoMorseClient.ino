#include <Ethernet2.h>
#include <SPI.h>
#include <HttpClient.h>


// Config Variables
bool testEnvActive = true;

// Network Variables
IPAddress ipServer(192, 168, 0, 124);
IPAddress ipClient(192, 168, 0, 227);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xF8 };
EthernetClient client;

// Pin variables
const int btnPin = 2;
const int ledRed = 3;

// Arrays to store morse messages in including array index
char msgArray[255];
char tempCharArray[128];
String sMessage = "";
String tempCapture;
int currMorseArrIndex = 0;
int currMsgArrayIndex = 0;

// Time control for morse functions
long timeFirstPress = 0;
long timeLastPress = 0;
long timeLastInsert = 0;
long timeDif = 0;

// Constants for timings etc.
const long timeMaxPush = 150;
const long timeFailSafe = 20;
const long timeMaxWaitBeforeConvert = 500;
const long timeMaxWaitBeforeSend = 2000;

// Capture button state to know when clicked and held
int buttonState = 0;
int buttonLastState = 0;
int msgInProgress = 0;

// Used for morse converter
#define SIZE 26
String letters[SIZE]={

// A to I
".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..",
// J to R 
".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.",
// S to Z
"...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.." 
};
int characterAscii = 0;



void setup() {

  //Start serial for debugging
  Serial.begin(9600);

  // Start EthernetConnection
  Ethernet.begin(mac, ipClient);

  // Register when button is pressed
  pinMode(btnPin, INPUT_PULLUP);
  timeLastInsert = millis();

  // Initialize LED's
  pinMode(ledRed, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  //Read button state (pressed or not pressed?)
    buttonState = digitalRead(btnPin);
  
  
    // Logic to record time since first press and last press
    // Millis is implemented to record whether the press is within different time thresholds
    if (buttonState == LOW)
    {
      // Records when the button is pressed, and current press
      if(buttonLastState == 1) {timeFirstPress = millis();}
      else{timeLastPress = millis();}
      
      buttonLastState = 0;
    }
    // Button is released and now logic will be applied depending on how long the button was pressed.
    else
    {
      timeDif = timeLastPress - timeFirstPress;
  
      // Make sure that the button was pushed last loop and only just released.
      // Otherwise no reason to apply logic
      if(buttonLastState == 0)
      {
        // Failsafe to avoid double inputs
        if((timeLastInsert + timeFailSafe) < millis())
        {
          // Records when the last time we finished a push, to make sure that we don't do double inserts
          // because of hardware bouncing
          timeLastInsert = millis();
  
          // If button is held for less than max threshold, 
          if (timeDif < timeMaxPush)
          {
            tempCapture += ".";
            currMorseArrIndex++;
            //Serial.println(".");
            msgInProgress = 1;
            
          }
          else
          {
            tempCapture += "-";
            currMorseArrIndex++;
            //Serial.println("-");
            msgInProgress = 1;
          }



          // Uncomment this one if you want to se how long you hold the button
          //Serial.println(timeDif);
          //Serial.print(buttonState);
          Serial.println(tempCapture);
          
        }
  
      } // If enough time has passed - start converting the morse signal to a CHAR
      else if ((tempCapture != "") && ((timeLastInsert + timeMaxWaitBeforeConvert) < millis()))
      {
        ConvertTempToMessage();
      } // Send message
      else if ((sMessage != "") && ((timeLastInsert + timeMaxWaitBeforeSend) < millis()))
      {
        SendMessage(sMessage);
      }
  
      // Reset state so we the loop can function properly
      timeFirstPress = 0;
      buttonLastState = 1;
    }
  
}

// Function that converts our morse code to a char and adds it to the message that will be sent
void ConvertTempToMessage()
{
  
  sMessage += MorseToChar(tempCapture);
  tempCapture = "";
  Serial.println("Captured morse converted to message");
}

// Function to send the message. It connects to a web server and sends the content of the sMessage string.
// Once sent, it turns the LED on so that the user can see that it's sent if not looking at the terminal.
void SendMessage(String content)
{
  Serial.print("About to send");
  Serial.println();
    if (client.connect(ipServer, 80)) {
    Serial.print("connected to Server");
    Serial.println("Sending following to server: " + content);
    // Make a HTTP request:
    client.println("POST / HTTP/1.1");
    client.println("Host: 192.168.0.124:80");
    client.println("Accept: */*");
    client.println("Content-Length: " + content.length());
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println();
    client.println(content);
    client.stop();

  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  digitalWrite(ledRed, HIGH);
  delay(1000);
  digitalWrite(ledRed, LOW);
  sMessage = "";
}

// The converter function used in ConvertTempToMessage
char MorseToChar(String characterCode)
{
  
  characterAscii=65;
  
  for(int i=0; i<SIZE; i++)
  {
    //Serial.println("Char Ascii = " + characterAscii);
    if(characterCode == letters[i])
    {
      //Serial.println(letters[i]);
      return characterAscii;   
    }
    characterAscii++;  
  }

  
}


 
