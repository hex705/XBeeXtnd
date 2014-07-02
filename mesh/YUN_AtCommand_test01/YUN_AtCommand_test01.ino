/**
 * Copyright (c) 2009 Andrew Rapp. All rights reserved.
 *
 * This file is part of XBee-Arduino.
 *
 * XBee-Arduino is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * XBee-Arduino is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XBee-Arduino.  If not, see <http://www.gnu.org/licenses/>.
 
 
  // this one works. 
  
 */

#include <XBee.h>
#include <SoftwareSerial.h>

#include <Bridge.h>



/*
This example is for Series 1 (10C8 or later firmware) or Series 2 XBee radios
Sends a few AT command queries to the radio and checks the status response for success

This example uses the SoftSerial library to view the XBee communication.  I am using a 
Modern Device USB BUB board (http://moderndevice.com/connect) and viewing the output
with the Arduino Serial Monitor.
*/

// Define SoftSerial TX/RX pins
// Connect Arduino pin 8 to TX of usb-serial device
uint8_t ssRX = 8;
// Connect Arduino pin 9 to RX of usb-serial device
uint8_t ssTX = 9;
// Remember to connect all devices to a common Ground: XBee, Arduino and USB-Serial device
SoftwareSerial nss(ssRX, ssTX);

XBee xbee = XBee();

// serial high
uint8_t shCmd[] = {'S','H'};
// serial low
uint8_t slCmd[] = {'S','L'};
// association status
uint8_t assocCmd[] = {'N','I'};

uint8_t NI[] = {'N','I'};  // Node Identifier

uint8_t ND[] = {'N','D'};  // Node Identifier

uint8_t NAME[] = {'R','O','C','K','I','E','S'};  // set the name of the node

AtCommandRequest atRequest = AtCommandRequest(shCmd);

AtCommandResponse atResponse = AtCommandResponse();

void setup() { 
  
   
 nss.begin(38400);
  xbee.begin(nss);
  
  Bridge.begin();
  Console.begin();
  
    
  while (!Console){
    ; // wait for Console port to connect.
  }
  
  Console.println("You're connected to the Console!!!!");
 

  
  // Startup delay to wait for XBee radio to initialize.
  // you may need to increase this value if you are not getting a response
  delay(2000);
  Console.println("===start===\n");
}

void loop() {
    // get SH
  sendAtCommand();
  
  // set command to SL
  atRequest.setCommand(slCmd);  
  sendAtCommand();
  
  Console.println("=== SETINTG UP NODE IDENTIFIER ===");
  atRequest.setCommand(NI);
  atRequest.setCommandValue(NAME);
  atRequest.setCommandValueLength(sizeof(NAME));
  sendAtCommand();
  atRequest.clearCommandValue();



  // set command to AI
  atRequest.setCommand(assocCmd);  
  sendAtCommand();
   // set command to AI
  atRequest.setCommand(ND);  
  sendAtCommand();
  Console.println("\n===done===\n\n\n");
  // we're done.  Hit the Arduino reset button to start the sketch over
  while (1) {};
}

void sendAtCommand() {
  Console.println("Sending command to the XBee");

  // send the command
  xbee.send(atRequest);

  // wait up to 5 seconds for the status response
  if (xbee.readPacket(5000)) {
    // got a response!
     
    // should be an AT command response
    if (xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE) {
      xbee.getResponse().getAtCommandResponse(atResponse);

      if (atResponse.isOk()) {
        Console.print("Command [");
        Console.print(atResponse.getCommand()[0]);
        Console.print(atResponse.getCommand()[1]);
        Console.println();
         Console.write(atResponse.getCommand()[0]);
        Console.write(atResponse.getCommand()[1]);
        Console.println("] was successful!");

        if (atResponse.getValueLength() > 0) {
          Console.print("Command value length is ");
          Console.println(atResponse.getValueLength(), DEC);

          Console.print("Command value: ");
          
          for (int i = 0; i < atResponse.getValueLength(); i++) {
            Console.print(atResponse.getValue()[i], HEX);
            Console.print(" ");
          }
                    Console.println("");
          for (int i = 0; i < atResponse.getValueLength(); i++) {
            Console.write(atResponse.getValue()[i]);
     
          }

          Console.println("");
        }
      } 
      else {
        Console.print("Command return error code: ");
        Console.println(atResponse.getStatus(), HEX);
      }
    } else {
      Console.print("Expected AT response but got ");
      Console.print(xbee.getResponse().getApiId(), HEX);
    }   
  } else {
    // at command failed
    if (xbee.getResponse().isError()) {
      Console.print("Error reading packet.  Error code: ");  
      Console.println(xbee.getResponse().getErrorCode());
    } 
    else {
      Console.println("No response from radio");  
    }
  }
}


