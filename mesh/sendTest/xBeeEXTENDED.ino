//// send 2 chars as ints
//void sendAtCommand ( uint8_t first, uint8_t second, int *payload) {
//  
//    uint8_t cmd[] = {first, second};  
//    sendAtCommand ( cmd ) ;
//}
//
//// send 2 chars as ints
//void sendAtCommand ( uint8_t first, uint8_t second, int payload) {
//  
//    uint8_t cmd[] = {first, second};  
//    sendAtCommand ( cmd ) ;
//}
//
//// send 2 chars as ints
//void sendAtCommand ( uint8_t first, uint8_t second) {
//    uint8_t cmd[] = {first, second};  
//    sendAtCommand ( cmd ) ;
//}
//
////AtCommandRequest request = AtCommandRequest(command);
//void sendAtCommand ( uint8_t *command ) {
//      AtCommandRequest request = AtCommandRequest(command);
//      sendAtCommand( request ); 
//} 
//
//
//void sendAtCommand ( AtCommandRequest ATCR ) {   
//   // send the command
//   xbee.send(ATCR);  
//   nss.println("at sent");
//} 


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
  
   // needs a timeout of it blocks
   xbee.readPacket(5);
    // do we have some data?
    if (xbee.getResponse().isAvailable()) {
      // got something
      nss.println("+++++got a packet+++++" );
        // if a message go here 
        
        if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
            // got a rx packet
            xbee.getResponse().getZBRxResponse(zbRx);
            int l = zbRx.getDataLength(); 
            nss.print("length " );
            nss.println(l);
            if ( l > 0 ) {
              for (int i = 0; i < l; i++) {
                nss.write(zbRx.getData(i));
              }
             nss.println();
            }
             return true;
        } // got some data
        
      
    }  // no data available  
   return false;
}

// send API ZB message to dst
void sendZB(XBeeAddress64 dst, String payload) {
     
  if ( DEBUG ) {
    nss.print( "payload " );
    nss.println(payload);
  }
     
     int len = payload.length()+1;
     unsigned char temp[len];
     payload.getBytes(temp,len);
     
     zbTx = ZBTxRequest( dst, temp, len );
     
     xbee.send(zbTx);
  
}

void buildMessage(XBeeAddress64 who, int payload) {
  elmers.clear();
    elmers.add(23);
    elmers.add(4.5f);
    if (payload == 2) {
    elmers.add("silly was slim");
    } else {
      elmers.add("steve is silly");
    }
      
      // convert String to byte buffer
     String s = elmers.getPackage();
     nss.println(s);
     int l = s.length()+1;
     unsigned char m[l];
     s.getBytes(m,l);
     
     zbTx = ZBTxRequest( who, m,l);
     
   xbee.send(zbTx);
   
}

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

boolean xBeeListen2(){
  
  int mSize;
  int i=0;
   // needs a timeout of it blocks
   xbee.readPacket(5);
    // do we have some data?
    if ( xbee.getResponse().isAvailable() > 0 ) {
      // got something
      //nss.println("+++++got a packet+++++" );
        // if a message go here 
        int API_TYPE = xbee.getResponse().getApiId();
        
       //  nss.println(API_TYPE);
        
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
            STORE = false;
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
                   
                    for (i = 0; i < atResponse.getValueLength(); i++) {
                     int val = atResponse.getValue()[i];
                      if ((STORE) && ( i >= 2 ) && ( i <= 9)) zbRxString += val;
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
 
 long msb, lsb; 
  if ( zbRxString.length() == 8 ) {  
    
    for (int i = 0; i<4; i++) {
        msb << (8*i);
        msb += (int) zbRxString.charAt(i);
    }
    for (int i = 4; i<8; i++) {
      lsb << (8*i);
      lsb += (int) zbRxString.charAt(i);
    }
    
   
    for ( int i = 0; i < 8; i++) {
      
      while ( i < 4) {
        
      }
      
    }
    
  XBeeAddress64 node = XBeeAddress64( msb , lsb);
  nss.print ( "(*)(*)(*)\tmsb = ");
  nss.print ( msb, HEX );
  nss.print ("\tlsb = ");
  nss.println (lsb, HEX );
  }
  // now add to arraylist
  
} // end store address

