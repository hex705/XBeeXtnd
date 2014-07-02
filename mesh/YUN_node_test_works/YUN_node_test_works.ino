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

#include <Bridge.h>

#define DEBUG  // if defined, print debug info to serial

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


AtCommandRequest requestA = AtCommandRequest(SH);
AtCommandResponse response = AtCommandResponse();
AtCommandResponse responseR = AtCommandResponse();
//RemoteAtCommandResponse responseR = RemoteAtCommandResponse();

uint8_t NAME[] = {'F','R','O','G','I','E','S'};  // set the name of the node

int ledPin =  13;
int timeout = 10000;  // default value of NT (if NT command fails)

void setup() {

  
 //Serial.begin(38400);
 
 nss.begin(38400);
  xbee.begin(nss);
  
  Bridge.begin();
  Console.begin();
  
    
  while (!Console){
    ; // wait for Console port to connect.
  }
  
  Console.println("You're connected to the Console!!!!");
    
  pinMode(ledPin, OUTPUT);
  
  
  delay(2000);
  Console.println("===start ( no work) ===");
}

void loop() {
  
 // xbeeListen();
  
  int nodeCount = 0;
  
  sendAtCommand(SH);
  getAtResponse();
  
  
  Console.println("=== SETINTG UP NODE IDENTIFIER ===");
  requestA.setCommand(NI);
  requestA.setCommandValue(NAME);
  requestA.setCommandValueLength(sizeof(NAME));
  sendAtCommand();
  requestA.clearCommandValue();
  
  #ifdef DEBUG
      Console.println("=== DEBUG INFO ===");
      requestA.setCommand(SH);
      sendAtCommand();
      requestA.setCommand(SL);
      sendAtCommand();
      requestA.setCommand(NI);
      sendAtCommand();
      requestA.setCommand(CH);
      sendAtCommand();
      requestA.setCommand(OI);
      sendAtCommand();
      requestA.setCommand(OP);
      sendAtCommand();
      Console.println("=== END DEBUG INFO ===\n\n\n");
  #endif
  
  Console.println("=== Node Discovery ===");
  
  // get the Node Discover Timeout (NT) value and set to timeout
  requestA.setCommand(NT);
  Console.print("Sending NT  command to the XBee ");
  xbee.send(requestA);

  
  while(xbee.readPacket(5000)) {
  if (xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE) {
    xbee.getResponse().getAtCommandResponse(responseR);
    if (response.isOk()) {
      if (response.getValueLength() > 0) {
        Console.print("print raw TI val ");
        Console.println(response.getValue()[0]);
        Console.println(response.getValue()[1]);
        if (response.getValueLength() > 2) {
          Console.println(response.getValue()[2]);
        }
        
        // NT response range should be from 0x20 - 0xFF, but
        // I see an inital byte set to 0x00, so grab the last byte
        timeout = response.getValue()[response.getValueLength() - 1] * 100;
        Console.print("calc ti " );
        Console.println(timeout);
      }
    }
  }
   }
  
  
  requestA.setCommand(ND);
  Console.print("Sending ND command to the XBee\n\r");
  xbee.send(requestA);
  
  while(xbee.readPacket(timeout)) {
    // should be receiving AT command responses
    if (xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE) {
      xbee.getResponse().getAtCommandResponse(responseR);
      if (responseR.isOk()) {
        nodeCount++;
          for (int i = 0; i < responseR.getValueLength(); i++) {
            Console.print(response.getValue()[i], HEX);
            Console.print(" ");
          }
          Console.println();
        // https://github.com/ivanseidel/LinkedList
       // XBeeAddress64 xbA  = responseR.getRemoteAddress64();
       // Console.println(xbA.getLsb());
      }
      Console.println();
    }
  }

  Console.print("Results: ");
  Console.print(nodeCount, DEC);
  Console.println(" node(s) responded.");
  Console.println("");
  
  // flash results
  delay(2000);  // wait 2 seconds for the user to look at the board...
  for(int i = 0; i < nodeCount; i++) {
    digitalWrite(ledPin, HIGH);  // set the LED on
    delay(500);
    digitalWrite(ledPin, LOW);   // set the LED off
    delay(500);
  }
  
  // hit the Arduino reset button to start the sketch over
  while(1) {};
}




void sendAtCommand() {
 // Console.print("Sending command " );

  //Console.println("to the XBee yay");
  
  // send the command
  xbee.send(requestA);
  
  // wait up to 1 second for the status response
  // we are just using this function for local queries, should be quick
  if (xbee.readPacket(5000)) {
    // got a response!
     //  Console.print("got response ");
    // should be an AT command response
    if (xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE) {
    
      xbee.getResponse().getAtCommandResponse(response);
   
      
      if (response.isOk()) {
        Console.print("Command [");
        Console.print(response.getCommand()[0]);
        Console.print(response.getCommand()[1]);
        Console.println("] was successful!");
        
        if (response.getValueLength() > 0) {
          Console.print("Command value length is ");
          Console.println(response.getValueLength(), DEC);
          
          Console.print("Command value: ");
          for (int i = 0; i < response.getValueLength(); i++) {
            Console.print(response.getValue()[i], HEX);
            Console.print(" ");
          }
          Console.println("");
        }
      } 
      else { // not ok
        Console.print("Command return error code: ");
        Console.println(response.getStatus(), HEX);
      }
    } else {
      Console.print("Expected AT response but got ");
      Console.println(xbee.getResponse().getApiId(), HEX);
    }   
  } else {
    // command failed
    if (xbee.getResponse().isError()) {
      Console.print("Error reading packet.  Error code: ");  
      Console.println(xbee.getResponse().getErrorCode(), DEC);
    } 
    else {
      Console.println("No response from radio");  
    }
  }
  Console.println("");
}

