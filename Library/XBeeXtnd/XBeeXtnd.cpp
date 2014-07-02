
// XBeeXtnd helper library
// two versions -- debug which utilizes LCD calls
// no LCD so you fly a bit blind

#include <XBeeXtnd.h>

#include <WString.h> // String


//void XBeeXtnd::xtCreate(int _i1, int _i2, int _d1, int _d2, int _d3, int _d4 )
void XBeeXtnd::xtCreate(XBee &_xbee)
{
	debugString = "";

	xbee = new XBee( _xbee);
	timeOut = 5;
	
	AtCommandRequest  atRequest  = AtCommandRequest();
	AtCommandResponse atResponse = AtCommandResponse();

	ZBRxResponse zbRx = ZBRxResponse(); // for receiving messages
	ZBTxRequest  zbTx = ZBTxRequest();  // for sending messages

	ZBTxStatusResponse zbSR = ZBTxStatusResponse(); // 

	XBeeAddress64 dst = XBeeAddress64(); 
	XBeeAddress64 src = XBeeAddress64(); // not used 
	
	LinkedList<XBeeAddress64> nodeList = LinkedList<XBeeAddress64>();
	clearList();
	
	DEBUG_STATE = 0;
	
	debugString = "xt:: end create";

	
}


void XBeeXtnd::clearList(){
	nodeList.clear();
}



void XBeeXtnd::storeAddress() {
 
 // have address in a string of hex values

   // those are 'l' == L not 1's after the FF
    uint32_t msb = ((tempAddress[0] & 0xFFl) << 24) | 
               ((tempAddress[1] & 0xFFl) << 16) |
               ((tempAddress[2] & 0xFFl) <<  8) |
               ((tempAddress[3] & 0xFFl) <<  0);
    
    uint32_t lsb = ((tempAddress[4] & 0xFFl) << 24) | 
               ((tempAddress[5] & 0xFFl) << 16) |
               ((tempAddress[6] & 0xFFl) <<  8) |
               ((tempAddress[7] & 0xFFl) <<  0);

  
  // now add to arraylist
  
  
   XBeeAddress64 node;
   node.setMsb( msb ); 
   node.setLsb( lsb );
 
   nodeList.add(node);
  
} // end store address



void XBeeXtnd::sendZB(XBeeAddress64 dst, String payload) {
     
     
     int len = payload.length()+1;
     unsigned char temp[len];
     payload.getBytes(temp,len);
     
     zbTx = ZBTxRequest( dst, temp, len );
     
     xbee->send(zbTx);
  
} // end sendZB



int XBeeXtnd::xtListen(){
	
  debugString = "";
  int mSize;
  int i=0;

  debugString = "xt::WT";
  

   // needs a timeout of it blocks
   xbee->readPacket(timeOut); // was 5 
    // do we have some data?
    // this data can come from local microcontroller OR remote radio

    if ( xbee->getResponse().isAvailable() > 0 ) {
      // got something
		debugString +=",GM";
		
        // if a message go here 
        int API_TYPE = xbee->getResponse().getApiId();

        switch (API_TYPE) {

         case ZB_EXPLICIT_RX_RESPONSE:
         case ZB_RX_RESPONSE:

            xbee->getResponse().getZBRxResponse(zbRx);
          
            zbRxString = "";
            if ( API_TYPE == ZB_EXPLICIT_RX_RESPONSE ) i = 6;
              for (i ; i < zbRx.getDataLength(); i++) {
               char c = zbRx.getData(i);
                zbRxString += c ;
              }

			debugString +=",ZB";
            return ZB; // 1

          break;

          case ZB_TX_STATUS_RESPONSE:
            xbee->getResponse().getZBTxStatusResponse(zbSR);
			status = zbSR.isSuccess();
			debugString +=",TX";
			return ZBSR; // 2
          break;


          case AT_COMMAND_RESPONSE:
            xbee->getResponse().getAtCommandResponse(atResponse);
            if (atResponse.isOk()) {
			  
              // have a good messge 

			    //  if the command was ND we need to do some work to capture the list
			    //  this sets us up to store know nodes in the linked list. 
                if ( (atResponse.getCommand()[0] == 'N') && (atResponse.getCommand()[1] = 'D' )) {
                   STORE_NODE_ADDRESS = 1;
                   zbRxString = "";
                }  

                if (atResponse.getValueLength() > 0) {
                    
                    for (int i = 0; i < atResponse.getValueLength(); i++) {
	
                      int val = atResponse.getValue()[i];
                      if ( (STORE_NODE_ADDRESS) && ( i >= 2 ) && ( i <= 9) ) tempAddress[i-2] = val;  
					  zbRxString += char(val);
					
                    }

					// have to deal with this next 
                    if (STORE_NODE_ADDRESS) storeAddress();
               
                }

               STORE_NODE_ADDRESS = 0;
               debugString +=",AT";
              return AT; // 3
            }

          break;

        } // end switch

	

    }  // no data available  
   	debugString +=",ND";
   return ERR; // 0

} // end listen




// send 2 chars as ints
void XBeeXtnd::xtSendAtCommand ( uint8_t first, uint8_t second) {
    uint8_t cmd[] = {first, second};  
    xtSendAtCommand ( cmd ) ;
}

//AtCommandRequest request = AtCommandRequest(command);
void XBeeXtnd::xtSendAtCommand ( uint8_t *command ) {
	  
      AtCommandRequest request = AtCommandRequest(command);
      xtSendAtCommand( request );
      
} 


void XBeeXtnd::xtSendAtCommand ( AtCommandRequest ATCR ) {   
   // send the command

   xbee->send(ATCR);  
   debugString ="SD AT";
}

void XBeeXtnd::xtDebug(int state ) {
	DEBUG_STATE = state;
}

int XBeeXtnd::getStatus(){
	return status;
}





void XBeeXtnd::setTimeout( int to) {
	timeOut = to;
	debugString = "set TO";
	
}


