#include <Glue.h>


/*
 Created 23 November 2009
 By Michael J Sepcot (michael.sepcot@gmail.com)
 
 https://github.com/msepcot/arduino_samples/blob/master/XBeeNodeDiscovery/XBeeNodeDiscovery.pde
 
 === Xbee Node Discovery ===
 
 In this project we setup our Xbee module with a Node Identifier and query
 for other nodes in our vicinity. Record the number of responses to our
 Node Discover command and flash the results on the LED on pin 13.
 
 == The Circuit ==
 
 This project makes use of the LED connected from digital pin 13 to ground
 found on most Arudino boards and the XBee Shield and Module.
 
 Information about the Xbee Shield and Arduino connections can be found here:
 
   http://www.arduino.cc/en/Main/ArduinoXbeeShield
 
 Information about the Xbee Module can be found here:
 
   http://www.digi.com/products/wireless/zigbee-mesh/xbee-zb-module.jsp
 
 == External Library ==
 
 This project makes use of the xbee-arduino library for communicating with
 Xbees in API mode. More information can be found here:
 
   http://code.google.com/p/xbee-arduino/
 
*/
#include <XBee.h>
#include <SoftwareSerial.h>

#define DEBUG 1  // if defined, print debug info to serial

// Define SoftSerial TX/RX pins
// Connect Arduino pin 8 to TX of usb-serial device
uint8_t ssRX = 8;
// Connect Arduino pin 9 to RX of usb-serial device
uint8_t ssTX = 9;

// Remember to connect all devices to a common Ground: XBee, Arduino and USB-Serial device
SoftwareSerial nss(ssRX, ssTX);

XBee xbee = XBee();
//AtCommandRequest request = AtCommandRequest();
//AtCommandResponse response = AtCommandResponse();

uint8_t SH[] = {'S','H'};  // Serial Number High
uint8_t SL[] = {'S','L'};  // Serial Number Low
uint8_t NI[] = {'N','I'};  // Node Identifier
uint8_t CH[] = {'C','H'};  // Operating Channel
uint8_t OP[] = {'O','P'};  // Operating Extended PAN ID
uint8_t OI[] = {'O','I'};  // Operating PAN ID
uint8_t ND[] = {'N','D'};  // Node Discover
uint8_t NT[] = {'N','T'};  // Node Discover Timeout

AtCommandRequest atRequest = AtCommandRequest(SH);
AtCommandResponse atResponse = AtCommandResponse();

ZBRxResponse zbRx = ZBRxResponse();
ZBTxRequest  zbTx = ZBTxRequest();

ZBTxStatusResponse zbSR = ZBTxStatusResponse();

XBeeAddress64 dst1 = XBeeAddress64(0x13A200, 0x409878DD);
XBeeAddress64 dst2 = XBeeAddress64(0x13A200, 0x4054F0F7);

XBeeAddress64 src = XBeeAddress64(0x13A200, 0x40311E08);

String zbRxString, zbTxString;


uint8_t NAME[] = {'F','R','O','G','I','E','S'};  // set the name of the node

int ledPin =  13;
int timeout = 5000;  // default value of NT (if NT command fails)

long t, threshold;

Glue elmers;


#define ERR 0
#define ZB 1
#define AT 2

boolean STORE = false;

#define button 4
long lastTime=8000;

void setup() {

  
 Serial.begin(38400);
  xbee.begin(Serial);
  
    nss.begin(9600);
    
  pinMode(ledPin, OUTPUT);
  elmers.create();
  
   pinMode ( button, INPUT);
   delay(2000);
   nss.println("===start ( send test ) ===");
  sendAtCommand('S', 'L');
  delay(1000);
 
}

void loop() {
  

  xBeeListen2();
 
 
  if ( digitalRead ( button ) == HIGH ) {
    // send a message that says you do this
    elmers.clear();
    elmers.add(87);
    elmers.add("ND");
    sendZB( dst1,  elmers.getPackage() );
    delay(1000);
    lastTime = millis(); // resrt clock to not mess up search 
  } // end if 
  
  
  if ( ( millis() - lastTime) > 10000) { 
     buildMessage(dst1,1) ;
     delay(250);
     lastTime = millis();
 }
}




void sendAtCommand() {
 // nss.print("Sending command " );

  //nss.println("to the XBee yay");
  
  // send the command
  xbee.send(atRequest);
  
  // wait up to 1 second for the status response
  // we are just using this function for local queries, should be quick
  if (xbee.readPacket(5000)) {
    // got a response!
     //  nss.print("got response ");
    // should be an AT command response
    if (xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE) {
    
      xbee.getResponse().getAtCommandResponse(atResponse);
   
      
      if (atResponse.isOk()) {
        nss.print("Command [");
        nss.print(atResponse.getCommand()[0]);
        nss.print(atResponse.getCommand()[1]);
        nss.println("] was successful!");
        
        if (atResponse.getValueLength() > 0) {
          nss.print("Command value length is ");
          nss.println(atResponse.getValueLength(), DEC);
          
          nss.print("Command value: ");
          for (int i = 0; i < atResponse.getValueLength(); i++) {
            nss.print(atResponse.getValue()[i], HEX);
            nss.print(" ");
          }
          nss.println("");
        }
      } 
      else { // not ok
        nss.print("Command return error code: ");
        nss.println(atResponse.getStatus(), HEX);
      }
    } else {
      nss.print("Expected AT response but got ");
      nss.println(xbee.getResponse().getApiId(), HEX);
    }   
  } else {
    // command failed
    if (xbee.getResponse().isError()) {
      nss.print("Error reading packet.  Error code: ");  
      nss.println(xbee.getResponse().getErrorCode(), DEC);
    } 
    else {
      nss.println("No response from radio");  
    }
  }
  nss.println("");
}

