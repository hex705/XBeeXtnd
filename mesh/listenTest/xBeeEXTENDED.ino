

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

boolean xBeeListen(){
  
   xbee.readPacket(5);
    
    if (xbee.getResponse().isAvailable()) {
      // got something
      nss.println(" +++++  got a packet ++++" );
      
        if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
            // got a rx packet
            xbee.getResponse().getZBRxResponse(zbRx);
            int l = zbRx.getDataLength(); 
            nss.print("length " );
            nss.println(l);
           
             zbRxString="";
           
            if ( l > 0 ) {
              for (int i = 0; i < l; i++) {
                nss.write(zbRx.getData(i));
                zbRxString+=(char)zbRx.getData(i);
              }
             nss.println();
            }
            
            //data = rx16.getData(0);
        }
       // set servo 1 to angle in first byte of payload
       // e.g. servo.setAngle(rx16.getData(0));
       // set servo 2 to angle in second byte of payload
       // e.g. servo2.setAngle(rx16.getData(1));
       return true;
    }
      
  return false;
}
//
//
//void buildMessage() {
//  elmers.clear();
//    elmers.add(23);
//    elmers.add(4.5f);
//    elmers.add("steve is silly");
//   
//     int l = elmers.length();
//     char m[l];
//     elmers.getPackage().toCharArray(m,l);
//   
//}

void getAtResponse() {
  
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


// going to try and get a packet 

int xBeeListen2(){
  
  int mSize;
  int i=0;
   // needs a timeout of it blocks
   xbee.readPacket(5);
    // do we have some data?
    if ( xbee.getResponse().isAvailable() > 0 ) {
      // got something
      nss.println("+++++got a packet+++++" );
        // if a message go here 
        int API_TYPE = xbee.getResponse().getApiId();
        
        switch (API_TYPE) {
          
         case ZB_EXPLICIT_RX_RESPONSE:
          
          case ZB_RX_RESPONSE:
          
            xbee.getResponse().getZBRxResponse(zbRx);
            nss.println("got zbRx packet");
            nss.print("length " );
            nss.println(zbRx.getDataLength());
 
            zbRxString = "";
            if ( API_TYPE == ZB_EXPLICIT_RX_RESPONSE ) i = 6;
              for (i ; i < zbRx.getDataLength(); i++) {
               char c = zbRx.getData(i);
                zbRxString += c ;
              }
              nss.print("raw: " ); 
            nss.println(zbRxString);
            return ZB;
              
          break;
          
          case ZB_TX_STATUS_RESPONSE:
            xbee.getResponse().getZBTxStatusResponse(zbSR);
            nss.print("Status response :: " );
            nss.println( zbSR.isSuccess());
          break;

          
          case AT_COMMAND_RESPONSE:
            xbee.getResponse().getAtCommandResponse(atResponse);
            if (atResponse.isOk()) {
              // have a good messge 
                if ( (atResponse.getCommand()[0] == 'N') && (atResponse.getCommand()[1] = 'D' )) {
                   STORE = true;
                   zbRxString = "";
                }  
                
                if (atResponse.getValueLength() > 0) {
                    nss.println("Got AT packet");
                    nss.print("Command value length is ");
                    nss.println(atResponse.getValueLength(), DEC);
                    nss.print("Command value: ");
                   
                    for (int i = 0; i < atResponse.getValueLength(); i++) {
                     int val = atResponse.getValue()[i];
                     
                      if ( (STORE) && ( i >= 2 ) && ( i <= 9) ) tempAddress[i-2] = val; 
                      nss.print(val, HEX);
                      nss.print(" ");
   
                    }
                if (STORE) storeAddress();
                nss.println("");
                }
               STORE = false;
               nss.println("\n\rDONE");
              return AT;
            }
          
          break;
          
        } // end switch
        
   
    }  // no data available  
   return ERR;
}

void storeAddress() {
 
 // have address in a string of hex values

    uint32_t msb = ((tempAddress[0] & 0xFFl) << 24) | 
               ((tempAddress[1] & 0xFFl) << 16) |
               ((tempAddress[2] & 0xFFl) <<  8) |
               ((tempAddress[3] & 0xFFl) <<  0);
    
    uint32_t lsb = ((tempAddress[4] & 0xFFl) << 24) | 
               ((tempAddress[5] & 0xFFl) << 16) |
               ((tempAddress[6] & 0xFFl) <<  8) |
               ((tempAddress[7] & 0xFFl) <<  0);

    
 
  
  nss.print ( "(*)(*)(*)\tmsb = 0x");
  nss.print ( msb, HEX );
  nss.print ("\tlsb = 0x");
  nss.println (lsb, HEX );
  
  // now add to arraylist
  
  XBeeAddress64 node;
   node.setMsb( msb ); 
   node.setLsb( lsb );
   nss.println(node.getMsb(),HEX);
  nodeList.add(node);
  
} // end store address

