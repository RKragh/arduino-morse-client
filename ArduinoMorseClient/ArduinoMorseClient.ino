/**
 * @file
 * Sketch that use a button to send morse code
 *
 * @version 29th February 2019
 * @author Rasmus Kragh and Philip Dein
 */
 
#include <Ethernet2.h>
#include <SPI.h>
#include <HttpClient.h>

/**
 * Variables used for networking
 */
IPAddress ipServer(192, 168, 0, 124);
IPAddress ipClient(192, 168, 0, 227);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xF8 };
EthernetClient client;

/**
 * Peripheral mapping
 */
const int btnPin = 2;
const int ledRed = 3;

/**
 * Strings to hold morse code and chars
 */
String sMessage = "";
String tempCapture = "";

/**
 * Time functions used in void Loop() to keep track of button presses.
 */
long timeFirstPress = 0;
long timeLastPress = 0;
long timeLastInsert = 0;
long timeDif = 0;

/**
 * Time constants used in button logic to find out if we do '.' or '-'
 * Also used for setting time before converting morse to char and when to send
 */
const long timeMaxPush = 150;
const long timeFailSafe = 20;
const long timeMaxWaitBeforeConvert = 500;
const long timeMaxWaitBeforeSend = 2000;

/**
 * Variables used to check button state
 */
int buttonState = 0;
int buttonLastState = 0;
int msgInProgress = 0;

/**
 * Used for the MorseConverter function
 */
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


/**
 * Standard function that runs on startup. We initialize the Serial port to be able to see terminal
 * We also start the Ethernet class so that we can communicate on the network
 * We set button to register button releases. Set a timer on timeLastInsert, this is to prevent weird happenings.
 */
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
            
            //Serial.println(".");
            msgInProgress = 1;
            
          }
          else
          {
            tempCapture += "-";
            
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

/**
 * Function that takes the current series of morse inputs and converts them to the char.
 * It uses the array of strings initalized before void start().
 * For debugging purposes it prints that it has finished converting.
 */
void ConvertTempToMessage()
{
  
  sMessage += MorseToChar(tempCapture);
  tempCapture = "";
  Serial.println("Captured morse converted to message");
}
/** 
 * Function to send the message. It connects to a web server and sends the content of the sMessage string.
 * Once sent, it turns the LED on so that the user can see that it's sent if not looking at the terminal. 
 * 
 * @param String content the content store in sMessage
 */
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

/**
 * The converter function used in ConvertTempToMessage
 * 
 * @param String characterCode The morse inputs. It will be found in the array and the index + characterAscii equals Capital letter.
 */
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


 
