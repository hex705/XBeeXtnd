

// send 2 chars as ints
void sendAtCommand ( uint8_t first, uint8_t second) {
    uint8_t cmd[] = {first, second};  
    sendAtCommand ( cmd ) ;
}

//AtCommandRequest request = AtCommandRequest(command);
void sendAtCommand ( uint8_t *command ) {
        AtCommandRequest request = AtCommandRequest(command);
      sendAtCommand( request );
      
} 



void sendAtCommand ( AtCommandRequest ATCR ) {   
   // send the command
   xbee.send(ATCR);  
} 

// going to try and get a packet 

void xBeeListen(){
  
  // xbee.readPacket(5);
    
    if (xbee.getResponse().isAvailable()) {
      // got something
      Console.println(" +++++  got a packet ++++" );
//            if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
//        // got a rx packet
//        xbee.getResponse().getRx16Response(rx16);
//        data = rx16.getData(0);
       
       // set servo 1 to angle in first byte of payload
       // e.g. servo.setAngle(rx16.getData(0));
       // set servo 2 to angle in second byte of payload
       // e.g. servo2.setAngle(rx16.getData(1));
    }
      
  
}


void getAtResponse() {
  
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

