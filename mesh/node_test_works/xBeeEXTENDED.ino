

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
      nss.println(" +++++  got a packet ++++" );
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
     //  nss.print("got response ");
    // should be an AT command response
  
     if (xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE) {
    
      xbee.getResponse().getAtCommandResponse(response);
   
      
      if (response.isOk()) {
        nss.print("Command [");
        nss.print(response.getCommand()[0]);
        nss.print(response.getCommand()[1]);
        nss.println("] was successful!");
        
        if (response.getValueLength() > 0) {
          nss.print("Command value length is ");
          nss.println(response.getValueLength(), DEC);
          
          nss.print("Command value: ");
          for (int i = 0; i < response.getValueLength(); i++) {
            nss.print(response.getValue()[i], HEX);
            nss.print(" ");
          }
          nss.println("");
        }
      } 
      else { // not ok
        nss.print("Command return error code: ");
        nss.println(response.getStatus(), HEX);
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

