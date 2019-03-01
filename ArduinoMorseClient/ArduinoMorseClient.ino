/**
 * @file
 * Sketch that use a button to send morse code
 *
 * @version 1st Of March 2019
 * @author Rasmus Kragh and Philip Dein
 */
 
#include <Ethernet2.h>
#include <SPI.h>
#include <HttpClient.h>

/** \brief Size of morse array as well as for loop in MorseConverter function*/
#define SIZE 26

IPAddress ipServer(10, 233, 146, 25); /** \brief The IP-adress of the server we want to communicate with */
IPAddress ipClient(10, 233, 146, 227); /** \brief Our own IP-adress */
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xF8 };/** \brief The MAC adress of our device */
EthernetClient client; /** \brief Used to connect to a webserver */

const int btnPin = 2; /** \brief Button pin */
const int ledRed = 3; /** \brief led pin */

String sMessage = ""; /** \brief Holds the message before it's being sent */
String tempCapture = ""; /** \brief Button pin */

long timeFirstPress = 0; /** \brief Used to record when button is first pressed*/
long timeLastPress = 0; /** \brief Used to record when the button was last pressed*/
long timeLastInsert = 0; /** \brief Used to record when we last inserted a char into our sMessage*/
long timeDif = 0; /** \brief Calculates difference between first and last press*/

const long timeMaxPush = 150; /** \brief Threshold in ms between '.' and '-'*/
const long timeFailSafe = 20; /** \brief if 2 button presses within 20ms ignore so we don't get double inputs*/
const long timeMaxWaitBeforeConvert = 500; /** \brief End of Morse threshold*/

int buttonState = 0; /** \brief Gets the current state of the button*/
int buttonLastState = 0; /** \brief Gets the last state of the button */
int buttonReadySend = 0; /** \brief Used to check if we're ready to send the message*/

String letters[SIZE]={
// A to I
".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..",
// J to R 
".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.",
// S to Z
"...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.." 
}; /** \brief Alphabet array for the morse code*/
int characterAscii = 0; /** \brief use to find char*/


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
            
            buttonReadySend = 0;
            
          }
          else if (timeDif < timeMaxWaitBeforeConvert)
          {
            tempCapture += "-";
            
            //Serial.println("-");
            
            buttonReadySend = 0;
          }
          else if (timeDif > timeMaxWaitBeforeConvert)
          {
            if (buttonReadySend == 1)
            {
              SendMessage(sMessage);
              buttonReadySend = 0;
            }
            else
            {
              ConvertTempToMessage();
              buttonReadySend++;
            }

          }




          // Uncomment this one if you want to se how long you hold the button
          //Serial.println(timeDif);
          //Serial.print(buttonState);
          Serial.println(tempCapture);
          
        }
  
      } // If enough time has passed - start converting the morse signal to a CHAR
//      else if ((tempCapture != "") && ((timeLastInsert + timeMaxWaitBeforeConvert) < millis()))
//      {
//        ConvertTempToMessage();
//      } // Send message
//      else if ((sMessage != "") && ((timeLastInsert + timeMaxWaitBeforeSend) < millis()))
//      {
//        Serial.println("Test");
//        SendMessage(sMessage);
//      }
  
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
    if (client.connect(ipServer, 5000)) {
    
    //char payload[50];
    //content.toCharArray(payload, 50);
    //Serial.print(payload);
    Serial.println("connected to Server");
    Serial.println("Sending following to server: " + content);
    //Serial.println(payload);

    // Make a HTTP request:
    client.println("POST /?message=" + content + " HTTP/1.1");
    Serial.println("POST /?message=" + content + " HTTP/1.1");
    client.println("host: 10.233.146.25:5000");
    client.println("user-agent: arduino/morse");
    client.println("accept: */*");
    client.println("content-length: " + content.length());
    client.println("content-type: application/x-www-form-urlencoded");
    client.println();
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


 
